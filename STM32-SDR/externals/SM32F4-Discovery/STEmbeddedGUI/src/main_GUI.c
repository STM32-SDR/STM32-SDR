/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    11-July-2011
  * @brief   This file provides main program functions.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "touchscreen.h"
#include "hw_config.h"
#include "cursor.h"
#include "graphicObject.h"
#include "pictures.h"
#include "uiframework.h"
#include <stdio.h>

/** @addtogroup Embedded_GUI_Example
  * @{
  */

/** @defgroup Main
  * @brief Main program body
  * @{
  */

/** @addtogroup Embedded_GUI_Example
  * @{
  */

/** @defgroup Main
  * @brief Main program body
  * @{
  */

/* External variables --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
#if defined(USE_STM32100E_EVAL)
#define LAST_FLASH_MEMORY_ADDRESS	((uint32_t)0x08080000)
#elif defined(USE_STM322xG_EVAL)
#define LAST_FLASH_MEMORY_ADDRESS	((uint32_t)0x08100000)
#elif defined(USE_STM3210C_EVAL)
#define LAST_FLASH_MEMORY_ADDRESS	((uint32_t)0x08040000)
#endif
/* Private macros ------------------------------------------------------------*/

/**
  * @brief   Small printf for GCC/RAISONANCE
  */
#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)

#endif /* __GNUC__ */

/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void InputInterface_Init(void);
void ShowLoadingLogo(void);
void CatchInputEvents(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
  RCC_ClocksTypeDef RCC_Clocks;

  /* Set the Vector Table base location at 0x08000000 */
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0000);
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

  /* Setup SysTick Timer for 10 msec interrupts  */
  RCC_GetClocksFreq(&RCC_Clocks);
  if (SysTick_Config(RCC_Clocks.SYSCLK_Frequency / 100))
  {
    /* Capture error */
    while (1);
  }
  /* configure Systick priority */
  NVIC_SetPriority(SysTick_IRQn, 0x0B);

  /* Set HW structure parameters */
  HWConfig_SetHardwareParams();


  /* If the LCD Panel has a resolution of 320x240 this command is not needed, it's set by default */
  /* Set_LCD_Resolution( 320, 240 ); */

  /* Initialize the LCD */
  GL_LCD_Init();

  GL_Clear(GL_White);

  InputInterface_Init();

#if TOUCH_SCREEN_CAPABILITY
  /* Check if Calibration has been done*/
  TS_CheckCalibration();
#endif

  /*Initialize cursor*/
  GL_Clear(White);
  CursorInit(GL_NULL);

  /* Menu Initialisation*/
  Show_HomeScreen();
  CursorShow(195, 80);
  STM_EVAL_LEDInit(LED1);
  STM_EVAL_LEDInit(LED2);
  STM_EVAL_LEDInit(LED3);
  STM_EVAL_LEDInit(LED4);
  /* Infinite main loop ------------------------------------------------------*/
  while (1)
  {

#if defined(USE_STM3210C_EVAL) || defined(USE_STM32100E_EVAL)
    /* Catching touch events */
    if ( TS_IsCalibrationDone() == SET )
#endif
    {
      ProcessInputData();
    }

    /* Time out calculate for power saving mode */
    TimeOutCalculate();

    CatchInputEvents();
  }
}

/**
  * @brief  Poll on Input devices to check for a new event
  * @param  None
  * @retval None
  */
void CatchInputEvents(void)
{
#if TOUCH_SCREEN_CAPABILITY
  /* catch touch screen events */
  TSC_Read();
#endif

#if defined(USE_STM3210C_EVAL) || defined(USE_STM322xG_EVAL)
  /* for STM3210C/STM322xG eval board joystick is interfaced with IOE */
  CursorReadJoystick(IOEXP_MODE);
#else
  /* Use IO polling */
  CursorReadJoystick(POLLING_MODE);
#endif
}

/**
  * @brief  Enable input interfaces
  * @param  None
  * @retval None
  */
void InputInterface_Init(void)
{
#if TOUCH_SCREEN_CAPABILITY
  uint32_t time_out = 0xFFF;
#endif

#if defined(USE_STM3210E_EVAL) || defined(USE_STM32100B_EVAL) ||\
  defined(USE_STM3210B_EVAL) || defined(USE_STM32100E_EVAL)
  /* Joystick Init */
  GL_JoyStickConfig_GPIO();

#elif  defined(USE_STM3210C_EVAL) || defined(USE_STM322xG_EVAL)
  /* Joystick Init */
  GL_JoyStickConfig_IOExpander();
#endif
  GL_ButtonInit();
  
#if TOUCH_SCREEN_CAPABILITY

  /* Set the last Flash Memory address */
  Set_LastFlashMemoryAddress( LAST_FLASH_MEMORY_ADDRESS );

  /* Touch Screen Init */
  TSC_Init();

  /* If key is pressed - Start Calibration */
  while ( GPIO_ReadInputDataBit(USER_BUTTON_PORT, USER_BUTTON_PIN) && (time_out > 0) )
  {
    time_out--;
  }
  if (time_out > 0)
  {
    TS_Calibration();
  }
#endif
  
}

#ifdef __GNUC__
/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(EVAL_COM1, (uint8_t) ch);

  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(EVAL_COM1, USART_FLAG_TC) == RESET)
  {}

  return ch;
}
#endif /* __GNUC__ */

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
