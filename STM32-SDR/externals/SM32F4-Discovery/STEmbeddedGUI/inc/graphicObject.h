/**
  ******************************************************************************
  * @file    graphicObject.h
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    11-July-2011
  * @brief   This file contains the methods to create the objects that
  *          can be printed on the LCD, and the TouchScreen calibration.
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
#ifndef __GRAPHIC_OBJECT_H
#define __GRAPHIC_OBJECT_H

#ifdef __cplusplus
 extern "C" {
#endif
   
/* Includes ------------------------------------------------------------------*/
#include "graphicObjectTypes.h"

/** @addtogroup Embedded_GUI_Library
  * @{
  */

/** @addtogroup graphicObject 
  * @{
  */ 

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/** @defgroup graphicObject_Exported_Constants
  * @{
  */
#define GL_NULL  0x00
/**
  * @}
  */

/** @defgroup graphicObject_Exported_variables
  * @{
  */
extern GL_Page_TypeDef* PagesList[];      /*!< Pointer to page list */
extern __IO uint8_t touch_done;           /*!< touchscreen event status */
extern __IO uint8_t calibration_done;     /*!< touchscreen calibration status */
extern __IO uint8_t joy_done;             /*!< joystick event status */

extern uint16_t LCD_Height;               /*!< Screen  Height */
extern uint16_t LCD_Width;                /*!< Screen  Width */
/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/** @defgroup graphicObject_Exported_Macros
  * @{
  */
#define RADIO_BUTTON_ADD(RadioButtonGrpName, Label, Function) \
 RadioButtonGrpName->AddRadioOption( RadioButtonGrpName, Label, Function );

/**
  * @}
  */
  
/* Exported functions ------------------------------------------------------- */
/** @defgroup graphicObject_Exported_Functions
  * @{
  */
/*----- High layer function -----*/
                                    
GL_PageControls_TypeDef* NewButton (uint16_t ID, const uint8_t* label,
                                    void (*pEventHandler)(void));
                                    
GL_PageControls_TypeDef* NewCheckbox (uint16_t ID, const uint8_t* label,
                                       void (*pEventHandler)(void));
                                       
GL_PageControls_TypeDef* NewSwitch (uint16_t ID, const uint8_t* label_1, const uint8_t* label_2,
                                    void (*pEventHandler)(void));

/* Label handler **************************************************************/ 
GL_PageControls_TypeDef* NewLabel (uint16_t ID, const uint8_t* label, GL_Direction direction,
                                    __IO uint8_t FontSize, __IO uint16_t Colour);

GL_ErrStatus Set_Label ( GL_Page_TypeDef* pPage, uint16_t ID, const uint8_t* label );

GL_ErrStatus Get_Label ( GL_Page_TypeDef* pPage, uint16_t ID, uint8_t* label );

/* Slidebar handler ***********************************************************/                                   
GL_PageControls_TypeDef* NewSlidebar (uint16_t ID, const uint8_t* label,
                                      GL_Direction direction, void (*pEventHandler)(void));
                                      
uint8_t Get_SlidebarValue ( GL_Page_TypeDef* pPage, uint16_t ID );  
                                    
/* Icon handler ***************************************************************/                                     
GL_PageControls_TypeDef* NewIcon (uint16_t ID, const uint8_t* Image_PTR,
                                  uint16_t Width, uint8_t Height, 
                                  void (*pEventHandler)(void));
                                  
GL_ErrStatus SetIconImage(GL_Page_TypeDef* pPage, uint16_t ID,
                          const uint8_t* pImage, uint16_t Width, uint8_t Height);

/* Histogram handler **********************************************************/                                       
GL_PageControls_TypeDef* NewHistogram (uint16_t ID, const uint8_t* labelX,
                                       const uint8_t* labelY, int16_t data_points[],
                                       uint8_t n_points);

GL_ErrStatus SetHistogramPoints( GL_Page_TypeDef* pPage, uint16_t ID,
                                 int16_t data_points[], uint8_t n_points );

/* Graph handler **************************************************************/                                        
GL_PageControls_TypeDef* NewGraphChart (uint16_t ID, const uint8_t* labelX,
                                        const uint8_t* labelY, int16_t data_points[],
                                        uint8_t n_points, GL_bool Background );

GL_ErrStatus SetGraphChartPoints( GL_Page_TypeDef* pPage, uint16_t ID,
                                  int16_t data_points[], uint8_t n_points );
/* Radio Button handler *******************************************************/ 
GL_RadioButtonGrp_TypeDef* NewRadioButtonGrp (uint16_t ID);

GL_PageControls_TypeDef* AddRadioOption (GL_RadioButtonGrp_TypeDef* pThis, const uint8_t* label, void (*pEventHandler)(void));

/* ComboBox handler ***********************************************************/ 
GL_PageControls_TypeDef* NewComboBoxGrp (uint16_t ID);
GL_ErrStatus AddComboOption (GL_ComboBoxGrp_TypeDef* pThis, const uint8_t* label, void (*pEventHandler)(void));
uint8_t GetComboOptionActive(GL_Page_TypeDef* pPage, uint16_t ID);
GL_ErrStatus ResetComboOptionActive(GL_Page_TypeDef* pPage, uint16_t ID);
const uint8_t* GetComboOptionLabel(GL_Page_TypeDef* pPage, uint16_t ID);
GL_ErrStatus SetComboOptionLabel(GL_Page_TypeDef* pPage, uint16_t ID, const uint8_t* label);

/* Graphic Page handler *******************************************************/ 
GL_ErrStatus Create_PageObj (GL_Page_TypeDef* pThis);

GL_ErrStatus AddPageControlObj (uint16_t PosX, uint16_t PosY,
                                GL_PageControls_TypeDef* objPTR,
                                GL_Page_TypeDef* pagePTR);
                                
GL_ErrStatus DestroyPageControl ( GL_Page_TypeDef* pPage, uint16_t ID );

GL_ErrStatus DestroyPage (GL_Page_TypeDef *pThis);

GL_ErrStatus ShowPage(GL_Page_TypeDef* pThis, GL_bool bVal);

void RefreshPage(GL_Page_TypeDef* pThis);

void ChangePage(GL_Page_TypeDef* pPageOld, GL_Page_TypeDef* pPageNew);

/* Common GUI handler *********************************************************/
void GL_Cross(uint16_t Ypos, uint16_t Xpos); 

void GL_DrawButtonBMP(uint16_t maxX, uint16_t minX, uint16_t maxY,
                      uint16_t minY, uint8_t* ptrBitmap);
                      
GL_bool GetObjStatus(GL_Page_TypeDef* pThis, uint16_t ID);

GL_ErrStatus RefreshPageControl( GL_Page_TypeDef* pPage, uint16_t ID);

void Set_LCD_Resolution( uint16_t Lcd_Width, uint16_t Lcd_Height );

void NullFunc(void);

/* Input handler **************************************************************/
uint8_t CompareCoordinates(uint16_t u16_XMax, uint16_t u16_XMin,
                           uint16_t u16_YMax, uint16_t u16_YMin);
                           
uint8_t CompareJoyCoordinates(uint16_t u16_XMax, uint16_t u16_XMin,
                              uint16_t u16_YMax, uint16_t u16_YMin);
                              
void ProcessInputData(void);

/* Delay handler **************************************************************/
void TimeOutCalculate(void);      
void GL_Delay(uint32_t nCount);   
void TimingDelay_Decrement(void);

#ifdef __cplusplus
}
#endif


#endif /*__GRAPHIC_OBJECT_H */
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
