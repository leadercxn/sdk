/**
  ******************************************************************************
  * @file    FT32f0xx_comp.c
  * @author  AE
  * @version V1.0.0
  * @date    2020-06-22
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the comparators (COMP1 and COMP2) peripheral
  *          applicable only on FT32F030 devices: 
  *           + Comparators configuration
  *           + Window mode control
  *
  *  @verbatim
  *
 ===============================================================================
                     ##### How to use this driver #####
 ===============================================================================
    [..]           
   
         The device integrates two analog comparators COMP1 and COMP2:
         (+) The non inverting input is set to PA1 for COMP1 and to PA3
             for COMP2.
  
         (+) The inverting input can be selected among: DAC1_OUT, DAC2_OUT 
             1/4 VREFINT, 1/2 VERFINT, 3/4 VREFINT, VREFINT,
             I/O (PA0 for COMP1 and PA2 for COMP2)
  
         (+) The COMP output is internally is available using COMP_GetOutputLevel()
             and can be set on GPIO pins: PA0, PA6, PA11 for COMP1
             and PA2, PA7, PA12 for COMP2
  
         (+) The COMP output can be redirected to embedded timers (TIM1, TIM2
             and TIM3)
  
         (+) The two comparators COMP1 and COMP2 can be combined in window
             mode and only COMP1 non inverting (PA1) can be used as non-
             inverting input.
  
         (+) The two comparators COMP1 and COMP2 have interrupt capability 
             with wake-up from Sleep and Stop modes (through the EXTI controller).
             COMP1 and COMP2 outputs are internally connected to EXTI Line 21
             and EXTI Line 22 respectively.
                   

                     ##### How to configure the comparator #####
 ===============================================================================
    [..] 
           This driver provides functions to configure and program the Comparators 
           of all FT32F030X8 devices.
             
    [..]   To use the comparator, perform the following steps:
  
         (#) Enable the SYSCFG APB clock to get write access to comparator
             register using RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  
         (#) Configure the comparator input in analog mode using GPIO_Init()
  
         (#) Configure the comparator output in alternate function mode
             using GPIO_Init() and use GPIO_PinAFConfig() function to map the
             comparator output to the GPIO pin
  
         (#) Configure the comparator using COMP_Init() function:
                 (++)  Select the inverting input
                 (++)  Select the output polarity  
                 (++)  Select the output redirection
                 (++)  Select the hysteresis level
                 (++)  Select the power mode
    
         (#) Enable the comparator using COMP_Cmd() function
  
         (#) If required enable the COMP interrupt by configuring and enabling
             EXTI line in Interrupt mode and selecting the desired sensitivity
             level using EXTI_Init() function. After that enable the comparator
             interrupt vector using NVIC_Init() function.
  
     @endverbatim
  */

/* Includes ------------------------------------------------------------------*/
#include "FT32f0xx_comp.h"

/** @addtogroup FT32f0xx_StdPeriph_Driver
  * @{
  */

