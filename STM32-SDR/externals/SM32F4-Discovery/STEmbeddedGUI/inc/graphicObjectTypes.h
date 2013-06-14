/**
  ******************************************************************************
  * @file    graphicObjectTypes.h
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    11-July-2011
  * @brief   This file contains all the specific Graphics Object types
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
#ifndef __GRAPHIC_OBJECT_TYPES_H
#define __GRAPHIC_OBJECT_TYPES_H

#ifdef __cplusplus
 extern "C" {
#endif
   
/* Includes ------------------------------------------------------------------*/
#include "stm32HAL.h"

/** @addtogroup Embedded_GUI_Library
  * @{
  */

/** @defgroup graphicObjectTypes 
  * @{
  */ 

  /* Exported constants --------------------------------------------------------*/
/** @defgroup graphicObjectTypes_Exported_Defines
  * @{
  */
#define MAX_CTRL_X_PAGE                      30
#define MAX_NAME_LENGTH                      22
#define MAX_LABEL_LENGTH                     46
#define MAX_BUTTON_LABEL_LENGTH              16
#define MAX_CHECKBOX_LABEL_LENGTH            25
#define MAX_RADIO_OPTION_LABEL_LENGTH        12
#define MAX_RADIO_OPTIONS                     3
#define MAX_COMBO_OPTIONS                    10
#define MAX_COMBO_LABEL_LENGTH               16
#define MAX_SWITCH_LABEL_LENGTH              16
#define MAX_SLIDE_LABEL_LENGTH               16
#define MAX_HIST_LABEL_LENGTH                 7
#define MAX_GRAPH_LABEL_LENGTH                7
#define MAX_HIST_POINTS                      50
#define MAX_GRAPH_POINTS                    100
#define HIST_MARGIN_LENGTH                   20
#define GRAPH_MARGIN_LENGTH                  18

/**
  * @}
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup graphicObjectTypes_Exported_Types
  * @{
  */

/** 
  * @brief  GL_FunctionalState enumeration definition  
  */
typedef enum 
{
   GL_DISABLE = 0,
   GL_ENABLE = !GL_DISABLE
} GL_FunctionalState;

/** 
  * @brief  GL_bool enumeration definition  
  */
typedef enum 
{
   GL_FALSE = 0,
   GL_TRUE = !GL_FALSE
} GL_bool;

/** 
  * @brief  GL_ErrStatus enumeration definition  
  */
typedef enum {
    GL_ERROR = 0, 
    GL_OK = !GL_ERROR
}GL_ErrStatus;

/** 
  * @brief  GL_Direction enumeration definition  
  */
typedef enum {
    GL_HORIZONTAL = 0, 
    GL_LEFT_VERTICAL,
    GL_RIGHT_VERTICAL
}GL_Direction;

/** 
  * @brief  GL_ButtonStatus enumeration definition  
  */
typedef enum {
    UNSELECTED = 0, 
    SELECTED = 1, 
    GL_UNKNOWN = 2
}GL_ButtonStatus;

/** 
  * @brief  GL_ObjType enumeration definition  
  */
typedef enum{
    GL_BUTTON = 1,
    GL_RADIO_BUTTON = 2,
    GL_CHECKBOX = 3,
    GL_LABEL = 4,
    GL_SWITCH = 5,
    GL_ICON = 6,
    GL_COMBOBOX = 7,
    GL_SLIDEBAR = 8,
    GL_HISTOGRAM = 9,
    GL_GRAPH_CHART = 10
}GL_ObjType;

/** 
  * @brief  GL_Coordinate struct definition  
  */
typedef struct   
{ uint16_t MaxX;
  uint16_t MinX;
  uint16_t MaxY;
  uint16_t MinY;
}GL_Coordinate_TypeDef;

/* Forward declaration for circular typedefs */
typedef struct GL_PageControlsObj GL_PageControls_TypeDef;

/** 
  * @brief  GL_PageControlsObj struct definition  
  */
struct GL_PageControlsObj
{
  uint16_t               ID;
  void*                  objPTR;
  GL_Coordinate_TypeDef  objCoordinates;
  GL_ObjType             objType;
  GL_ErrStatus           (*SetObjVisible)(GL_PageControls_TypeDef* pThis, GL_Coordinate_TypeDef objCoordinates);
};

/* Forward declaration for circular typedefs */
typedef struct GL_LabelObj GL_Label_TypeDef;

