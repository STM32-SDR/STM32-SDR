/**
  ******************************************************************************
  * @file    JoyHal.h
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    11-July-2011
  * @brief   This file contains all the Joystick functions whose implementation
  *          depends on the Joystick Controller used in your Design.
  *          You only need to change these functions implementations in order to
  *          reuse this code with other Joystick Controller 
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
#ifndef __JOY_HAL_H
#define __JOY_HAL_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes --------------------------------------------------------------------*/
#include "stm32HAL.h"

/** @addtogroup Embedded_GUI_Library
  * @{
  */

/** @addtogroup JoyHal
  * @{
  */ 

/** @defgroup JoyHal_Exported_Types
  * @{
  */

/**
  * @}
  */

/** @defgroup JoyHal_Private_Defines
  * @{
  */
   
#define GL_JOY_NONE     0x00
#define GL_JOY_CENTER   0x01
#define GL_JOY_DOWN     0x02
#define GL_JOY_LEFT     0x03
#define GL_JOY_RIGHT    0x04
#define GL_JOY_UP       0x05

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

/*........ */
/* TouchScreen Registers */
#define GL_JOY_CTRL         IOE_REG_TSC_CTRL 
#define GL_JOY_CTRL_VALUE   0x93
   
/**
  * @brief  JOYSTICK Pins definition 
  */ 
#define GL_JOY_IO_SEL       JOY_IO_SEL
#define GL_JOY_IO_DOWN      JOY_IO_DOWN
#define GL_JOY_IO_LEFT      JOY_IO_LEFT
#define GL_JOY_IO_RIGHT     JOY_IO_RIGHT
#define GL_JOY_IO_UP        JOY_IO_UP
#define GL_JOY_IO_NONE      JOY_IO_NONE
#define GL_JOY_IO_PINS      JOY_IO_PINS
#define JOY_IO_PINS         (uint32_t)(IO_Pin_3 | IO_Pin_4 | IO_Pin_5 | IO_Pin_6 | IO_Pin_7)
    
#define EDGE_NONE           GL_JOY_IO_PINS
#define EDGE_CENTER         pJoyHwParam.JOY_Select_Pin
#define EDGE_DOWN           pJoyHwParam.JOY_Down_Pin
#define EDGE_LEFT           pJoyHwParam.JOY_Left_Pin
#define EDGE_RIGHT          pJoyHwParam.JOY_Right_Pin
#define EDGE_UP             pJoyHwParam.JOY_Up_Pin

/** 
  * @brief  IO Pins  
  */ 
#define GL_IO_Pin_0         IO_Pin_0
#define GL_IO_Pin_1         IO_Pin_1
#define GL_IO_Pin_2         IO_Pin_2
#define GL_IO_Pin_3         IO_Pin_3
#define GL_IO_Pin_4         IO_Pin_4
#define GL_IO_Pin_5         IO_Pin_5
#define GL_IO_Pin_6         IO_Pin_6
#define GL_IO_Pin_7         IO_Pin_7
#define GL_IO_Pin_ALL       IO_Pin_ALL

#define GL_RCC_IRQChannel        RCC_IRQn

/**
  * @}
  */
   
/** @defgroup JoyHal_Private_TypesDefinitions
  * @{
  */
/** 
  * @brief  JOY_ReadMode enumeration definition  
  */
typedef enum
{ IOEXP_MODE = 0,
  POLLING_MODE = 1,
  INTERRUPT    = 2
}JOY_ReadMode;

typedef struct   
{ 
    uint8_t          JOY_PortSource;
    uint8_t          JOY_PinSource;
    GPIO_TypeDef *   JOY_Select_Port;
    GPIO_TypeDef *   JOY_Right_Port;
    GPIO_TypeDef *   JOY_Left_Port;
    GPIO_TypeDef *   JOY_Up_Port;
    GPIO_TypeDef *   JOY_Down_Port;
    uint16_t         JOY_Select_Pin;
    uint16_t         JOY_Right_Pin;
    uint16_t         JOY_Left_Pin;
    uint16_t         JOY_Up_Pin;
    uint16_t         JOY_Down_Pin;
    uint32_t         JOY_Rcc_BusPeriph_GpioSel;
    uint32_t         JOY_Rcc_BusPeriph_GpioRight;
    uint32_t         JOY_Rcc_BusPeriph_GpioLeft;
    uint32_t         JOY_Rcc_BusPeriph_GpioUp;
    uint32_t         JOY_Rcc_BusPeriph_GpioDown;
    uint8_t          JOY_Exti_IrqChannel;
    uint32_t         JOY_Exti_Line;
    uint16_t         JOY_DeviceRegister;
    uint16_t         JOY_IT_Gpio_Pin;
    GPIO_TypeDef *         JOY_IT_Gpio_Port;
}JOY_HW_Parameters_TypeDef;

typedef struct   
{ 
    uint8_t          BTN_PortSource;
    uint16_t         BTN_PinSource;
    uint8_t          BTN_Exti_IrqChannel;
    uint32_t         BTN_Exti_Line;    
    GPIO_TypeDef *   BTN_Port;
    uint16_t         BTN_Pin;
    BitAction        BTN_Pin_Active;    
    uint32_t         BTN_Rcc_BusPeriph_Gpio;    
}BTN_HW_Parameters_TypeDef;

/**
  * @}
  */ 

/** @defgroup JoyHal_Exported_variables
  * @{
  */
extern JOY_HW_Parameters_TypeDef pJoyHwParam;     /* Joystick Hardware Parameters Structure */
extern BTN_HW_Parameters_TypeDef pBtnHwParam;     /* User Button Hardware Parameters Structure */
/**
  * @}
  */
   
/** @defgroup JoyHal_Exported_Macros
  * @{
  */

/**
  * @}
  */

/** @defgroup JoyHal_Exported_Functions
  * @{
  */
JOY_HW_Parameters_TypeDef* NewJoyHwParamObj (void);
BTN_HW_Parameters_TypeDef* NewBtnHwParamObj (void);
  
void GL_JoyStickConfig_IOExpander(void);
void GL_JoyStickConfig_GPIO(void);
uint32_t GL_JoyStickState(JOY_ReadMode mode);
uint32_t GL_JoyStickStateIOEXP(void);
uint32_t GL_JoyStickStatePolling(void);
uint32_t GL_JOY_Interface_Init(void);
uint8_t GL_GPIO_ReadInputDataBit(GPIO_TypeDef * GPIOx, uint16_t GPIO_Pin);
void GL_ButtonInit(void);

#ifdef __cplusplus
}
#endif

#endif /*__JOY_HAL_H */

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
