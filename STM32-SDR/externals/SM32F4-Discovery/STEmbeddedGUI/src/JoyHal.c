/**
  ******************************************************************************
  * @file    JoyHal.c
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    11-July-2011
  * @brief   This file contains all the Joystick functions whose
  *          implementation depends on the Controller used in your Design.
  *          You only need to change these functions implementations
  *          in order to reuse this code with other Controller 
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

/* Includes ---------------------------------------------------------------------------*/
#include "JoyHal.h"
#include "graphicObjectTypes.h"

#ifdef USE_STM3210C_EVAL
   #include "stm3210c_eval_ioe.h"
#elif USE_STM322xG_EVAL
   #include "stm322xg_eval_ioe.h"
#endif

/** @addtogroup Embedded_GUI_Library
  * @{
  */

/** @defgroup JoyHal 
  * @brief JoyHal main functions
  * @{
  */
  
/* External variables --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Joystick Hardware Parameters Structure */
JOY_HW_Parameters_TypeDef pJoyHwParam; 

/* BUTTON Hardware Parameters Structure */
BTN_HW_Parameters_TypeDef pBtnHwParam; 

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup JoyHal_Private_Functions
  * @{
  */

/**
  * @brief  Create and initialize a new Joystick Hw Parameter structure object
  * @param  None
  * @retval JOY_HW_Parameters_TypeDef*, The created Object pointer
  */
JOY_HW_Parameters_TypeDef* NewJoyHwParamObj (void)
{
  return &pJoyHwParam;
}

/**
  * @brief  Create and initialize a new Button Hw Parameter structure object
  * @param  None
  * @retval BTN_HW_Parameters_TypeDef*, The created Object pointer
  */
BTN_HW_Parameters_TypeDef* NewBtnHwParamObj (void)
{
  return &pBtnHwParam;
}

/**
  * @brief  Initializes the IO Expander registers.
  * @param  None
  * @retval - 0: if all initializations are OK.
  */
uint32_t GL_JOY_Interface_Init(void)
{
#if defined(USE_STM3210C_EVAL) || defined(USE_STM322xG_EVAL)
  IOE_Config();
  
  /* Read IO Expander ID  */
  if (IOE_IsOperational(pJoyHwParam.JOY_DeviceRegister))
  {
    return 0x01;
  }
#endif
  return 0; /* Configuration is OK */
}

/**
  * @brief  Initializes the IO Expander for JoyStick operations.
  * @param  None
  * @retval - 0: if all initializations are OK.
  */
void GL_JoyStickConfig_IOExpander(void)
{
#if defined(USE_STM3210C_EVAL) || defined(USE_STM322xG_EVAL)
  GL_JOY_Interface_Init();
#endif
}

/**
  * @brief  Configures the GPIO ports pins concerned with joystick.
  * @param  None
  * @retval None
  */
void GL_JoyStickConfig_GPIO(void)
{
#if !defined(USE_STM322xG_EVAL) && !defined(USE_STM3210C_EVAL)
  GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_StructInit((GPIO_InitTypeDef*)&GPIO_InitStructure);
  
#if defined(STM32L1XX_MD)
    /* Enable GPIOx clock */
  RCC_AHBPeriphClockCmd( pJoyHwParam.JOY_Rcc_BusPeriph_GpioSel | 
                            pJoyHwParam.JOY_Rcc_BusPeriph_GpioRight | 
                            pJoyHwParam.JOY_Rcc_BusPeriph_GpioLeft | 
                            pJoyHwParam.JOY_Rcc_BusPeriph_GpioUp | 
                            pJoyHwParam.JOY_Rcc_BusPeriph_GpioDown, ENABLE);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
#else
  /* Enable GPIOx clock */
  RCC_APB2PeriphClockCmd( pJoyHwParam.JOY_Rcc_BusPeriph_GpioSel | 
                            pJoyHwParam.JOY_Rcc_BusPeriph_GpioRight | 
                            pJoyHwParam.JOY_Rcc_BusPeriph_GpioLeft | 
                            pJoyHwParam.JOY_Rcc_BusPeriph_GpioUp | 
                            pJoyHwParam.JOY_Rcc_BusPeriph_GpioDown, ENABLE);
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
#endif
  
  /* Configure Joystick Select Pin as input Push Down */
  GPIO_InitStructure.GPIO_Pin = pJoyHwParam.JOY_Select_Pin;

  GPIO_Init(pJoyHwParam.JOY_Select_Port, &GPIO_InitStructure);

  /* Configure Joystick Right Pin as input Push Down */
  GPIO_InitStructure.GPIO_Pin = pJoyHwParam.JOY_Right_Pin;

  GPIO_Init(pJoyHwParam.JOY_Right_Port, &GPIO_InitStructure);

  /* Configure Joystick Left Pin as input Push Down */
  GPIO_InitStructure.GPIO_Pin = pJoyHwParam.JOY_Left_Pin;

  GPIO_Init(pJoyHwParam.JOY_Left_Port, &GPIO_InitStructure);

  /* Configure Joystick Up Pin as input Push Down */
  GPIO_InitStructure.GPIO_Pin = pJoyHwParam.JOY_Up_Pin;

  GPIO_Init(pJoyHwParam.JOY_Up_Port, &GPIO_InitStructure);

  /* Configure Joystick Down Pin as input Push Down */
  GPIO_InitStructure.GPIO_Pin = pJoyHwParam.JOY_Down_Pin;

  GPIO_Init(pJoyHwParam.JOY_Down_Port, &GPIO_InitStructure);

#endif  
}

/**
  * @brief  Return the Joystick status.
  * @param  None
  * @retval uint32_t - The code of the Joystick key pressed
  */
uint32_t GL_JoyStickState(JOY_ReadMode mode)
{
  uint32_t val = GL_JOY_NONE;
  if (mode == POLLING_MODE)
  {
     val = GL_JoyStickStatePolling();
  }
#if defined(USE_STM3210C_EVAL) || defined (USE_STM322xG_EVAL)  
  else
  {
    if (mode == IOEXP_MODE)
    {
        val = GL_JoyStickStateIOEXP();
    }
  }
#endif  
    return  val;
}

/**
  * @brief  Return the Joystick status.
  * @param  None
  * @retval uint32_t - The code of the Joystick key pressed
  */
uint32_t GL_JoyStickStateIOEXP(void)
{
#if defined(USE_STM3210C_EVAL) || defined (USE_STM322xG_EVAL)  
  uint8_t tmp = 0;

  /* Check if the Device is opeartaional */
  if (IOE_IsOperational(pJoyHwParam.JOY_DeviceRegister))
  {
    return 0xFF;
  }
  /* Read the status of all pins */
  tmp = (uint32_t)I2C_ReadDeviceRegister(pJoyHwParam.JOY_DeviceRegister, GL_GPIO_MP_STA);
  I2C_WriteDeviceRegister(pJoyHwParam.JOY_DeviceRegister, GL_GPIO_INT_STA, 0xFF);

  /* Check the pressed keys */
  if ((tmp & EDGE_NONE) == EDGE_NONE)
  {
    return (uint32_t)GL_JOY_NONE;
  }

  else if (!(tmp & EDGE_CENTER) )
  {
    return (uint32_t)GL_JOY_CENTER;
  }

  else if (!(tmp & EDGE_DOWN) )
  {
    return (uint32_t)GL_JOY_DOWN;
  }

  else if (!(tmp & EDGE_LEFT) )
  {
    return (uint32_t)GL_JOY_LEFT;
  }

  else if (!(tmp & EDGE_RIGHT) )
  {
    return (uint32_t)GL_JOY_RIGHT;
  }

  else if (!(tmp & EDGE_UP) )
  {
    return (uint32_t)GL_JOY_UP;
  }

  else
#endif    
  {
    return (uint32_t)GL_JOY_NONE;
  }
}


/**
  * @brief  Return the Joystick status.
  * @param  None
  * @retval uint32_t - The code of the Joystick key pressed
  */
uint32_t GL_JoyStickStatePolling(void)
{
  uint32_t time_out = 0xFFF;
  /* Check the pressed keys */
  while ( time_out > 0)
  {
    if ( !GPIO_ReadInputDataBit( pJoyHwParam.JOY_Select_Port, pJoyHwParam.JOY_Select_Pin) )
    {
      return (uint32_t)GL_JOY_CENTER;
    }

    else if ( !GL_GPIO_ReadInputDataBit(pJoyHwParam.JOY_Down_Port, pJoyHwParam.JOY_Down_Pin) )
    {
      return (uint32_t)GL_JOY_DOWN;
    }

    else if ( !GPIO_ReadInputDataBit(pJoyHwParam.JOY_Left_Port, pJoyHwParam.JOY_Left_Pin) )
    {
      return (uint32_t)GL_JOY_LEFT;
    }

    else if ( !GPIO_ReadInputDataBit(pJoyHwParam.JOY_Right_Port, pJoyHwParam.JOY_Right_Pin) )
    {
      return (uint32_t)GL_JOY_RIGHT;
    }

    else if ( !GPIO_ReadInputDataBit(pJoyHwParam.JOY_Up_Port, pJoyHwParam.JOY_Up_Pin) )
    {
      return (uint32_t)GL_JOY_UP;
    }
    time_out--;
  }
  return (uint32_t)GL_JOY_NONE;
}

/**
  * @brief  Configure user key button.
  * @param  none
  * @retval none
  */
void GL_ButtonInit(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_StructInit((GPIO_InitTypeDef*)&GPIO_InitStructure);
  
#if defined(STM32L1XX_MD)
    /* Enable GPIOx clock */
  RCC_AHBPeriphClockCmd( pBtnHwParam.BTN_Rcc_BusPeriph_Gpio, ENABLE);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  
#elif defined(STM32F2XX)
  /* Enable GPIOx clock */
  RCC_AHB1PeriphClockCmd( pBtnHwParam.BTN_Rcc_BusPeriph_Gpio, ENABLE);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  
#else
  /* Enable GPIOx clock */
  RCC_APB2PeriphClockCmd(pBtnHwParam.BTN_Rcc_BusPeriph_Gpio, ENABLE);
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
#endif
  
  GPIO_InitStructure.GPIO_Pin = pBtnHwParam.BTN_Pin;
    
  GPIO_Init(pBtnHwParam.BTN_Port, &GPIO_InitStructure);
}
/**
  * @brief  Reads the specified input port pin.
  * @param  GPIOx: where x can be (A..G) to select the GPIO peripheral.
  * @param  GPIO_Pin: specifies the port bit to read.
  *         This parameter can be GPIO_Pin_x where x can be (0..15).
  * @retval uint8_t - The input port pin value.
  */
uint8_t GL_GPIO_ReadInputDataBit(GPIO_TypeDef * GPIOx, uint16_t GPIO_Pin)
{
  uint8_t i = GPIO_ReadInputDataBit( GPIOx, GPIO_Pin );
  return i;
}

/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