/** 
  * @brief  GL_LabelObj struct definition  
  */
struct GL_LabelObj
{
  uint16_t          ID;
  uint8_t           label[MAX_LABEL_LENGTH];
  __IO uint8_t      FontSize;
  GL_bool           Control_Visible;
  uint16_t          Colour;
  GL_Direction      Direction;
};

/* Forward declaration for circular typedefs */
typedef struct GL_ButtonObj GL_Button_TypeDef;

/** 
  * @brief  GL_ButtonObj struct definition  
  */
struct GL_ButtonObj
{
  uint16_t          ID;
#ifndef USE_2D_OBJECTS
  uint8_t*          ImageClickedPTR;
  uint8_t*          ImageUnClickedPTR;
#endif
  uint8_t           label[MAX_BUTTON_LABEL_LENGTH];
  GL_bool           isObjectTouched;
  GL_bool           Control_Visible;
  void              (*EventHandler)(void);
};

/* Forward declaration for circular typedefs */
typedef struct GL_CheckboxObj GL_Checkbox_TypeDef;

/** 
  * @brief  GL_CheckboxObj struct definition  
  */
struct GL_CheckboxObj
{
  uint16_t          ID;
#ifndef USE_2D_OBJECTS
  uint8_t*          ImageCheckedPTR;
  uint8_t*          ImageUnCheckedPTR;
#endif
  uint8_t           label[MAX_CHECKBOX_LABEL_LENGTH];
  GL_bool           IsChecked;
  GL_bool           Control_Visible;
  void              (*EventHandler)(void);
};

/* Forward declaration for circular typedefs */
typedef struct GL_SwitchObj GL_Switch_TypeDef;

/** 
  * @brief  GL_SwitchObj struct definition  
  */
struct GL_SwitchObj
{
  uint16_t          ID;
#ifndef USE_2D_OBJECTS
  uint8_t*          ImageClickedPTR;
  uint8_t*          ImageUnClickedPTR;
#endif
  uint8_t           label_1[MAX_SWITCH_LABEL_LENGTH];
  uint8_t           label_2[MAX_SWITCH_LABEL_LENGTH];
  GL_bool           isObjectTouched;
  GL_bool           Control_Visible;
  void              (*EventHandler)(void);
};

/* Forward declaration for circular typedefs */
typedef struct GL_RadioButtonOptionObj GL_RadioOption_TypeDef;
/* Forward declaration for circular typedefs */
typedef struct GL_RadioButtonGrp GL_RadioButtonGrp_TypeDef;

/** 
  * @brief  GL_RadioButtonGrp struct definition  
  */
struct GL_RadioButtonGrp
{
  uint16_t                 ID;
  uint16_t                 RadioOptionCount;
  GL_PageControls_TypeDef* RadioOptions[MAX_RADIO_OPTIONS];
  GL_PageControls_TypeDef* (*AddRadioOption)( GL_RadioButtonGrp_TypeDef* pThis, const uint8_t* label, void (*pEventHandler)(void) );
};

/** 
  * @brief  GL_RadioButtonOptionObj struct definition  
  */
struct GL_RadioButtonOptionObj
{
  GL_RadioButtonGrp_TypeDef* RadioButtonGrp;
#ifndef USE_2D_OBJECTS
  uint8_t*                   ImageCheckedPTR;
  uint8_t*                   ImageUnCheckedPTR;
#endif
  uint8_t                    label[MAX_RADIO_OPTION_LABEL_LENGTH];
  GL_bool                    IsChecked;
  GL_bool                    Control_Visible;
  void                       (*EventHandler)(void);
};

/* Forward declaration for circular typedefs */
typedef struct GL_ComboOptionObj GL_ComboOption_TypeDef;
/* Forward declaration for circular typedefs */
typedef struct GL_ComboBoxGrp    GL_ComboBoxGrp_TypeDef;

/** 
  * @brief  GL_ComboBoxGrp struct definition  
  */
struct GL_ComboBoxGrp
{
#ifndef USE_2D_OBJECTS
  uint8_t*                  ImageClickedPTR;
  uint8_t*                  ImageUnClickedPTR;
#endif
  GL_bool                   Secondary;
  GL_PageControls_TypeDef*  PrimaryComboOpt;
  uint16_t                  ID;
  GL_bool                   Control_Visible;
  uint16_t                  ComboOptionCount;
  GL_ComboOption_TypeDef*   ComboOptions[MAX_COMBO_OPTIONS];
  GL_ErrStatus              (*AddComboOption)( GL_ComboBoxGrp_TypeDef* pThis, const uint8_t* label, void (*pEventHandler)(void) );
};

