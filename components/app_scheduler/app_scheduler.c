/* Copyright (c) 2012 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "board_config.h"
#include "app_scheduler.h"


/**@brief Function for incrementing a queue index, and handle wrap-around.
 *
 * @param[in]   index   Old index.
 *
 * @return      New (incremented) index.
 */
static __INLINE uint8_t next_index(app_scheduler_t * p_sched, uint8_t index)
{
    return (index < p_sched->m_queue_size) ? (index + 1) : 0;
}


static __INLINE uint8_t app_sched_queue_full(app_scheduler_t * p_sched)
{
  uint8_t tmp = p_sched->m_queue_start_index;
  return next_index(p_sched, p_sched->m_queue_end_index) == tmp;
}

/**@brief Macro for checking if a queue is full. */
#define APP_SCHED_QUEUE_FULL(x) app_sched_queue_full(x)


static __INLINE uint8_t app_sched_queue_empty(app_scheduler_t * p_sched)
{
  uint8_t tmp = p_sched->m_queue_start_index;
  return p_sched->m_queue_end_index == tmp;
}

/**@brief Macro for checking if a queue is empty. */
#define APP_SCHED_QUEUE_EMPTY(x) app_sched_queue_empty(x)


int app_sched_init(app_scheduler_t * p_sched, uint16_t event_size, uint16_t queue_size, void * p_event_buffer)
{
    uint16_t data_start_index = (queue_size + 1) * sizeof(event_header_t);

    // Check that buffer is correctly aligned
    if (!is_word_aligned(p_event_buffer))
    {
        return -EINVAL;
    }

    // Initialize event scheduler
    p_sched->m_queue_event_headers = p_event_buffer;
    p_sched->m_queue_event_data    = &((uint8_t *)p_event_buffer)[data_start_index];
    p_sched->m_queue_end_index     = 0;
    p_sched->m_queue_start_index   = 0;
    p_sched->m_queue_event_size    = event_size;
    p_sched->m_queue_size          = queue_size;

    p_sched->m_max_queue_utilization = 0;

//    p_sched->m_SemHandle = osSemaphoreNew(1, 1, NULL);

    return ENONE;
}

static void queue_utilization_check(app_scheduler_t * p_sched)
{
    uint16_t start = p_sched->m_queue_start_index;
    uint16_t end   = p_sched->m_queue_end_index;
    uint16_t queue_utilization = (end >= start) ? (end - start) :
        (p_sched->m_queue_size + 1 - start + end);

    if (queue_utilization > p_sched->m_max_queue_utilization)
    {
        p_sched->m_max_queue_utilization = queue_utilization;
    }
}

uint16_t app_sched_queue_utilization_get(app_scheduler_t * p_sched)
{
    return p_sched->m_max_queue_utilization;
}

int app_sched_event_put(app_scheduler_t              * p_sched,
                             void                    * p_event_data,
                             uint16_t                  event_data_size,
                             app_sched_event_handler_t handler)
{
    int err_code;

    if (event_data_size <= p_sched->m_queue_event_size)
    {
        uint16_t event_index = 0xFFFF;

        // CRITICAL_REGION_ENTER();
//        osSemaphoreAcquire(p_sched->m_SemHandle, 0);

        if (!APP_SCHED_QUEUE_FULL(p_sched))
        {
            event_index       = p_sched->m_queue_end_index;
            p_sched->m_queue_end_index = next_index(p_sched, p_sched->m_queue_end_index);

            // This function call must be protected with critical region because
            // it modifies 'p_sched->m_max_queue_utilization'.
            queue_utilization_check(p_sched);
        }

        // CRITICAL_REGION_EXIT();
//        osSemaphoreRelease(p_sched->m_SemHandle);

        if (event_index != 0xFFFF)
        {
            // NOTE: This can be done outside the critical region since the event consumer will
            //       always be called from the main loop, and will thus never interrupt this code.
            p_sched->m_queue_event_headers[event_index].handler = handler;
            if ((p_event_data != NULL) && (event_data_size > 0))
            {
                memcpy(&p_sched->m_queue_event_data[event_index * p_sched->m_queue_event_size],
                       p_event_data,
                       event_data_size);
                p_sched->m_queue_event_headers[event_index].event_data_size = event_data_size;
            }
            else
            {
                p_sched->m_queue_event_headers[event_index].event_data_size = 0;
            }

            err_code = ENONE;
        }
        else
        {
            err_code = -ENOMEM;
        }
    }
    else
    {
        err_code = -EINVAL;
    }

    return err_code;
}


/**@brief Function for reading the next event from specified event queue.
 *
 * @param[out]  pp_event_data       Pointer to pointer to event data.
 * @param[out]  p_event_data_size   Pointer to size of event data.
 * @param[out]  p_event_handler     Pointer to event handler function pointer.
 *
 * @return      NRF_SUCCESS if new event, NRF_ERROR_NOT_FOUND if event queue is empty.
 */
static int app_sched_event_get(app_scheduler_t                * p_sched,
                                    void                     ** pp_event_data,
                                    uint16_t *                  p_event_data_size,
                                    app_sched_event_handler_t * p_event_handler)
{
    int err_code = ECHILD;

    if (!APP_SCHED_QUEUE_EMPTY(p_sched))
    {
        uint16_t event_index;

        // NOTE: There is no need for a critical region here, as this function will only be called
        //       from app_sched_execute() from inside the main loop, so it will never interrupt
        //       app_sched_event_put(). Also, updating of (i.e. writing to) the start index will be
        //       an atomic operation.
        event_index         = p_sched->m_queue_start_index;
        p_sched->m_queue_start_index = next_index(p_sched, p_sched->m_queue_start_index);

        *pp_event_data     = &p_sched->m_queue_event_data[event_index * p_sched->m_queue_event_size];
        *p_event_data_size = p_sched->m_queue_event_headers[event_index].event_data_size;
        *p_event_handler   = p_sched->m_queue_event_headers[event_index].handler;

        err_code = ENONE;
    }

    return err_code;
}


void app_sched_execute(app_scheduler_t * p_sched)
{
    void                    * p_event_data;
    uint16_t                  event_data_size;
    app_sched_event_handler_t event_handler;

    // Get next event (if any), and execute handler
    while ((app_sched_event_get(p_sched, &p_event_data, &event_data_size, &event_handler) == ENONE))
    {
        event_handler(p_event_data);
    }
}
