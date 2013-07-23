/**
  ******************************************************************************
  * @file    TscHal.h
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    11-July-2011
  * @brief   This file contains all the TouchScreen functions whose
  *          implementation depends on the TSC Controller used in your Design.
  *          You only need to change these functions implementations
  *          in order to reuse this code with other TSC Controller 
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Define to prevent recursive inclusion ---------------------------------------*/
#ifndef __TSC_HAL_H
#define __TSC_HAL_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes --------------------------------------------------------------------*/
#include "stm32HAL.h"
#include "graphicObjectTypes.h"

/** @addtogroup Embedded_GUI_Library
  * @{
  */

/** @addtogroup TscHal
  * @{
  */ 

/** @defgroup TscHal_Private_Defines
  * @{
  */
#if (defined(USE_STM3210C_EVAL) || defined(USE_STM32100E_EVAL) || defined(USE_STM322xG_EVAL))
#define TOUCH_SCREEN_CAPABILITY			(1)
#endif
   
/* General Control Registers */
#define GL_SYS_CTRL1        IOE_REG_SYS_CTRL1
#define GL_SYS_CTRL2        IOE_REG_SYS_CTRL2 

/* Interrupt Control register */
#define GL_SPI_CFG          IOE_REG_SPI_CFG
#define GL_INT_CTRL         IOE_REG_INT_CTRL
#define GL_INT_EN           IOE_REG_INT_EN
#define GL_INT_STA          IOE_REG_INT_STA
#define GL_GPIO_INT_EN      IOE_REG_GPIO_INT_EN
#define GL_GPIO_INT_STA     IOE_REG_GPIO_INT_STA
/* GPIO Registers */
#define GL_GPIO_SET_PIN     IOE_REG_GPIO_SET_PIN 
#define GL_GPIO_CLR_PIN     IOE_REG_GPIO_CLR_PIN 
#define GL_GPIO_MP_STA      IOE_REG_GPIO_MP_STA
#define GL_GPIO_DIR         IOE_REG_GPIO_DIR
#define GL_GPIO_ED          IOE_REG_GPIO_ED 
#define GL_GPIO_RE          IOE_REG_GPIO_RE
#define GL_GPIO_FE          IOE_REG_GPIO_FE 
#define GL_GPIO_AF          IOE_REG_GPIO_AF

/* ADC Registers */
#define GL_ADC_CTRL         IOE_REG_ADC_CTRL1
/*........ */
/* TouchScreen Registers */
#define GL_TSC_CTRL         IOE_REG_TSC_CTRL
#define GL_TSC_CFG          IOE_REG_TSC_CFG  
#define GL_WDM_TR_X         IOE_REG_WDM_TR_X 
#define GL_WDM_TR_Y         IOE_REG_WDM_TR_Y   
#define GL_WDM_BL_X         IOE_REG_WDM_BL_X   
#define GL_WDM_BL_Y         IOE_REG_WDM_BL_Y 
#define GL_FIFO_TH          IOE_REG_FIFO_TH    
#define GL_FIFO_CTRL_STA    IOE_REG_FIFO_STA 
#define GL_FIFO_SIZE        IOE_REG_FIFO_SIZE   
#define GL_TSC_DATA_X       IOE_REG_TSC_DATA_X  
#define GL_TSC_DATA_Y       IOE_REG_TSC_DATA_Y  
#define GL_TSC_DATA_Z       IOE_REG_TSC_DATA_Z    
#define GL_TSC_DATA_XYZ     IOE_REG_TSC_DATA_XYZ
#define GL_TSC_FRACT_XYZ    IOE_REG_TSC_FRACT_XYZ
#define GL_TSC_DATA         IOE_REG_TSC_DATA
#define GL_TSC_STA          IOE_REG_INT_STA 
#define GL_TSC_I_DRIVE      IOE_REG_TSC_I_DRIVE
#define GL_TSC_SHIELD       IOE_REG_TSC_SHIELD

#define GL_TSC_CTRL_VALUE   0x93
#define GL_TSC_CFG_VALUE    0x18
#define GL_INT_EN_VALUE     0x01
#define GL_INT_CTRL_VALUE   0x01
#define GL_FIFO_TH_VALUE    0x01

