/**
  ******************************************************************************
  * @file    uiframework.h
  * @author	 IMS Systems LAB & Technical Marketing
  * @version V2.0.0
  * @date    11-July-2011
  * @brief   Screen Declarations
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
#ifndef __UIFRAMEWORK
#define __UIFRAMEWORK

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "graphicObjectTypes.h"
#include "graphicObject.h"

/**@addtogroup Embedded_GUI_Example
  *@{
  */

/**@addtogroup uiframework
  *@{
  */

/* External variables --------------------------------------------------------*/
extern GL_Page_TypeDef *CurrentScreen;
extern GL_Page_TypeDef LedControlExample;
extern GL_Page_TypeDef LedCtrlScreen;

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/ 
#define LED_CTRLColor 31
#define DesignLabel01Color 0
#define DesignLabel02Color 0
#define LED1_Switch_LabelColor 0
#define LED2_Switch_LabelColor 0
#define LED3_Switch_LabelColor 0
#define LED4_Switch_LabelColor 0


/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void Create_LedControlExample(void);
void Create_LedCtrlScreen(void);
void Show_LedControlExample(void);
void Show_LedCtrlScreen(void);
void Show_HomeScreen(void);   

/**
  *@}
  */

/**
  *@}
  */

#ifdef __cplusplus
}
#endif
  
#endif /*__uiframework */ 
/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/ 