/** @defgroup COMP 
  * @brief COMP driver modules
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* CSR register Mask */
#define COMP_CSR_CLEAR_MASK              ((uint32_t)0x00003FFE)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup COMP_Private_Functions
  * @{
  */

/** @defgroup COMP_Group1 Initialization and Configuration functions
 *  @brief   Initialization and Configuration functions 
 *
@verbatim   
 ===============================================================================
               ##### Initialization and Configuration functions #####
 ===============================================================================  

@endverbatim
  * @{
  */
   
/**
  * @brief  Deinitializes COMP peripheral registers to their default reset values.
  * @note   Deinitialization can't be performed if the COMP configuration is locked.
  *         To unlock the configuration, perform a system reset.
  * @param  None
  * @retval None
  */
void COMP_DeInit(void)
{
  COMP->CSR = ((uint32_t)0x00000000);    /*!< Set COMP_CSR register to reset value */
}

/**
  * @brief  Initializes the COMP peripheral according to the specified parameters
  *         in COMP_InitStruct
  * @note   If the selected comparator is locked, initialization can't be performed.
  *         To unlock the configuration, perform a system reset.
  * @note   By default, PA1 is selected as COMP1 non inverting input.
  *         
  * @param  COMP_Selection: the selected comparator. 
  *          This parameter can be one of the following values:
  *            @arg COMP_Selection_COMP1: COMP1 selected
  *            @arg COMP_Selection_COMP2: COMP2 selected
  * @param  COMP_InitStruct: pointer to an COMP_InitTypeDef structure that contains 
  *         the configuration information for the specified COMP peripheral.
  * @retval None
  */
void COMP_Init(uint32_t COMP_Selection, COMP_InitTypeDef* COMP_InitStruct)
{
  uint32_t tmpreg = 0;

  /* Check the parameters */
  assert_param(IS_COMP_ALL_PERIPH(COMP_Selection));
  assert_param(IS_COMP_VIP_SEL(COMP_InitStruct->COMP_VipSel));
  assert_param(IS_COMP_VINSEL(COMP_InitStruct->COMP_VinSel));
  assert_param(IS_COMP_OUTPUT_SEL(COMP_InitStruct->COMP_OutputSel));
  assert_param(IS_COMP_POL(COMP_InitStruct->COMP_Pol));

  /*!< Get the COMP_CSR register value */
  tmpreg = COMP->CSR;

  /*!< Clear the bits */ 
  tmpreg &= (uint32_t) ~(COMP_CSR_CLEAR_MASK<<COMP_Selection);

  /*!< Configure COMP: COMP_VipSel, COMP_VinSel, COMP_OutputSel value and COMP_Pol */
  tmpreg |= (uint32_t)((COMP_InitStruct->COMP_VipSel | COMP_InitStruct->COMP_VinSel|
                       COMP_InitStruct->COMP_OutputSel | COMP_InitStruct->COMP_Pol));

  /*!< Write to COMP_CSR register */
  COMP->CSR = tmpreg;  
}

/**
  * @brief  Fills each COMP_InitStruct member with its default value.
  * @param  COMP_InitStruct: pointer to an COMP_InitTypeDef structure which will 
  *         be initialized.
  * @retval None
  */
void COMP_StructInit(COMP_InitTypeDef* COMP_InitStruct)
{
  COMP_InitStruct->COMP_VipSel = NCOMP_VIP_SEL_PAD_PA1;
  COMP_InitStruct->COMP_VinSel = NCOMP_VIN_SEL_PAD_PA0 | PCOMP_VIN_SEL_PAD_PA2;
  COMP_InitStruct->COMP_OutputSel = 0;
  COMP_InitStruct->COMP_Pol = 0;
}

/**
  * @brief  Enable or disable the COMP peripheral.
  * @note   If the selected comparator is locked, enable/disable can't be performed.
  *         To unlock the configuration, perform a system reset.
  * @param  COMP_Selection: the selected comparator.
  *          This parameter can be one of the following values:
  *            @arg NCOMP_Selection_COMP: COMP1 selected
  *            @arg PCOMP_Selection_COMP: COMP2 selected
  * @param  NewState: new state of the COMP peripheral.
  *          This parameter can be: ENABLE or DISABLE.
  * @note   When enabled, the comparator compares the non inverting input with 
  *         the inverting input and the comparison result is available on comparator output.
  * @note   When disabled, the comparator doesn't perform comparison and the 
  *         output level is low.
  * @retval None
  */
void COMP_Cmd(uint32_t COMP_Selection, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_COMP_ALL_PERIPH(COMP_Selection));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Enable the selected COMP peripheral */
    COMP->CSR |= (uint32_t) (1<<COMP_Selection);
  }
  else
  {
    /* Disable the selected COMP peripheral  */
    COMP->CSR &= (uint32_t)(~((uint32_t)1<<COMP_Selection));
  }
}

