/**
  ******************************************************************************
  * @file    uiframework.c
  * @author  IMS Systems LAB & Technical Marketing
  * @version V2.0.0
  * @date    11-July-2011
  * @brief   Screen Code
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

/*includes---------------------------------------------------------------------*/
#include "uiframework.h"
#include "uiappuser.h"
#include "float.h"
#include "LcdHal.h"
#include "TscHal.h"
#include "pictures.h"

/**@addtogroup Embedded_GUI_Example
*@{
*/

/** @defgroup uiframework
* @brief uiframework main functions
*@{
*/

/* External variables --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/**
  *@brief the current screen is
  */
GL_Page_TypeDef *CurrentScreen;

GL_Page_TypeDef LedControlExample;
GL_Page_TypeDef LedCtrlScreen;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  *@brief Develops screen and generates control
  *@param None
  *@retval None
  */
void Create_LedControlExample(void)
{
  GL_PageControls_TypeDef* LED_CTRL = NewLabel(1, "LED Control Example", GL_HORIZONTAL, GL_FONT_BIG, LED_CTRLColor);
  GL_PageControls_TypeDef* Start_button = NewButton(2, "Start", LedControlExample_Start_button_Click);
  GL_PageControls_TypeDef* DesignLabel01 = NewLabel(3, "This example allows to switch", GL_HORIZONTAL, GL_FONT_SMALL, DesignLabel01Color);
  GL_PageControls_TypeDef* DesignLabel02 = NewLabel(4, "On/Off LEDs via switch buttons.", GL_HORIZONTAL, GL_FONT_SMALL, DesignLabel02Color);
  Create_PageObj( &LedControlExample );
  AddPageControlObj(311, 17, LED_CTRL, &LedControlExample);
  AddPageControlObj(187, 135, Start_button, &LedControlExample);
  AddPageControlObj(275, 64, DesignLabel01, &LedControlExample);
  AddPageControlObj(283, 86, DesignLabel02, &LedControlExample);
}


/**
  *@brief Develops screen and generates control
  *@param None
  *@retval None
  */
void Create_LedCtrlScreen(void)
{
  GL_PageControls_TypeDef* Led1_Switch = NewSwitch(5, "LED1 On", "LED1 Off", LedCtrlScreen_Led1_Switch_Toggle);
  GL_PageControls_TypeDef* Led2_Switch = NewSwitch(6, "LED2 On", "LED2 Off", LedCtrlScreen_Led2_Switch_Toggle);
  GL_PageControls_TypeDef* Led3_Switch = NewSwitch(7, "LED3 On", "LED3 Off", LedCtrlScreen_Led3_Switch_Toggle);
  GL_PageControls_TypeDef* Led4_Switch = NewSwitch(8, "LED4 On", "LED4 Off", LedCtrlScreen_Led4_Switch_Toggle);
  GL_PageControls_TypeDef* LED1_Switch_Label = NewLabel(9, "LED1 Control Switch", GL_HORIZONTAL, GL_FONT_SMALL, LED1_Switch_LabelColor);
  GL_PageControls_TypeDef* LED2_Switch_Label = NewLabel(10, "LED2 Control Switch", GL_HORIZONTAL, GL_FONT_SMALL, LED2_Switch_LabelColor);
  GL_PageControls_TypeDef* LED3_Switch_Label = NewLabel(11, "LED3 Control Switch", GL_HORIZONTAL, GL_FONT_SMALL, LED3_Switch_LabelColor);
  GL_PageControls_TypeDef* LED4_Switch_Label = NewLabel(12, "LED4 Control Switch", GL_HORIZONTAL, GL_FONT_SMALL, LED4_Switch_LabelColor);
  GL_PageControls_TypeDef* Back_Button = NewButton(13, "Back", LedCtrlScreen_Back_Button_Click);
  Create_PageObj( &LedCtrlScreen );
  AddPageControlObj(104, 10, Led1_Switch, &LedCtrlScreen);
  AddPageControlObj(103, 50, Led2_Switch, &LedCtrlScreen);
  AddPageControlObj(102, 89, Led3_Switch, &LedCtrlScreen);
  AddPageControlObj(103, 130, Led4_Switch, &LedCtrlScreen);
  AddPageControlObj(300, 18, LED1_Switch_Label, &LedCtrlScreen);
  AddPageControlObj(300, 53, LED2_Switch_Label, &LedCtrlScreen);
  AddPageControlObj(300, 94, LED3_Switch_Label, &LedCtrlScreen);
  AddPageControlObj(300, 133, LED4_Switch_Label, &LedCtrlScreen);
  AddPageControlObj(183, 186, Back_Button, &LedCtrlScreen);
}




/**
  *@brief Shows the screen
  *@param None
  *@retval None
  */
void Show_LedControlExample(void)
{
  if (CurrentScreen != GL_NULL)
  {
    CurrentScreen->ShowPage(CurrentScreen, GL_FALSE);
  }
  GL_Clear(65535);
  GL_SetTextColor(63488);
  LedControlExample.ShowPage(&LedControlExample, GL_TRUE);
  CurrentScreen = &LedControlExample;
}

/**
  *@brief Shows the screen
  *@param None
  *@retval None
  */
void Show_LedCtrlScreen(void)
{
  if (CurrentScreen != GL_NULL)
  {
    CurrentScreen->ShowPage(CurrentScreen, GL_FALSE);
  }
  GL_Clear(65535);
  GL_SetTextColor(0);
  LedCtrlScreen.ShowPage(&LedCtrlScreen, GL_TRUE);
  CurrentScreen = &LedCtrlScreen;
}

/**
  *@brief Controls home screen
  *@param None
  *@retval None
  */
void Show_HomeScreen(void)
{
  Create_LedControlExample();
  Create_LedCtrlScreen();
  CurrentScreen = &LedControlExample;
  GL_Clear(65535);
  GL_SetTextColor(63488);
  LedControlExample.ShowPage(&LedControlExample, GL_TRUE);
}

/**
  *@}
  */

/**
  *@}
  */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