/* EXTI Lines ----------------------------------------------------------------*/
#define GL_EXTI_Line0       EXTI_Line0
#define GL_EXTI_Line1       EXTI_Line1
#define GL_EXTI_Line2       EXTI_Line2
#define GL_EXTI_Line3       EXTI_Line3
#define GL_EXTI_Line4       EXTI_Line4
#define GL_EXTI_Line5       EXTI_Line5
#define GL_EXTI_Line6       EXTI_Line6
#define GL_EXTI_Line7       EXTI_Line7
#define GL_EXTI_Line8       EXTI_Line8
#define GL_EXTI_Line9       EXTI_Line9
#define GL_EXTI_Line10      EXTI_Line10
#define GL_EXTI_Line11      EXTI_Line11
#define GL_EXTI_Line12      EXTI_Line12
#define GL_EXTI_Line13      EXTI_Line13
#define GL_EXTI_Line14      EXTI_Line14
#define GL_EXTI_Line15      EXTI_Line15
#define GL_EXTI_Line16      EXTI_Line16
#define GL_EXTI_Line17      EXTI_Line17
#define GL_EXTI_Line18      EXTI_Line18
#define GL_EXTI_Line19      EXTI_Line19

/* Preemption Priority Group -------------------------------------------------*/
#define GL_NVIC_VectTab_FLASH    NVIC_VectTab_FLASH

#define GL_NVIC_PriorityGroup_0  NVIC_PriorityGroup_0
#define GL_NVIC_PriorityGroup_1  NVIC_PriorityGroup_1
#define GL_NVIC_PriorityGroup_2  NVIC_PriorityGroup_2
#define GL_NVIC_PriorityGroup_3  NVIC_PriorityGroup_3
#define GL_NVIC_PriorityGroup_4  NVIC_PriorityGroup_4

#define GL_RCC_IRQChannel        RCC_IRQn

/* For Flashing */
#define FLASH_PAGE_SIZE          ((uint16_t)0x800)
#define TSC_FLASH_COMPLETE       FLASH_COMPLETE

#if TOUCH_SCREEN_CAPABILITY 
  #define TSC_FLASH_FLAG_BSY       ((uint32_t)FLASH_FLAG_BSY)  /*!< FLASH Busy flag */
  #define TSC_FLASH_FLAG_EOP       ((uint32_t)FLASH_FLAG_EOP)  /*!< FLASH End of Operation flag */
#ifndef STM32F2XX   
  #define TSC_FLASH_FLAG_PGERR     ((uint32_t)FLASH_FLAG_PGERR)  /*!< FLASH Program error flag */
  #define TSC_FLASH_FLAG_WRPRTERR  ((uint32_t)FLASH_FLAG_WRPRTERR)  /*!< FLASH Write protected error flag */
#else
  #define TSC_FLASH_FLAG_PGERR     ((uint32_t)FLASH_FLAG_PGAERR|FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR)  /*!< FLASH Program error flag */
  #define TSC_FLASH_FLAG_WRPRTERR  ((uint32_t)FLASH_FLAG_WRPERR)  /*!< FLASH Write protected error flag */
#endif   
#endif
   
/**
  * @}
  */

/** @defgroup TscHal_Exported_Types
  * @{
  */
   
/* Redefine micro specific types */
typedef FLASH_Status      TSC_FLASH_Status;

/** 
  * @brief  TSC_Flash_TestStatus (Flash Programming) enumeration definition  
  */
typedef enum {FAILED = 0, PASSED = !FAILED} TSC_Flash_TestStatus;

/* The TouchScreen I2C Peripheral settings */
typedef struct {
    I2C_TypeDef *  TSC_I2C_Ptr;
    uint32_t       TSC_I2C_Clock;
    uint16_t       TSC_I2C_Address;
}TSC_I2C_SettingsType;

/** 
  * @brief  TSC_HW_Parameters type definition  
  */
