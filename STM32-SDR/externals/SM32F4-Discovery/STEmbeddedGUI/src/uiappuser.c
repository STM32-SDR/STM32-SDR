/**
  ******************************************************************************
  * @file    uiappuser.c
  * @author  IMS Systems LAB & Technical Marketing
  * @version V2.0.0
  * @date    11-July-2011
  * @brief   Event Handlers
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

/*includes----------------------------------------------------------------------*/
#include "uiframework.h"
#include "uiappuser.h"
#include "LcdHal.h"
#include "uiappuser.h"

/**@addtogroup Embedded_GUI_Example
  *@{
  */

/** @defgroup uiappuser
  * @brief uiappuser main functions
  *@{
  */

/* External variables --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  LedControlExample_Start_button_Click()
  *         This function gets called when User clicks Button Start_button.
  * @param  None
  * @retval None
  */
void LedControlExample_Start_button_Click()
{
  Show_LedCtrlScreen();
}

/**
  * @brief  LedCtrlScreen_Led1_Switch_Toggle()
  *         This function gets called when User toggles the  SwitchButton Led1_Switch.
  * @param  None
  * @retval None
  */
void LedCtrlScreen_Led1_Switch_Toggle()
{
  STM_EVAL_LEDToggle(LED1);
}

/**
  * @brief  LedCtrlScreen_Led2_Switch_Toggle()
  *         This function gets called when User toggles the  SwitchButton Led2_Switch.
  * @param  None
  * @retval None
  */
void LedCtrlScreen_Led2_Switch_Toggle()
{
  STM_EVAL_LEDToggle(LED2);
}

/**
  * @brief  LedCtrlScreen_Led3_Switch_Toggle()
  *         This function gets called when User toggles the  SwitchButton Led3_Switch.
  * @param  None
  * @retval None
  */
void LedCtrlScreen_Led3_Switch_Toggle()
{
  STM_EVAL_LEDToggle(LED3);
}

/**
  * @brief  LedCtrlScreen_Led4_Switch_Toggle()
  *         This function gets called when User toggles the  SwitchButton Led4_Switch.
  * @param  None
  * @retval None
  */
void LedCtrlScreen_Led4_Switch_Toggle()
{
  STM_EVAL_LEDToggle(LED4);
}

/**
  * @brief  LedCtrlScreen_Back_Button_Click()
  *         This function gets called when User clicks Button Back_Button.
  * @param  None
  * @retval None
  */
void LedCtrlScreen_Back_Button_Click()
{
  Show_LedControlExample();
}

/**
  *@}
  */

/**
  *@}
  */

/**
  *@}
  */
/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