/** 
  * @brief  GL_ComboOptionObj struct definition  
  */
struct GL_ComboOptionObj
{
  __IO uint16_t             BackColor;
  __IO uint16_t             TextColor;
  GL_ComboBoxGrp_TypeDef*   ComboBoxGrp;
  uint8_t                   label[MAX_COMBO_LABEL_LENGTH];
  GL_bool                   IsActive;
  void                      (*EventHandler)(void);
};

/* Forward declaration for circular typedefs */
typedef struct GL_IconObj GL_Icon_TypeDef;

/** 
  * @brief  GL_IconObj struct definition  
  */
struct GL_IconObj
{
  uint16_t             ID;
  const uint8_t*       ImagePTR;
  uint16_t             ImageWidth;
  uint8_t              ImageHeight;
  GL_bool              isObjectTouched;
  GL_bool              Control_Visible;
  void                 (*EventHandler)(void);
};

/* Forward declaration for circular typedefs */
typedef struct GL_SlidebarObj GL_Slidebar_TypeDef;

/** 
  * @brief  GL_SlidebarObj struct definition  
  */
struct GL_SlidebarObj
{
  uint16_t           ID;
#ifndef USE_2D_OBJECTS
  uint8_t*           ImageCursorPTR;
  uint8_t*           ImageBackgroundPTR;
#endif
  uint8_t            label[MAX_SLIDE_LABEL_LENGTH];
  GL_Direction       Direction;
  uint8_t            Width;
  uint8_t            Length;
  int8_t             CurrentValue;
  uint8_t            PrevValue;
  uint8_t            MinValue;
  uint8_t            MaxValue;
  uint16_t*          BehindCursor;
  GL_bool            Control_Visible;
  void               (*EventHandler)(void);
};

/* Forward declaration for circular typedefs */
typedef struct GL_HistogramObj GL_Histogram_TypeDef;

/** 
  * @brief  GL_HistogramObj struct definition  
  */
struct GL_HistogramObj
{
  uint16_t      ID;
  uint8_t       label_X[MAX_HIST_LABEL_LENGTH];
  uint8_t       label_Y[MAX_HIST_LABEL_LENGTH];
  int16_t       points[MAX_HIST_POINTS];
  uint8_t       n_points;
  GL_bool       Control_Visible;
};

/* Forward declaration for circular typedefs */
typedef struct GL_GraphChartObj GL_GraphChart_TypeDef;

/** 
  * @brief  GL_GraphChartObj struct definition  
  */
struct GL_GraphChartObj
{
  uint16_t      ID;
  uint8_t       label_X[MAX_GRAPH_LABEL_LENGTH];
  uint8_t       label_Y[MAX_GRAPH_LABEL_LENGTH];
  int16_t       points[MAX_GRAPH_POINTS];
  uint8_t       n_points;
  GL_bool       Background;
  GL_bool       Control_Visible;
};

/** 
  * @brief  GL_ObjDimensions type definition  
  */
typedef struct   
{ uint16_t Height;
  uint16_t Length;
}GL_ObjDimensions_TypeDef;

/* Forward declaration for circular typedefs */
typedef struct GL_PageObj GL_Page_TypeDef;

/** 
  * @brief  GL_PageObj struct definition  
  */
struct GL_PageObj
{
  uint8_t                  objName[MAX_NAME_LENGTH];
  GL_bool                  Page_Active;
  GL_bool                  Page_Visible;
  uint16_t                 ControlCount;
  GL_ErrStatus             (*ShowPage)(GL_Page_TypeDef* pThis, GL_bool bVal);
  GL_PageControls_TypeDef* PageControls[MAX_CTRL_X_PAGE]; 
  GL_ErrStatus             (*SetPage)(GL_Page_TypeDef* pThis, GL_bool bVal);
  GL_bool                  (*GetObjStatus)(GL_Page_TypeDef* pThis, uint16_t ID);
  GL_Coordinate_TypeDef    (*GetObjCoordinates)(GL_Page_TypeDef* pThis, uint16_t ID);
};

/**
  * @}
  */
   
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */ 

#ifdef __cplusplus
}
#endif

#endif /*__GRAPHIC_OBJECT_TYPES_H */

/**
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
