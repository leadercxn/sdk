#include "24cxx.h"
#include "trace.h"

uint8_t eeprom_check(i2c_object_t *p_i2c_obj)
{
	uint8_t temp;
    uint8_t value = 0x55;

    eeprom_read_byte(p_i2c_obj, 255, &temp);        //避免每次开机都写AT24CXX	
    trace_info("temp = 0x%x\r\n", temp);
	if (temp == 0x55)
    {
        return 0;	
    }	   
	else    //排除第一次初始化的情况
	{
        eeprom_read_byte(p_i2c_obj, 255, &value);
        eeprom_read_byte(p_i2c_obj, 255, &temp);
		if (temp == 0x55)
        {
            return 0;
        }
	}
	return 1;	
}

uint8_t eeprom_init(i2c_object_t *p_i2c_obj)
{
    p_i2c_obj->i2c_ops.init(&p_i2c_obj->i2c_cfg);

    return 0;
}

uint8_t eeprom_write_byte(i2c_object_t *p_i2c_obj, uint16_t reg, uint8_t *buf)
{
    p_i2c_obj->i2c_ops.xfer_start(&p_i2c_obj->i2c_cfg);
    p_i2c_obj->i2c_ops.send_byte(&p_i2c_obj->i2c_cfg, 0xA0);
    p_i2c_obj->i2c_ops.wait_ack(&p_i2c_obj->i2c_cfg, &p_i2c_obj->i2c_ops);

    p_i2c_obj->i2c_ops.send_byte(&p_i2c_obj->i2c_cfg, (reg & 0xff));
    p_i2c_obj->i2c_ops.wait_ack(&p_i2c_obj->i2c_cfg, &p_i2c_obj->i2c_ops);

    p_i2c_obj->i2c_ops.send_byte(&p_i2c_obj->i2c_cfg,*buf);
    p_i2c_obj->i2c_ops.wait_ack(&p_i2c_obj->i2c_cfg, &p_i2c_obj->i2c_ops);

    p_i2c_obj->i2c_ops.xfer_stop(&p_i2c_obj->i2c_cfg);
    p_i2c_obj->i2c_ops.delay_ms(10);
}

uint8_t eeprom_read_byte(i2c_object_t *p_i2c_obj, uint16_t reg, uint8_t *buf)
{
    p_i2c_obj->i2c_ops.xfer_start(&p_i2c_obj->i2c_cfg);
    p_i2c_obj->i2c_ops.send_byte(&p_i2c_obj->i2c_cfg, 0xA0);
    p_i2c_obj->i2c_ops.wait_ack(&p_i2c_obj->i2c_cfg, &p_i2c_obj->i2c_ops);

    p_i2c_obj->i2c_ops.send_byte(&p_i2c_obj->i2c_cfg, (reg & 0xff));
    p_i2c_obj->i2c_ops.wait_ack(&p_i2c_obj->i2c_cfg, &p_i2c_obj->i2c_ops);

    p_i2c_obj->i2c_ops.xfer_start(&p_i2c_obj->i2c_cfg);     
    p_i2c_obj->i2c_ops.send_byte(&p_i2c_obj->i2c_cfg, 0xA1);
    p_i2c_obj->i2c_ops.wait_ack(&p_i2c_obj->i2c_cfg, &p_i2c_obj->i2c_ops);

    *buf = p_i2c_obj->i2c_ops.read_byte(&p_i2c_obj->i2c_cfg, &p_i2c_obj->i2c_ops, 0);
    p_i2c_obj->i2c_ops.xfer_stop(&p_i2c_obj->i2c_cfg);
}

void eeprom_read(i2c_object_t *p_i2c_obj, uint16_t reg, uint8_t *buf, uint8_t len)
{
    uint8_t i = 0;

    for (i = 0; i < len; i++)
    {
        eeprom_read_byte(p_i2c_obj, (reg+i), &buf[i]);
    }
}

void eeprom_write(i2c_object_t *p_i2c_obj, uint16_t reg, uint8_t *buf, uint8_t len)
{
    uint8_t i = 0;

    for (i = 0; i < len; i++)
    {
        eeprom_write_byte(p_i2c_obj, (reg+i), &buf[i]);
    }
}
