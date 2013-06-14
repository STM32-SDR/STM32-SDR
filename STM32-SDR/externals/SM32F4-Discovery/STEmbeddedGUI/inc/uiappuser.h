/**
  ******************************************************************************
  * @file    uiappuser.h
  * @author	 IMS Systems LAB & Technical Marketing
  * @version V2.0.0
  * @date    11-July-2011
  * @brief   Event HandlerDeclarations
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
#ifndef __UIAPPUSER
#define __UIAPPUSER

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/**@addtogroup Embedded_GUI_Example
*@{
*/

/**@addtogroup uiappuser
*@{
*/

/**@defgroup uiappuser_Exported_Types
*@{
*/

/**
*@}
*/

/**@brief uiappuser_Exported_Functions
*@{
*/

 /**
* @brief  LedControlExample_Start_button_Click()
*         This function gets called when User clicks Button Start_button.
* @param  None
* @retval None
*/
void LedControlExample_Start_button_Click(void);
  
/**
* @brief  LedCtrlScreen_Led1_Switch_Toggle()
*         This function gets called when User toggles the  SwitchButton Led1_Switch.
* @param  None
* @retval None
*/
void LedCtrlScreen_Led1_Switch_Toggle(void);
/**
* @brief  LedCtrlScreen_Led2_Switch_Toggle()
*         This function gets called when User toggles the  SwitchButton Led2_Switch.
* @param  None
* @retval None
*/
void LedCtrlScreen_Led2_Switch_Toggle(void);
/**
* @brief  LedCtrlScreen_Led3_Switch_Toggle()
*         This function gets called when User toggles the  SwitchButton Led3_Switch.
* @param  None
* @retval None
*/
void LedCtrlScreen_Led3_Switch_Toggle(void);
/**
* @brief  LedCtrlScreen_Led4_Switch_Toggle()
*         This function gets called when User toggles the  SwitchButton Led4_Switch.
* @param  None
* @retval None
*/
void LedCtrlScreen_Led4_Switch_Toggle(void);
    /**
* @brief  LedCtrlScreen_Back_Button_Click()
*         This function gets called when User clicks Button Back_Button.
* @param  None
* @retval None
*/
void LedCtrlScreen_Back_Button_Click(void);

/**
*@}
*/

/**
*@}
*/

/**
*@}
*/

#ifdef __cplusplus
}
#endif

#endif /*__uiappuser */ 
/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/ 

