/**
  ******************************************************************************
  * @file    hw_config.h
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    11-July-2011
  * @brief   Target config file module.
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
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HARDWARE_CONFIG_H
#define __HARDWARE_CONFIG_H

/** @addtogroup Embedded_GUI_Example
  * @{
  */

/** @addtogroup HWConfig
  * @{
  */

#ifdef __cplusplus
 extern "C" {
#endif 
   

/* Includes ------------------------------------------------------------------*/
#if 0
#if defined(STM32F2XX)
#include "stm32f2xx.h"
#elif defined(STM32L1XX_MD)
#include "stm32l1xx.h"   
#else/* Suppose F1 Series */
#include "stm32f10x.h"
#endif
#endif
#include "stm32f4xx.h"
   
#if USE_STM322xG_EVAL
   #include "stm32_eval.h"
   #include "stm322xg_eval_ioe.h"
#endif  
#include <stdio.h>
#include "TscHal.h"
#include "JoyHal.h"
#include "LcdHal.h"
   
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* IOE I2C device address */
#define TSC_I2C_DEVICE_REGISTER     0x82
#define JOY_I2C_DEVICE_REGISTER     0x88


/** 
  * @brief   Touchscreen Controller DEFINES  
  */
#if 0
/* Touchscreen Controller DEFINES for STM3210C-EVAL */
#ifdef USE_STM3210C_EVAL
  #define TSC_I2C_PORT              I2C1
  #define TSC_I2C_CLK               RCC_APB1Periph_I2C1
  #define TSC_I2C_SDA_GPIO_PIN      GPIO_Pin_6
  #define TSC_I2C_SDA_GPIO_PORT     GPIOB
  #define TSC_I2C_SDA_GPIO_CLK      RCC_APB2Periph_GPIOB
  #define TSC_I2C_SDA_SOURCE        GPIO_PinSource6
  #define TSC_I2C_SDA_AF            GPIO_Remap_I2C1
  #define TSC_I2C_SCL_GPIO_PIN      GPIO_Pin_7
  #define TSC_I2C_SCL_GPIO_PORT     GPIOB
  #define TSC_I2C_SCL_GPIO_CLK      RCC_APB2Periph_GPIOB
  #define TSC_I2C_SCL_SOURCE        GPIO_PinSource7
  #define TSC_I2C_SCL_AF            GPIO_Remap_I2C1
  #define TSC_GPIO_PIN_SOURCE       GPIO_PinSource14
  #define TSC_IT_EXTI_PIN_SOURCE    GPIO_PinSource14
  #define TSC_GPIO_PORT_SOURCE      GPIO_PortSourceGPIOB
  #define TSC_EXTI_IRQ_CHANNEL      EXTI15_10_IRQn
  #define TSC_EXTI_LINE             EXTI_Line14
  #define TSC_IT_GPIO_CLK           RCC_APB2Periph_GPIOB
  #define TSC_IT_GPIO_PORT          GPIOB
  #define TSC_IT_GPIO_PIN           GPIO_Pin_14

/* Touchscreen Controller DEFINES for STM32100E-EVAL */   
#elif USE_STM32100E_EVAL
  #define TSC_I2C_PORT              I2C2 
  #define TSC_I2C_CLK               RCC_APB1Periph_I2C2
  #define TSC_I2C_SDA_GPIO_PIN      GPIO_Pin_11
  #define TSC_I2C_SDA_GPIO_PORT     GPIOB
  #define TSC_I2C_SDA_GPIO_CLK      RCC_APB2Periph_GPIOB
  #define TSC_I2C_SDA_SOURCE        GPIO_PinSource11
  #define TSC_I2C_SDA_AF            NULL
   
  #define TSC_I2C_SCL_GPIO_PIN      GPIO_Pin_10
  #define TSC_I2C_SCL_GPIO_PORT     GPIOB
  #define TSC_I2C_SCL_GPIO_CLK      RCC_APB2Periph_GPIOB
  #define TSC_I2C_SCL_SOURCE        GPIO_PinSource10
  #define TSC_I2C_SCL_AF            NULL
   
  #define TSC_GPIO_PIN_SOURCE       GPIO_PinSource12
  #define TSC_IT_EXTI_PIN_SOURCE    GPIO_PinSource12
  #define TSC_GPIO_PORT_SOURCE      GPIO_PortSourceGPIOA
  #define TSC_EXTI_IRQ_CHANNEL      EXTI15_10_IRQn
  #define JOY_EXTI_IRQ_CHANNEL      NULL
  #define TSC_EXTI_LINE             EXTI_Line12
  #define TSC_IT_GPIO_CLK           RCC_APB2Periph_GPIOA
  #define TSC_IT_GPIO_PORT          GPIOA
  #define TSC_IT_GPIO_PIN           GPIO_Pin_12
  #define JOY_EXTI_LINE             NULL
  
/* Touchscreen Controller DEFINES for STM322xG-EVAL */  
#elif defined(USE_STM322xG_EVAL)
  #define TSC_I2C_PORT              I2C1
  #define TSC_I2C_CLK               RCC_APB1Periph_I2C1
  #define TSC_I2C_SDA_GPIO_PIN      GPIO_Pin_6
  #define TSC_I2C_SDA_GPIO_PORT     GPIOB
  #define TSC_I2C_SDA_GPIO_CLK      RCC_AHB1Periph_GPIOB
  #define TSC_I2C_SDA_SOURCE        GPIO_PinSource6
  #define TSC_I2C_SDA_AF            GPIO_AF_I2C1
  #define TSC_I2C_SCL_GPIO_PIN      GPIO_Pin_9
  #define TSC_I2C_SCL_GPIO_PORT     GPIOB
  #define TSC_I2C_SCL_GPIO_CLK      RCC_AHB1Periph_GPIOB
  #define TSC_I2C_SCL_SOURCE        GPIO_PinSource9
  #define TSC_I2C_SCL_AF            GPIO_AF_I2C1
  #define TSC_GPIO_PIN_SOURCE       GPIO_PinSource2
  #define TSC_IT_EXTI_PIN_SOURCE    EXTI_PinSource2
  #define TSC_GPIO_PORT_SOURCE      EXTI_PortSourceGPIOI
  #define TSC_EXTI_IRQ_CHANNEL      EXTI2_IRQn
  #define TSC_EXTI_LINE             EXTI_Line2
  #define TSC_IT_GPIO_CLK           RCC_AHB1Periph_GPIOI
  #define TSC_IT_GPIO_PORT          GPIOI
  #define TSC_IT_GPIO_PIN           GPIO_Pin_2     
#endif
#endif

// Discovery Board:
#define TSC_I2C_PORT              I2C1
#define TSC_I2C_CLK               RCC_APB1Periph_I2C1
#define TSC_I2C_SDA_GPIO_PIN      GPIO_Pin_6
#define TSC_I2C_SDA_GPIO_PORT     GPIOB
#define TSC_I2C_SDA_GPIO_CLK      RCC_APB2Periph_GPIOB
#define TSC_I2C_SDA_SOURCE        GPIO_PinSource6
#define TSC_I2C_SDA_AF            GPIO_Remap_I2C1
#define TSC_I2C_SCL_GPIO_PIN      GPIO_Pin_7
#define TSC_I2C_SCL_GPIO_PORT     GPIOB
#define TSC_I2C_SCL_GPIO_CLK      RCC_APB2Periph_GPIOB
#define TSC_I2C_SCL_SOURCE        GPIO_PinSource7
#define TSC_I2C_SCL_AF            GPIO_Remap_I2C1
#define TSC_GPIO_PIN_SOURCE       GPIO_PinSource14
#define TSC_IT_EXTI_PIN_SOURCE    GPIO_PinSource14
#define TSC_GPIO_PORT_SOURCE      GPIO_PortSourceGPIOB
#define TSC_EXTI_IRQ_CHANNEL      EXTI15_10_IRQn
#define TSC_EXTI_LINE             EXTI_Line14
#define TSC_IT_GPIO_CLK           RCC_APB2Periph_GPIOB
#define TSC_IT_GPIO_PORT          GPIOB
#define TSC_IT_GPIO_PIN           GPIO_Pin_14


#if 0
/** 
  * @brief   Joystick Controller DEFINES  
  */ 
/* Joystick Params DEFINES for STM3210C-EVAL board */
#ifdef USE_STM3210C_EVAL
  #define JOY_GPIO_PIN_SOURCE       GPIO_PinSource14
  #define JOY_GPIO_SELECT_PORT      GPIOG
  #define JOY_GPIO_SELECT_PIN       GPIO_Pin_7
  #define JOY_GPIO_RIGHT_PORT       GPIOG
  #define JOY_GPIO_RIGHT_PIN        GPIO_Pin_4
  #define JOY_GPIO_LEFT_PORT        GPIOG
  #define JOY_GPIO_LEFT_PIN         GPIO_Pin_5
  #define JOY_GPIO_UP_PORT          GPIOG
  #define JOY_GPIO_UP_PIN           GPIO_Pin_3
  #define JOY_GPIO_DOWN_PORT        GPIOD
  #define JOY_GPIO_DOWN_PIN         GPIO_Pin_6
  #define JOY_IT_GPIO_PORT          GPIOB
  #define JOY_IT_GPIO_PIN           GPIO_Pin_14
  #define JOY_GPIO_PORT_SOURCE      GPIO_PortSourceGPIOB
  #define JOY_EXTI_IRQ_CHANNEL      EXTI15_10_IRQn  
  #define JOY_EXTI_LINE             EXTI_Line14       
  #define JOY_GPIO_RCC_BUS_PERIPH1  RCC_APB2Periph_GPIOG
  #define JOY_GPIO_RCC_BUS_PERIPH2  RCC_APB2Periph_GPIOG
  #define JOY_GPIO_RCC_BUS_PERIPH3  RCC_APB2Periph_GPIOG
  #define JOY_GPIO_RCC_BUS_PERIPH4  RCC_APB2Periph_GPIOG
  #define JOY_GPIO_RCC_BUS_PERIPH5  RCC_APB2Periph_GPIOD

#elif USE_STM3210E_EVAL
  /* Joystick GPIO DEFINES for STM3210E-EVAL board */
  #define JOY_GPIO_PIN_SOURCE       GPIO_PinSource14
  #define JOY_GPIO_SELECT_PORT      GPIOG
  #define JOY_GPIO_SELECT_PIN       GPIO_Pin_7
  #define JOY_GPIO_RIGHT_PORT       GPIOG
  #define JOY_GPIO_RIGHT_PIN        GPIO_Pin_13
  #define JOY_GPIO_LEFT_PORT        GPIOG
  #define JOY_GPIO_LEFT_PIN         GPIO_Pin_14
  #define JOY_GPIO_UP_PORT          GPIOG
  #define JOY_GPIO_UP_PIN           GPIO_Pin_15
  #define JOY_GPIO_DOWN_PORT        GPIOD
  #define JOY_GPIO_DOWN_PIN         GPIO_Pin_3
  #define JOY_GPIO_RCC_BUS_PERIPH1  RCC_APB2Periph_GPIOG
  #define JOY_GPIO_RCC_BUS_PERIPH2  RCC_APB2Periph_GPIOG
  #define JOY_GPIO_RCC_BUS_PERIPH3  RCC_APB2Periph_GPIOG
  #define JOY_GPIO_RCC_BUS_PERIPH4  RCC_APB2Periph_GPIOG
  #define JOY_GPIO_RCC_BUS_PERIPH5  RCC_APB2Periph_GPIOD

#elif USE_STM3210B_EVAL
   /* Joystick GPIO DEFINES for STM3210E-EVAL board */
  #define JOY_GPIO_PIN_SOURCE       GPIO_PinSource14 
  
  #define JOY_GPIO_SELECT_PORT      GPIOD
  #define JOY_GPIO_SELECT_PIN       GPIO_Pin_12

  #define JOY_GPIO_RIGHT_PORT       GPIOE
  #define JOY_GPIO_RIGHT_PIN        GPIO_Pin_0

  #define JOY_GPIO_LEFT_PORT        GPIOE
  #define JOY_GPIO_LEFT_PIN         GPIO_Pin_1

  #define JOY_GPIO_UP_PORT          GPIOD
  #define JOY_GPIO_UP_PIN           GPIO_Pin_8

  #define JOY_GPIO_DOWN_PORT        GPIOD
  #define JOY_GPIO_DOWN_PIN         GPIO_Pin_14

  #define JOY_GPIO_RCC_BUS_PERIPH1  RCC_APB2Periph_GPIOD
  #define JOY_GPIO_RCC_BUS_PERIPH2  RCC_APB2Periph_GPIOE
  #define JOY_GPIO_RCC_BUS_PERIPH3  RCC_APB2Periph_GPIOE
  #define JOY_GPIO_RCC_BUS_PERIPH4  RCC_APB2Periph_GPIOD
  #define JOY_GPIO_RCC_BUS_PERIPH5  RCC_APB2Periph_GPIOD

#elif USE_STM32100B_EVAL
  /* Joystick GPIO DEFINES for STM32100B-EVAL board */
  #define JOY_GPIO_PIN_SOURCE       GPIO_PinSource14

  #define JOY_GPIO_SELECT_PORT      GPIOD
  #define JOY_GPIO_SELECT_PIN       GPIO_Pin_12

  #define JOY_GPIO_RIGHT_PORT       GPIOE
  #define JOY_GPIO_RIGHT_PIN        GPIO_Pin_1

  #define JOY_GPIO_LEFT_PORT        GPIOE
  #define JOY_GPIO_LEFT_PIN         GPIO_Pin_0

  #define JOY_GPIO_UP_PORT          GPIOD
  #define JOY_GPIO_UP_PIN           GPIO_Pin_8

  #define JOY_GPIO_DOWN_PORT        GPIOD
  #define JOY_GPIO_DOWN_PIN         GPIO_Pin_14

  #define JOY_GPIO_RCC_BUS_PERIPH1  RCC_APB2Periph_GPIOD
  #define JOY_GPIO_RCC_BUS_PERIPH2  RCC_APB2Periph_GPIOE
  #define JOY_GPIO_RCC_BUS_PERIPH3  RCC_APB2Periph_GPIOE
  #define JOY_GPIO_RCC_BUS_PERIPH4  RCC_APB2Periph_GPIOD
  #define JOY_GPIO_RCC_BUS_PERIPH5  RCC_APB2Periph_GPIOD
   
#elif USE_STM32100E_EVAL
  /* Joystick GPIO DEFINES for STM32100E-EVAL board */
  #define JOY_GPIO_PORT_SOURCE      NULL
  #define JOY_GPIO_PIN_SOURCE       GPIO_PinSource14

  #define JOY_GPIO_SELECT_PORT      GPIOG
  #define JOY_GPIO_SELECT_PIN       GPIO_Pin_7

  #define JOY_GPIO_RIGHT_PORT       GPIOG
  #define JOY_GPIO_RIGHT_PIN        GPIO_Pin_13

  #define JOY_GPIO_LEFT_PORT        GPIOG
  #define JOY_GPIO_LEFT_PIN         GPIO_Pin_14

  #define JOY_GPIO_UP_PORT          GPIOG
  #define JOY_GPIO_UP_PIN           GPIO_Pin_15

  #define JOY_GPIO_DOWN_PORT        GPIOG
  #define JOY_GPIO_DOWN_PIN         GPIO_Pin_11

  #define JOY_GPIO_RCC_BUS_PERIPH1  RCC_APB2Periph_GPIOG
  #define JOY_GPIO_RCC_BUS_PERIPH2  RCC_APB2Periph_GPIOG
  #define JOY_GPIO_RCC_BUS_PERIPH3  RCC_APB2Periph_GPIOG
  #define JOY_GPIO_RCC_BUS_PERIPH4  RCC_APB2Periph_GPIOG
  #define JOY_GPIO_RCC_BUS_PERIPH5  RCC_APB2Periph_GPIOG

#elif defined(USE_STM322xG_EVAL)  
   
   /* Joystick Params DEFINES for STM322xG-EVAL */
  #define JOY_GPIO_PIN_SOURCE       GPIO_PinSource2
  #define JOY_GPIO_PORT_SOURCE      EXTI_PortSourceGPIOI    
  #define JOY_EXTI_IRQ_CHANNEL      EXTI2_IRQn 
  #define JOY_EXTI_LINE             EXTI_Line2
  #define JOY_IT_GPIO_PORT          GPIOI
  #define JOY_IT_GPIO_PIN           GPIO_Pin_2   



  #define JOY_GPIO_SELECT_PIN       GL_JOY_IO_SEL
  #define JOY_GPIO_RIGHT_PIN        GL_JOY_IO_RIGHT
  #define JOY_GPIO_LEFT_PIN         GL_JOY_IO_LEFT
  #define JOY_GPIO_UP_PIN           GL_JOY_IO_UP
  #define JOY_GPIO_DOWN_PIN         GL_JOY_IO_DOWN

#elif USE_STM32L152_EVAL

  #define JOY_GPIO_SELECT_PORT      GPIOE
  #define JOY_GPIO_SELECT_PIN       GPIO_Pin_8

  #define JOY_GPIO_RIGHT_PORT       GPIOE
  #define JOY_GPIO_RIGHT_PIN        GPIO_Pin_11

  #define JOY_GPIO_LEFT_PORT        GPIOE
  #define JOY_GPIO_LEFT_PIN         GPIO_Pin_12

  #define JOY_GPIO_UP_PORT          GPIOE
  #define JOY_GPIO_UP_PIN           GPIO_Pin_9

  #define JOY_GPIO_DOWN_PORT        GPIOE
  #define JOY_GPIO_DOWN_PIN         GPIO_Pin_10

  #define JOY_GPIO_RCC_BUS_PERIPH1  RCC_AHBPeriph_GPIOE
  #define JOY_GPIO_RCC_BUS_PERIPH2  RCC_AHBPeriph_GPIOE
  #define JOY_GPIO_RCC_BUS_PERIPH3  RCC_AHBPeriph_GPIOE
  #define JOY_GPIO_RCC_BUS_PERIPH4  RCC_AHBPeriph_GPIOE
  #define JOY_GPIO_RCC_BUS_PERIPH5  RCC_AHBPeriph_GPIOE
   
#else

  #error define a board

#endif
#endif

#if 0
/** 
  * @brief   KEY Button Controller DEFINES  
  */ 
/* User Button GPIO Port and Pin*/
#if defined(USE_STM3210C_EVAL)
  #define USER_BUTTON_BUS_PERIPH     RCC_APB2Periph_GPIOB   
  #define USER_BUTTON_PORT           GPIOB
  #define USER_BUTTON_PIN            GPIO_Pin_9
  #define USER_BUTTON_PIN_ACTIVE     Bit_RESET   
#elif defined(USE_STM3210E_EVAL)
  #define USER_BUTTON_BUS_PERIPH     RCC_APB2Periph_GPIOG   
  #define USER_BUTTON_PORT           GPIOG
  #define USER_BUTTON_PIN            GPIO_Pin_8
  #define USER_BUTTON_PIN_ACTIVE     Bit_RESET   
#elif defined(USE_STM3210B_EVAL)
  #define USER_BUTTON_BUS_PERIPH     RCC_APB2Periph_GPIOB   
  #define USER_BUTTON_PORT           GPIOB
  #define USER_BUTTON_PIN            GPIO_Pin_9
  #define USER_BUTTON_PIN_ACTIVE     Bit_RESET   
#elif defined(USE_STM32100B_EVAL)
  #define USER_BUTTON_BUS_PERIPH     RCC_APB2Periph_GPIOB   
  #define USER_BUTTON_PORT           GPIOB
  #define USER_BUTTON_PIN            GPIO_Pin_9
  #define USER_BUTTON_PIN_ACTIVE     Bit_RESET   
#elif defined(USE_STM32100E_EVAL)
  #define USER_BUTTON_BUS_PERIPH     RCC_APB2Periph_GPIOG   
  #define USER_BUTTON_PORT           GPIOG
  #define USER_BUTTON_PIN            GPIO_Pin_8
  #define USER_BUTTON_PIN_ACTIVE     Bit_RESET   
#elif defined(USE_STM322xG_EVAL)
  #define USER_BUTTON_BUS_PERIPH     RCC_AHB1Periph_GPIOG
  #define USER_BUTTON_PORT           GPIOG
  #define USER_BUTTON_PIN            GPIO_Pin_15 
  #define USER_BUTTON_PIN_ACTIVE     Bit_RESET   
#elif defined(USE_STM32L152_EVAL)
  #define USER_BUTTON_BUS_PERIPH     RCC_AHBPeriph_GPIOA
  #define USER_BUTTON_PORT           GPIOA
  #define USER_BUTTON_PIN            GPIO_Pin_0   
  #define USER_BUTTON_PIN_ACTIVE     Bit_SET
#else
  #error define a board
#endif
#endif

/** 
  * @brief   LCD Controller DEFINES  
  */ 
#ifdef USE_STM3210C_EVAL
  /* LCD Controller DEFINES for STM3210C-EVAL board */
  #define LCD_CTRL_PORT_NCS            GPIOB
  #define LCD_GPIO_DATA_PORT           GPIOC
  #define LCD_CTRL_PIN_NCS             GPIO_Pin_2
  #define LCD_GPIO_PIN_SCK             GPIO_Pin_10
  #define LCD_GPIO_PIN_MISO            GPIO_Pin_11
  #define LCD_GPIO_PIN_MOSI            GPIO_Pin_12
  #define LCD_GPIO_RCC_BUS_PERIPH      RCC_APB2Periph_GPIOC
  #define LCD_GPIO_RCC_BUS_PERIPH_NCS  RCC_APB2Periph_GPIOB
  #define LCD_GPIO_REMAP_PORT          GPIO_Remap_SPI3
  #define LCD_RCC_BUS_PERIPH           RCC_APB1Periph_SPI3
  #define LCD_SPI_PORT                 SPI3
  #define LCD_CONNECTION_MODE          GL_SPI

#elif USE_STM3210B_EVAL

  /* LCD Controller DEFINES for STM3210B-EVAL board */
  #define LCD_CTRL_PORT_NCS            GPIOB
  #define LCD_GPIO_DATA_PORT           GPIOB
  #define LCD_CTRL_PIN_NCS             GPIO_Pin_2
  #define LCD_GPIO_PIN_SCK             GPIO_Pin_13
  #define LCD_GPIO_PIN_MISO            GPIO_Pin_14
  #define LCD_GPIO_PIN_MOSI            GPIO_Pin_15
  #define LCD_GPIO_RCC_BUS_PERIPH      RCC_APB2Periph_GPIOB
  #define LCD_GPIO_RCC_BUS_PERIPH_NCS  RCC_APB2Periph_GPIOB
  #define LCD_RCC_BUS_PERIPH           RCC_APB1Periph_SPI2
  #define LCD_SPI_PORT                 SPI2
  #define LCD_GPIO_REMAP_PORT          ((uint32_t)0)
  #define LCD_CONNECTION_MODE          GL_SPI

#elif USE_STM32100B_EVAL

  /* LCD Controller DEFINES for STM32100B-EVAL board  */
  #define LCD_CTRL_PORT_NCS            GPIOB
  #define LCD_GPIO_DATA_PORT           GPIOB
  #define LCD_CTRL_PIN_NCS             GPIO_Pin_2
  #define LCD_GPIO_PIN_SCK             GPIO_Pin_13
  #define LCD_GPIO_PIN_MISO            GPIO_Pin_14
  #define LCD_GPIO_PIN_MOSI            GPIO_Pin_15
  #define LCD_GPIO_RCC_BUS_PERIPH      RCC_APB2Periph_GPIOB
  #define LCD_GPIO_RCC_BUS_PERIPH_NCS  RCC_APB2Periph_GPIOB
  #define LCD_RCC_BUS_PERIPH           RCC_APB1Periph_SPI2
  #define LCD_SPI_PORT                 SPI2
  #define LCD_GPIO_REMAP_PORT          ((uint32_t)0)
  #define LCD_CONNECTION_MODE          GL_SPI
   
#elif USE_STM32L152_EVAL 
 
  /* LCD Controller DEFINES for STM32L152-EVAL board */
  #define LCD_CTRL_PORT_NCS            GPIOH
  #define LCD_GPIO_DATA_PORT           GPIOE
  #define LCD_CTRL_PIN_NCS             GPIO_Pin_2
  #define LCD_GPIO_PIN_SCK             GPIO_Pin_13
  #define LCD_GPIO_PIN_MISO            GPIO_Pin_14
  #define LCD_GPIO_PIN_MOSI            GPIO_Pin_15
  #define LCD_GPIO_RCC_BUS_PERIPH      RCC_AHBPeriph_GPIOE
  #define LCD_GPIO_RCC_BUS_PERIPH_NCS  RCC_AHBPeriph_GPIOH
  #define LCD_RCC_BUS_PERIPH           RCC_APB2Periph_SPI1
  #define LCD_SPI_PORT                 SPI1
  #define LCD_GPIO_REMAP_PORT          ((uint32_t)0)
  #define LCD_CONNECTION_MODE          GL_SPI
   
#elif defined(USE_STM322xG_EVAL)
  /* LCD Controller DEFINES for STM322xG-EVAL board */
  #define LCD_RCC_BUS_PERIPH           RCC_AHB3Periph_FSMC
  #define LCD_CONNECTION_MODE          GL_FSMC   
#else
  /* LCD Controller DEFINES for STM3210E-EVAL & STM32100E-EVAL boards */
  #define LCD_RCC_BUS_PERIPH           RCC_AHBPeriph_FSMC
  #define LCD_CONNECTION_MODE          GL_FSMC
#endif


/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */ 
   void HWConfig_SetHardwareParams(void);
   
#ifdef __cplusplus
}
#endif

#endif /* __HARDWARE_CONFIG_H */

   
/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
