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

#define BUTTON_SLICE_LENGTH             8
#define SLIDEBAR_CURSOR_LENGTH          6
#define SLIDEBAR_CENTRAL_LENGTH         27
#define SLIDEBAR_OFFSET_LENGTH          4
#define SLIDEBAR_PIECE_LENGTH           4
#define SLIDEBAR_HEIGHT                 18
#define BUTTON_HEIGHT                   26
#define BUTTON_PIECE_LENGTH             8
#define RBUTTON_OPT_SIZE                20
#define RADIO_BUTTON_RADIUS             9
#define COMBOBOX_SIZE                   22
#define CHECKBOX_SIZE                   20
#define PAGE_MAX_NUM                    50
#define TIMEOUT                         1000000

/**
  * @}
  */

/** @defgroup graphicObject_Exported_variables
  * @{
  */
extern GL_Page_TypeDef* PagesList[];      /*!< Pointer to page list */
extern __IO uint8_t calibration_done;     /*!< touchscreen calibration status */

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
                                    
GL_PageControls_TypeDef* NewButton (uint16_t ID, const char* label,
                                    void (*pEventHandler)(GL_PageControls_TypeDef* pThis));
                                    
GL_PageControls_TypeDef* NewCheckbox (uint16_t ID, const char* label,
                                       void (*pEventHandler)(void));
                                       
GL_PageControls_TypeDef* NewSwitch (uint16_t ID, const char* label_1, const char* label_2,
                                    void (*pEventHandler)(void));

/* Label handler **************************************************************/ 
GL_PageControls_TypeDef* NewLabel (uint16_t ID, const char* label, GL_Direction direction,
									GL_FontOption FontSize, __IO uint16_t Colour);

GL_ErrStatus Set_Label ( GL_Page_TypeDef* pPage, uint16_t ID, const char* label );

GL_ErrStatus Get_Label ( GL_Page_TypeDef* pPage, uint16_t ID, char* label );

/* Slidebar handler ***********************************************************/                                   
GL_PageControls_TypeDef* NewSlidebar (uint16_t ID, const char* label,
                                      GL_Direction direction, void (*pEventHandler)(void));
                                      
uint8_t Get_SlidebarValue ( GL_Page_TypeDef* pPage, uint16_t ID );  
                                    
/* Icon handler ***************************************************************/                                     
GL_PageControls_TypeDef* NewIcon (uint16_t ID, const uint8_t* Image_PTR,
                                  uint16_t Width, uint8_t Height, 
                                  void (*pEventHandler)(void));
                                  
GL_ErrStatus SetIconImage(GL_Page_TypeDef* pPage, uint16_t ID,
                          const uint8_t* pImage, uint16_t Width, uint8_t Height);

/* Histogram handler **********************************************************/                                       
GL_PageControls_TypeDef* NewHistogram (uint16_t ID, const char* labelX,
                                       const char* labelY, int16_t data_points[],
                                       uint8_t n_points);

GL_ErrStatus SetHistogramPoints( GL_Page_TypeDef* pPage, uint16_t ID,
                                 int16_t data_points[], uint8_t n_points );

/* Graph handler **************************************************************/                                        
GL_PageControls_TypeDef* NewGraphChart (uint16_t ID, const char* labelX,
                                        const char* labelY, int16_t data_points[],
                                        uint8_t n_points, GL_bool Background );

GL_ErrStatus SetGraphChartPoints( GL_Page_TypeDef* pPage, uint16_t ID,
                                  int16_t data_points[], uint8_t n_points );
/* Radio Button handler *******************************************************/ 
GL_RadioButtonGrp_TypeDef* NewRadioButtonGrp (uint16_t ID);

GL_PageControls_TypeDef* AddRadioOption (GL_RadioButtonGrp_TypeDef* pThis, const char* label, void (*pEventHandler)(void));

/* ComboBox handler ***********************************************************/ 
GL_PageControls_TypeDef* NewComboBoxGrp (uint16_t ID);
GL_ErrStatus AddComboOption (GL_ComboBoxGrp_TypeDef* pThis, const char* label, void (*pEventHandler)(void));
uint8_t GetComboOptionActive(GL_Page_TypeDef* pPage, uint16_t ID);
GL_ErrStatus ResetComboOptionActive(GL_Page_TypeDef* pPage, uint16_t ID);
const char* GetComboOptionLabel(GL_Page_TypeDef* pPage, uint16_t ID);
GL_ErrStatus SetComboOptionLabel(GL_Page_TypeDef* pPage, uint16_t ID, const char* label);

/* Custom Widget **************************************************************/
GL_PageControls_TypeDef* NewCustomWidget (
		uint16_t ID,
		uint16_t (*pGetWidth)(GL_PageControls_TypeDef* pThis),
		uint16_t (*pGetHeight)(GL_PageControls_TypeDef* pThis),
		void (*pEventHandler)(GL_PageControls_TypeDef* pThis),
		void (*pDrawHandler)(GL_PageControls_TypeDef* pThis, _Bool force),
		void *pInstanceData
		);

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

void GL_DrawFilledCircle(uint16_t Xpos, uint8_t Ypos, uint16_t Radius, uint16_t Color);

void GL_DrawFilledRectangle(uint16_t maxX, uint16_t minX, uint8_t maxY, uint8_t minY, uint16_t Color);

GL_bool GetObjStatus(GL_Page_TypeDef* pThis, uint16_t ID);

GL_ErrStatus RefreshPageControl( GL_Page_TypeDef* pPage, uint16_t ID);

void NullFunc(void);

/* Input handler **************************************************************/
void ProcessInputData(void);
void UpdateScreenWithChanges(void);

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
