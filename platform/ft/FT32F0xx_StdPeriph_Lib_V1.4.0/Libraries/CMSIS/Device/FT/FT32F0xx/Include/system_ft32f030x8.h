/**
  ******************************************************************************
  * @file    system_ft32f030x8.h
  * @author  AE
  * @version V1.0.0
  * @date    2020-06-22
  * @brief   CMSIS Cortex-M0 Device Peripheral Access Layer System Header File.
  */

/** @addtogroup CMSIS
  * @{
  */

/** @addtogroup ft32F030x8_system
  * @{
  */  
  
/**
  * @brief Define to prevent recursive inclusion
  */
#ifndef __SYSTEM_FT32F030X8_H
#define __SYSTEM_FT32F030X8_H

#ifdef __cplusplus
 extern "C" {
#endif 

/** @addtogroup FT32F030X8_System_Includes
  * @{
  */

/**
  * @}
  */


/** @addtogroup FT32F030X8_System_Exported_types
  * @{
  */

extern uint32_t SystemCoreClock;          /*!< System Clock Frequency (Core Clock) */

/**
  * @}
  */

/** @addtogroup FT32F030X8_System_Exported_Constants
  * @{
  */

/**
  * @}
  */

/** @addtogroup FT32F030X8_System_Exported_Macros
  * @{
  */

/**
  * @}
  */

/** @addtogroup FT32F030X8_System_Exported_Functions
  * @{
  */
  
extern void SystemInit(void);
extern void SystemCoreClockUpdate(void);
/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /*__SYSTEM_FT32F030X8_H */

/**
  * @}
  */
  
/**
  * @}
  */  
/************************ (C) COPYRIGHT FMD *****END OF FILE****/
