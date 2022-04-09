/**
  ******************************************************************************
  * @file    ft32f0xx.h
  * @author  AE
  * @brief   CMSIS FT32F0xx Device Peripheral Access Layer Header File.           
  *            
  *          The file is the unique include file that the application programmer
  *          is using in the C source code, usually in main.c. This file contains:
  *           - Configuration section that allows to select:
  *              - The FT32F0xx device used in the target application
  *              - To use or not the peripheral抯 drivers in application code(i.e. 
  *                code will be based on direct access to peripheral抯 registers 
  *                rather than drivers API), this option is controlled by 
  *                "#define USE_HAL_DRIVER"
  *  
  ******************************************************************************
  */

/** @addtogroup CMSIS
  * @{
  */

/** @addtogroup ft32f0xx
  * @{
  */
    
#ifndef __FT32F0xx_H
#define __FT32F0xx_H

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */
   
/** @addtogroup Library_configuration_section
  * @{
  */
  
/**
  * @brief FT32 Family
  */
#if !defined  (FT32F0)
#define FT32F0
#endif /* FT32F0 */

/* Uncomment the line below according to the target FT32 device used in your
   application 
  */

#if !defined (FT32F030x6) && !defined (FT32F030x8) &&                           \
    !defined (FT32F031x6) && !defined (FT32F038xx) &&                           \
    !defined (FT32F042x6) && !defined (FT32F048xx) && !defined (FT32F070x6) && \
    !defined (FT32F051x8) && !defined (FT32F058xx) &&                           \
    !defined (FT32F071xB) && !defined (FT32F072xB) && !defined (FT32F078xx) && !defined (FT32F070xB) && \
    !defined (FT32F091xC) && !defined (FT32F098xx) && !defined (FT32F030xC)
  /* #define FT32F030x6 */  /*!< FT32F030x4, FT32F030x6 Devices (FT32F030xx microcontrollers where the Flash memory ranges between 16 and 32 Kbytes)              */
  /* #define FT32F030x8 */  /*!< FT32F030x8 Devices (FT32F030xx microcontrollers where the Flash memory is 64 Kbytes)                                              */
  /* #define FT32F031x6 */  /*!< FT32F031x4, FT32F031x6 Devices (FT32F031xx microcontrollers where the Flash memory ranges between 16 and 32 Kbytes)              */
  /* #define FT32F038xx */  /*!< FT32F038xx Devices (FT32F038xx microcontrollers where the Flash memory is 32 Kbytes)                                              */
  /* #define FT32F042x6 */  /*!< FT32F042x4, FT32F042x6 Devices (FT32F042xx microcontrollers where the Flash memory ranges between 16 and 32 Kbytes)              */
  /* #define FT32F048x6 */  /*!< FT32F048xx Devices (FT32F042xx microcontrollers where the Flash memory is 32 Kbytes)                                              */
  /* #define FT32F051x8 */  /*!< FT32F051x4, FT32F051x6, FT32F051x8 Devices (FT32F051xx microcontrollers where the Flash memory ranges between 16 and 64 Kbytes) */
  /* #define FT32F058xx */  /*!< FT32F058xx Devices (FT32F058xx microcontrollers where the Flash memory is 64 Kbytes)                                              */
  /* #define FT32F070x6 */  /*!< FT32F070x6 Devices (FT32F070x6 microcontrollers where the Flash memory ranges between 16 and 32 Kbytes)                           */
  /* #define FT32F070xB */  /*!< FT32F070xB Devices (FT32F070xB microcontrollers where the Flash memory ranges between 64 and 128 Kbytes)                          */
  /* #define FT32F071xB */  /*!< FT32F071x8, FT32F071xB Devices (FT32F071xx microcontrollers where the Flash memory ranges between 64 and 128 Kbytes)             */
  /* #define FT32F072xB */  /*!< FT32F072x8, FT32F072xB Devices (FT32F072xx microcontrollers where the Flash memory ranges between 64 and 128 Kbytes)             */
  /* #define FT32F078xx */  /*!< FT32F078xx Devices (FT32F078xx microcontrollers where the Flash memory is 128 Kbytes)                                             */
  /* #define FT32F030xC */  /*!< FT32F030xC Devices (FT32F030xC microcontrollers where the Flash memory is 256 Kbytes)                                             */  
  /* #define FT32F091xC */  /*!< FT32F091xB, FT32F091xC Devices (FT32F091xx microcontrollers where the Flash memory ranges between 128 and 256 Kbytes)            */
  /* #define FT32F098xx */  /*!< FT32F098xx Devices (FT32F098xx microcontrollers where the Flash memory is 256 Kbytes)                                             */
#endif
   
/*  Tip: To avoid modifying this file each time you need to switch between these
        devices, you can define the device in your toolchain compiler preprocessor.
  */
#if !defined  (USE_HAL_DRIVER)
/**
 * @brief Comment the line below if you will not use the peripherals drivers.
   In this case, these drivers will not be included and the application code will 
   be based on direct access to peripherals registers 
   */
  /*#define USE_HAL_DRIVER */
#endif /* USE_HAL_DRIVER */

/**
  * @brief CMSIS Device version number V2.3.3
  */
#define __FT32F0_DEVICE_VERSION_MAIN   (0x02) /*!< [31:24] main version */
#define __FT32F0_DEVICE_VERSION_SUB1   (0x03) /*!< [23:16] sub1 version */
#define __FT32F0_DEVICE_VERSION_SUB2   (0x03) /*!< [15:8]  sub2 version */
#define __FT32F0_DEVICE_VERSION_RC     (0x00) /*!< [7:0]  release candidate */ 
#define __FT32F0_DEVICE_VERSION        ((__FT32F0_DEVICE_VERSION_MAIN << 24)\
                                        |(__FT32F0_DEVICE_VERSION_SUB1 << 16)\
                                        |(__FT32F0_DEVICE_VERSION_SUB2 << 8 )\
                                        |(__FT32F0_DEVICE_VERSION_RC))
                                             
/**
  * @}
  */

/** @addtogroup Device_Included
  * @{
  */

#if defined(FT32F030x6)
  #include "ft32f030x6.h"
#elif defined(FT32F030x8)
  #include "ft32f030x8.h"
#elif defined(FT32F072x8)
  #include "ft32f072x8.h" 
#else
 #error "Please select first the target FT32F0xx device used in your application (in ft32f0xx.h file)"
#endif

/**
  * @}
  */



/**
  * @}
  */


/** @addtogroup Exported_macros
  * @{
  */
#define SET_BIT(REG, BIT)     ((REG) |= (BIT))

#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))

#define READ_BIT(REG, BIT)    ((REG) & (BIT))

#define CLEAR_REG(REG)        ((REG) = (0x0))

#define WRITE_REG(REG, VAL)   ((REG) = (VAL))

#define READ_REG(REG)         ((REG))

#define MODIFY_REG(REG, CLEARMASK, SETMASK)  WRITE_REG((REG), (((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))


/**
  * @}
  */

#if defined (USE_HAL_DRIVER)
 #include "ft32f0xx_hal.h"
#endif /* USE_HAL_DRIVER */


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __FT32F0xx_H */
/**
  * @}
  */

/**
  * @}
  */
  



/*****************************END OF FILE****/