typedef struct   
{ 
    uint8_t          TSC_PortSource;
    uint16_t         TSC_PinSource;
    uint32_t         TSC_I2C_Clk;
    uint16_t         TSC_I2C_Sda_Gpio_Pin;
    GPIO_TypeDef *   TSC_I2C_Sda_Gpio_Port;
    uint32_t         TSC_I2C_Sda_Gpio_Clk;
    uint16_t         TSC_I2C_Sda_PinSource;
    uint32_t         TSC_I2C_Sda_AltFunc;
    uint16_t         TSC_I2C_Scl_Gpio_Pin;
    GPIO_TypeDef *   TSC_I2C_Scl_Gpio_Port;
    uint32_t         TSC_I2C_Scl_Gpio_Clk;
    uint16_t         TSC_I2C_Scl_PinSource;
    uint32_t         TSC_I2C_Scl_AltFunc;
    uint8_t          TSC_Exti_IrqChannel;
    uint32_t         TSC_Exti_Line;
    uint16_t         TSC_IT_Exti_Pin_Source;
    uint32_t         TSC_IT_Gpio_Clk;
    GPIO_TypeDef *   TSC_IT_Gpio_Port;
    uint16_t         TSC_IT_Gpio_Pin;
    uint32_t         TSC_DeviceRegister;
    uint32_t         TSC_RegisterAddress;
    I2C_TypeDef *    TSC_Bus_Port;
}TSC_HW_Parameters_TypeDef;

/**
  * @}
  */

/** @defgroup TscHal_Exported_variables
  * @{
  */
/* Touchscreen Hardware Parameters Structure */  
extern TSC_HW_Parameters_TypeDef pTscHwParam;

/**
  * @}
  */
   
/** @defgroup TscHal_Exported_Macros
  * @{
  */

/**
  * @}
  */

/** @defgroup TscHal_Exported_Functions
  * @{
  */
void GL_GPIO_Init(GPIO_TypeDef * GPIOx, GPIO_InitTypeDef* GPIO_InitStruct);
void GL_SPI_Init(SPI_TypeDef* SPIx, SPI_InitTypeDef* SPI_InitStruct);
void GL_NVIC_SetVectorTable(uint32_t NVIC_VectTab, uint32_t Offset);
void GL_NVIC_Init(NVIC_InitTypeDef* NVIC_InitStruct);
void GL_NVIC_PriorityGroupConfig(uint32_t NVIC_PriorityGroup);
void GL_NVIC_SystemHandlerPriorityConfig(uint32_t SystemHandler, uint8_t SystemHandlerPreemptionPriority, uint8_t SystemHandlerSubPriority);
void GL_EXTI_DeInit(void);
void GL_EXTI_Init(EXTI_InitTypeDef* EXTI_InitStruct);
void GL_GPIO_EXTILineConfig(uint8_t GPIO_PortSource, uint8_t GPIO_PinSource);
void GL_NVIC_DeInit(void);
void GL_EXTI_TSC_IRQHandler(void);
TSC_FLASH_Status TSC_WriteDataToNVM(uint32_t FlashFree_Address, int32_t *Data, uint32_t Size);
void TSC_Init(void);
uint32_t GL_TSC_Interface_Init(void);
void TSC_NVIC_Configuration(void);
void TSC_InterruptConfig(void);
void GL_RCC_APBPeriphClockCmd(uint32_t RCC_APBPeriph, GL_FunctionalState NewState, uint8_t APB_Selector);
void GL_RCC_AHBPeriphClockCmd(uint32_t RCC_AHBPeriph, GL_FunctionalState NewState);
void TSC_FLASH_Unlock(void);
void TSC_FLASH_ClearFlag(uint32_t FLASH_FLAG);
TSC_FLASH_Status TSC_FLASH_ErasePage(uint32_t Page_Address);
TSC_FLASH_Status TSC_FLASH_ProgramWord(uint32_t Address, uint32_t Data);
void Set_LastFlashMemoryAddress( uint32_t address);
void TSC_Read(void);
TSC_HW_Parameters_TypeDef* NewTscHwParamObj (void);
uint8_t GL_GPIO_ReadInputDataBit(GPIO_TypeDef * GPIOx, uint16_t GPIO_Pin);

#ifdef __cplusplus
}
#endif

#endif /*__TSC_HAL_H */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