/**
  * @brief  Return the output level (high or low) of the selected comparator. 
  * @note   The output level depends on the selected polarity.
  * @note   If the polarity is not inverted:
  *          - Comparator output is low when the non-inverting input is at a lower
  *            voltage than the inverting input
  *          - Comparator output is high when the non-inverting input is at a higher
  *            voltage than the inverting input
  * @note   If the polarity is inverted:
  *          - Comparator output is high when the non-inverting input is at a lower
  *            voltage than the inverting input
  *          - Comparator output is low when the non-inverting input is at a higher
  *            voltage than the inverting input
  * @param  COMP_Selection: the selected comparator. 
  *          This parameter can be one of the following values:
  *            @arg COMP_Selection_COMP1: COMP1 selected
  *            @arg COMP_Selection_COMP2: COMP2 selected  
  * @retval Returns the selected comparator output level: low or high.
  *       
  */
uint32_t COMP_GetOutputLevel(uint32_t COMP_Selection)
{
  uint32_t compout = 0x0;

  /* Check the parameters */
  assert_param(IS_COMP_ALL_PERIPH(COMP_Selection));

  /* Check if selected comparator output is high */
   if ((COMP->CSR & (COMP_CSR_COMP1OUT<<COMP_Selection)) != 0)
  {
    compout = COMP_OutputLevel_High;
  }
  else
  {
    compout = COMP_OutputLevel_Low;
  }

  /* Return the comparator output level */
  return (uint32_t)(compout);
}

/**
  * @}
  */

/** @defgroup COMP_Group2 Window mode control function
 *  @brief   Window mode control function 
 *
@verbatim   
 ===============================================================================
                     ##### Window mode control function #####
 ===============================================================================  

@endverbatim
  * @{
  */

/**
  * @brief  Enables or disables the window mode.
  * @note   In window mode, COMP1 and COMP2 non inverting inputs are connected
  *         together and only COMP1 non inverting input (PA1) can be used.
  * @param  NewState: new state of the window mode.
  *          This parameter can be :
  *           @arg ENABLE: COMP1 and COMP2 non inverting inputs are connected together.
  *           @arg DISABLE: OMP1 and COMP2 non inverting inputs are disconnected.
  * @retval None
  */
void COMP_WindowCmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  if (NewState != DISABLE)
  {
    /* Enable the window mode */
    COMP->CSR |= (uint32_t) COMP_CSR_WNDWEN;
  }
  else
  {
    /* Disable the window mode */
    COMP->CSR &= (uint32_t)(~COMP_CSR_WNDWEN);
  }
}

/**
  * @}
  */

/** @defgroup COMP_Group3 COMP configuration locking function
 *  @brief   COMP1 and COMP2 configuration locking function
 *           COMP1 and COMP2 configuration can be locked each separately.
 *           Unlocking is performed by system reset.
 *
@verbatim   
 ===============================================================================
                     ##### Configuration Lock function #####
 ===============================================================================  

@endverbatim
  * @{
  */

/**
  * @brief  Lock the selected comparator (COMP1/COMP2) configuration.
  * @note   Locking the configuration means that all control bits are read-only.
  *         To unlock the comparator configuration, perform a system reset.
  * @param  COMP_Selection: selects the comparator to be locked 
  *          This parameter can be a value of the following values:
  *            @arg NCOMP_Selection_COMP: COMP1 configuration is locked.
  *            @arg PCOMP_Selection_COMP: COMP2 configuration is locked.  
  * @retval None
  */
void COMP_LockConfig(uint32_t COMP_Selection)
{
  /* Check the parameter */
  assert_param(IS_COMP_ALL_PERIPH(COMP_Selection));

  /* Set the lock bit corresponding to selected comparator */
  COMP->CSR |= (uint32_t) (COMP_CSR_NCOMPLOCK<<COMP_Selection);
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT FMD *****END OF FILE****/
