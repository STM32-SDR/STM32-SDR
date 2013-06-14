/**
  ******************************************************************************
  * @file    graphicObject.c
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    11-July-2011
  * @brief   This file contains the methods to create the objects that
  *          can be printed on the LCD.
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
#include "graphicObject.h"
#include "images.h"
#include "touchscreen.h"
#include "TscHal.h"
#include "JoyHal.h"
#include "LcdHal.h"
#include "cursor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** @addtogroup Embedded_GUI_Library
  * @{
  */

/** @defgroup graphicObject 
  * @brief graphicObject functions
  * @{
  */ 

/* External variables --------------------------------------------------------*/
extern __IO uint16_t GL_TextColor;
extern __IO uint32_t u32_TSXCoordinate;
extern __IO uint32_t u32_TSYCoordinate;

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/** @defgroup graphicObject_Private_Defines
  * @{
  */
#define FONT_LENGTH                     8
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
/* Private macros ------------------------------------------------------------*/
/** @defgroup graphicObject_Private_Macros
  * @{
  */
#define min(x,y)                    ((x<y)? x:y)
#define p_strncpy(oBuf, iBuf, Len)  strncpy((char*)oBuf, (char*)iBuf, (int)Len)
#define p_strlen(iBuf)              strlen((char*)iBuf)
#define p_strcmp(str1,str2)         strcmp((char*)str1,(char*)str2)
#define p_strcpy(str1,str2)         strcpy((char*)str1,(char*)str2)
/**
  * @}
  */
  
/* Private variables ---------------------------------------------------------*/
/** @defgroup graphicObject_Private_Variables
  * @{
  */    
GL_Page_TypeDef* PagesList[PAGE_MAX_NUM];

/*Header and bitmap image of cursor's symbol*/
static uint32_t SlidebarCursorPointer[] =
  {
    0x14, /*Height of cursor symbol*/
    0x06, /*Width of cursor symbol*/
    0x0D, /*Count of pixels of cursor symbol*/
    0x80001, 0x80001, 0x80001, 0x80001, 0x80001, 0x80001, 0x80001
  };

static uint16_t PageCount = 0;
static __IO uint32_t TimingDelay;

__IO uint32_t vu32_gTimeOutCount = 0;
__IO uint8_t vu8_gTouchEnable = 1;
__IO uint8_t vu8_gSleepState = 0;
uint8_t ValidTouchDetected = 0;
/* Set to 0 to disable The Automatic Backlight Switch Off */
__IO uint8_t vu8_gPowerSaveOption = 1;

/* touch_done global variable is used to handle the Touch event/interrupt.
   touch_done must be defined as external variable in main application using this library */
__IO uint8_t touch_done = 0;
__IO uint8_t joy_done = 0;
__IO uint8_t calibration_done = 0;

/**
  * @}
  */
/* Private function prototypes -----------------------------------------------*/
/** @defgroup graphicObject_Private_FunctionPrototypes
  * @{
  */
static void GL_SetStringFieldValue(uint8_t * dBuf, uint8_t * sBuf, uint32_t MaxLength) ;

static void CallPreEvents(GL_PageControls_TypeDef* pControl);
static void CallEvent(GL_PageControls_TypeDef* pControl);

static GL_ErrStatus Create_Label (GL_Label_TypeDef* );
static GL_ErrStatus Create_Button (GL_Button_TypeDef* );
static GL_ErrStatus Create_RadioButtonGrp (GL_RadioButtonGrp_TypeDef* );
static GL_ErrStatus Create_RadioButtonOption (GL_RadioOption_TypeDef* );
static GL_ErrStatus Create_ComboBoxGrp (GL_ComboBoxGrp_TypeDef* );
static GL_ErrStatus Create_ComboBoxOption (GL_ComboOption_TypeDef* );
static GL_ErrStatus Create_Checkbox (GL_Checkbox_TypeDef* );
static GL_ErrStatus Create_Switch (GL_Switch_TypeDef* );
static GL_ErrStatus Create_Icon (GL_Icon_TypeDef* );
static GL_ErrStatus Create_Slidebar (GL_Slidebar_TypeDef* );
static GL_ErrStatus Create_Histogram (GL_Histogram_TypeDef* );
static GL_ErrStatus Create_GraphChart (GL_GraphChart_TypeDef* );

static GL_ErrStatus SetLabelVisible(GL_PageControls_TypeDef* , GL_Coordinate_TypeDef );
static GL_ErrStatus SetCheckboxVisible(GL_PageControls_TypeDef* , GL_Coordinate_TypeDef );
static GL_ErrStatus SetSwitchVisible(GL_PageControls_TypeDef* , GL_Coordinate_TypeDef );
static GL_ErrStatus SetButtonVisible(GL_PageControls_TypeDef* , GL_Coordinate_TypeDef );
static GL_ErrStatus SetRadioButtonVisible(GL_PageControls_TypeDef* , GL_Coordinate_TypeDef );
static GL_ErrStatus SetComboBoxVisible(GL_PageControls_TypeDef* , GL_Coordinate_TypeDef );
static GL_ErrStatus SetIconVisible(GL_PageControls_TypeDef* , GL_Coordinate_TypeDef );
static GL_ErrStatus SetSlidebarVisible(GL_PageControls_TypeDef* , GL_Coordinate_TypeDef );
static GL_ErrStatus SetHistogramVisible( GL_PageControls_TypeDef* , GL_Coordinate_TypeDef );
static GL_ErrStatus SetGraphChartVisible( GL_PageControls_TypeDef* , GL_Coordinate_TypeDef );

static void SlidebarCursorPreDraw(GL_PageControls_TypeDef* pControl, GL_bool);
static GL_ObjDimensions_TypeDef GetObjSize(GL_PageControls_TypeDef* pPageControl);
static GL_Coordinate_TypeDef GetObjCoordinates(GL_Page_TypeDef* pPage, uint16_t ID);
static GL_ErrStatus SetPage(GL_Page_TypeDef* pThis, GL_bool bVal);
static void GL_DrawRectangle(uint16_t maxX, uint16_t minX, uint8_t maxY, uint8_t minY);

/**
  * @}
  */

/* Private functions ---------------------------------------------------------*/
/** @defgroup graphicObject_Private_Functions
  * @{
  */

/**
  * @brief  Displays a Rectangle.
  * @param  maxX - Maximum X coordinate
  * @param  minX - Minimum X coordinate
  * @param  maxY - Maximum Y coordinate
  * @param  minY - Minimum Y coordinate
  * @retval None
  */
static void GL_DrawRectangle(uint16_t maxX, uint16_t minX, uint8_t maxY, uint8_t minY)
{
  GL_DrawLine(minY, maxX, maxX - minX, GL_Horizontal);
  GL_DrawLine((minY + maxY - minY), maxX, maxX - minX, GL_Horizontal);

  GL_DrawLine(minY, maxX, maxY - minY, GL_Vertical);
  GL_DrawLine(minY, (maxX - (maxX - minX) + 1), maxY - minY, GL_Vertical);
}

/**
  * @brief  Displays a filled Rectangle.
  * @param  maxX: Maximum X coordinate
  * @param  minX: Minimum X coordinate
  * @param  maxY: Maximum Y coordinate
  * @param  minY: Minimum Y coordinate
  * @param  Color: The filling color
  * @retval None
  */
static void GL_DrawFilledRectangle(uint16_t maxX, uint16_t minX, uint8_t maxY, uint8_t minY, uint16_t Color)
{
  uint32_t counter = 0;

  if ( (maxX > minX + 1) && (maxY > minY - 1) )
  {
    GL_DrawLine(minY, maxX, maxX - minX, GL_Horizontal);
    GL_DrawLine((minY + maxY - minY), maxX, maxX - minX, GL_Horizontal);

    GL_DrawLine(minY, maxX, maxY - minY, GL_Vertical);
    GL_DrawLine(minY, (maxX - (maxX - minX) + 1), maxY - minY, GL_Vertical);
  }

  GL_SetTextColor(Color);
  for (counter = 1; counter < (maxY - minY); counter++)
  {
    GL_DrawLine(minY + counter, maxX - 1, maxX - minX - 2, GL_Horizontal);
  }
}

/**
  * @brief  Displays a circle.
  * @param  Xpos: specifies the X position.
  * @param  Ypos: specifies the Y position.
  * @param  Radius: specifies the circle radius.
  * @param  Color: specifies the filling color.
  * @retval None
  */
void GL_DrawFilledCircle(uint16_t Xpos, uint8_t Ypos, uint16_t Radius, uint16_t Color)
{
  uint32_t n = 2;
  GL_LCD_DrawCircle( Ypos - Radius, Xpos - Radius, Radius);
  GL_LCD_DrawCircle( Ypos - Radius, Xpos - Radius, Radius - 1);
  GL_SetTextColor(Color);
  for (; n < Radius; n++)
  {
    GL_LCD_DrawCircle( Ypos - Radius, Xpos - Radius, Radius - n);
  }
}

/**
  * @brief  Displays a line using Bresenham line algorithm.
  * @param  Xpos1: specifies the first point X position
  * @param  Ypos1: specifies the first point Y position
  * @param  Xpos2: specifies the second point X position
  * @param  Ypos2: specifies the second point Y position
  * @param  Color: RGB color of line.
  * @retval None
  */
static void GL_DrawObliqueLine(uint16_t Ypos1, uint16_t Xpos1, uint16_t Ypos2, uint16_t Xpos2, uint16_t Color)
{
  uint8_t steep = 0; /* Steepness of line,0 - non-steep (angle 45º-),1 - steep (45º+) */
  uint16_t TempPos = 0;
  uint16_t x = 0, y = 0;
  uint16_t deltax = 0, deltay = 0;
  int16_t errorX = 0;
  int8_t ystep = 0;

  /*Case the both points are identical*/
  if ((Xpos1 == Xpos2) && (Ypos1 == Ypos2))
  {
    LCD_PutPixel(Xpos1, Ypos1, Color, SinglePixel);
  }
  /*Case the line are pure horizontal*/
  else if (Ypos1 == Ypos2)
  {
    /*Swap X positions if it is necessary*/
    if (Xpos1 > Xpos2)
    {
      TempPos = Xpos2;
      Xpos2 = Xpos1;
      Xpos1 = TempPos;
    }
    /*Draw first-opening pixel.*/
    LCD_PutPixel(Xpos1, Ypos1, Color, FirstPixel);

    /*Draw middle pixels*/
    for (x = Xpos1 + 1;x < Xpos2;x++)
    {
      LCD_PutPixel(Xpos1, Ypos1, Color, MiddlePixel);
    }

    /*Draw last-ending pixel*/
    LCD_PutPixel(Xpos2, Ypos1, Color, LastPixel);
  }
  /*Case the line are pure vertical*/
  else if (Xpos1 == Xpos2)
  {
    /*Swap Y positions if it is necessary*/
    if (Ypos1 > Ypos2)
    {
      TempPos = Ypos2;
      Ypos2 = Ypos1;
      Ypos1 = TempPos;
    }

    /*Draw line as rectangle of 1 pixel width*/
    GL_DrawFilledRectangle( Ypos2 - 1, Ypos1 - 1, Xpos2 - 1, Xpos1 - 2, Color);
  }
  /*The general line*/
  else
  {
    /*Calculate steepness. If elevation angle is smaller than 45º steep is reset to 0*/
    steep = ((Ypos1 > Ypos2) ? (Ypos1 - Ypos2) : (Ypos2 - Ypos1)) > ((Xpos1 > Xpos2) ? (Xpos1 - Xpos2) : (Xpos2 - Xpos1));

    /*Coordinators X and Y are swapped with each other in case of steep = 1*/
    if (steep != 0x00)
    {
      TempPos = Ypos1;
      Ypos1 = Xpos1;
      Xpos1 = TempPos;

      TempPos = Ypos2;
      Ypos2 = Xpos2;
      Xpos2 = TempPos;
    }
    /*Coordinators X and Y are swapped separetely if it is necessary.*/
    if (Xpos1 > Xpos2)
    {
      TempPos = Ypos1;
      Ypos1 = Ypos2;
      Ypos2 = TempPos;

      TempPos = Xpos2;
      Xpos2 = Xpos1;
      Xpos1 = TempPos;
    }

    /*Bresenham line algorithm */
    deltax = Xpos2 - Xpos1;
    deltay = ((Ypos1 > Ypos2) ? (Ypos1 - Ypos2) : (Ypos2 - Ypos1));
    errorX = -(deltax + 1) / 2;
    y = Ypos1;
    if (Ypos1 < Ypos2)
    {
      ystep = 1;
    }
    else
    {
      ystep = -1;
    }

    for (x = Xpos1; x <= Xpos2 ; x++)
    {
      if (steep != 0x00)
      {
        {
          LCD_PutPixel(y, x, Color, SinglePixel);
        }
      }
      else
      {
        {
          LCD_PutPixel(x, y, Color, SinglePixel);
        }
      }

      errorX = errorX + deltay;
      if (errorX >= 0)
      {
        y = y + ystep;
        errorX = errorX - deltax;
      }
    }
  }
}



/**
  * @brief  Displays a Cross.
  * @param  Xpos: specifies the left X position.
  * @param  Ypos: specifies the botton Y position.
  * @retval None
  */
void GL_Cross(uint16_t Ypos, uint16_t Xpos)
{
  GL_DrawLine(Ypos, Xpos + 30, 60, GL_Horizontal); /* Horizontal Line */
  GL_DrawLine(Ypos - 30, Xpos, 60, GL_Vertical);   /* Vertical Line   */
}


/**
  * @brief  Displays a button.
  * @param  maxX: Maximum X coordinate
  * @param  minX: Minimum X coordinate
  * @param  maxY: Maximum Y coordinate
  * @param  minY: Minimum Y coordinate
  * @param  ptrBitmap: pointer to the image
  * @retval None
  */
void GL_DrawButtonBMP(uint16_t maxX, uint16_t minX, uint16_t maxY, uint16_t minY, uint8_t* ptrBitmap)
{
  GL_SetDisplayWindow(maxY, maxX, maxY - minY, maxX - minX);
  GL_DrawBMP(ptrBitmap);
  GL_SetDisplayWindow(LCD_Height - 1, LCD_Width - 1, LCD_Height, LCD_Width);
}

/**
  * @brief  assignes to string fields their values
  * @param  dBuf: destination buffer (string field)
  * @param  sBuf: source buffer (string field  value)
  * @param  MaxLength: The size of the string field
  * @retval None
  */
static void GL_SetStringFieldValue(uint8_t * dBuf, uint8_t * sBuf, uint32_t MaxLength)
{
  uint32_t tmp_length = 0;

  tmp_length = min(p_strlen(sBuf), MaxLength - 1);
  if (tmp_length > 0)
  {
    p_strncpy(dBuf, sBuf, tmp_length);
    dBuf[tmp_length] = 0;
  }
  else
  {
    dBuf[0] = 0;
  }
}

/**
  * @brief  Create and initialize a Label object
  * @param  oName: Object Name
  * @param  label: Text on a Label Object
  * @param  FontSize: The size of the Font.
  *         This parameter can be one of the following values:
  *     @arg  FONT_BIG
  *     @arg  FONT_SMALL
  * @retval pPageControlObj - The pointer to the graphic object
  */
GL_PageControls_TypeDef* NewLabel (uint16_t ID, const uint8_t* label, GL_Direction direction, __IO uint8_t FontSize, __IO uint16_t Colour)
{
  GL_Label_TypeDef *pControlObj = NULL;
  GL_PageControls_TypeDef * pPageControlObj = NULL;

  pControlObj = (GL_Label_TypeDef *)malloc(sizeof(GL_Label_TypeDef));
  if (pControlObj)
  {
    pControlObj->ID = ID;
    GL_SetStringFieldValue(pControlObj->label, (uint8_t *)label, MAX_LABEL_LENGTH);

    pControlObj->FontSize = FontSize;
    pControlObj->Colour = Colour;
    pControlObj->Direction = direction;

    /* Create the Label object */
    Create_Label(pControlObj);

    pPageControlObj = (GL_PageControls_TypeDef*)malloc(sizeof(GL_PageControls_TypeDef));
    if ( pPageControlObj )
    {
      pPageControlObj->objPTR = (void*)pControlObj;
      pPageControlObj->objType = GL_LABEL;
    }
    else
    {
      free(pControlObj);
      pControlObj = NULL;
    }
  }
  return pPageControlObj;
}

/**
  * @brief  Create and initialize a Button object
  * @param  oName: Object Name
  * @param  label: Label on button
  * @param  pEventHandler: pointer to function associated to its touch event
  * @retval GL_PageControls_TypeDef* - The created Object pointer
  */
GL_PageControls_TypeDef* NewButton (uint16_t ID, const uint8_t* label, void (*pEventHandler)(void))
{
  GL_PageControls_TypeDef *pPageControlObj = NULL;
  GL_Button_TypeDef *pControlObj = NULL;

  pControlObj = (GL_Button_TypeDef *)malloc(sizeof(GL_Button_TypeDef));

  if (pControlObj)
  {
    pControlObj->ID = ID;
#ifndef USE_2D_OBJECTS
    pControlObj->ImageUnClickedPTR  = (uint8_t*)BtnNormal;
    pControlObj->ImageClickedPTR    = (uint8_t*)BtnPressed;
#endif

    GL_SetStringFieldValue(pControlObj->label, (uint8_t *)label, MAX_BUTTON_LABEL_LENGTH);
    pControlObj->EventHandler = pEventHandler;
    /* Create the Button object */
    Create_Button(pControlObj);

    pPageControlObj = (GL_PageControls_TypeDef*)malloc(sizeof(GL_PageControls_TypeDef));
    if ( pPageControlObj )
    {
      pPageControlObj->objPTR = (void*)pControlObj;
      pPageControlObj->objType = GL_BUTTON;
    }
    else
    {
      free(pControlObj);
      pControlObj = NULL;
    }
  }
  return pPageControlObj;
}

/**
  * @brief  Create and initialize a Checkbox object
  * @param  oName: Object Name
  * @param  label: Label for the Checkbox
  * @param  pEventHandler: pointer to function associated to its touch event
  * @retval GL_PageControls_TypeDef* - The created Object pointer
  */
GL_PageControls_TypeDef* NewCheckbox (uint16_t ID, const uint8_t* label, void (*pEventHandler)(void))
{
  GL_Checkbox_TypeDef *pControlObj = NULL;
  GL_PageControls_TypeDef* pPageControlObj = NULL;

  pControlObj = (GL_Checkbox_TypeDef *)malloc(sizeof(GL_Checkbox_TypeDef));
  if (pControlObj)
  {
    pControlObj->ID = ID;
    GL_SetStringFieldValue(pControlObj->label, (uint8_t *)label, MAX_CHECKBOX_LABEL_LENGTH);

#ifndef USE_2D_OBJECTS
    pControlObj->ImageUnCheckedPTR  = (uint8_t*)CheckBox1;
    pControlObj->ImageCheckedPTR  = (uint8_t*)CheckBox2;
#endif

    pControlObj->EventHandler = pEventHandler;
    /* Create the Checkbox object */
    Create_Checkbox(pControlObj);

    pPageControlObj = (GL_PageControls_TypeDef*)malloc(sizeof(GL_PageControls_TypeDef));
    if ( pPageControlObj )
    {
      pPageControlObj->objPTR = (void*)pControlObj;
      pPageControlObj->objType = GL_CHECKBOX;
    }
    else
    {
      free(pControlObj);
      pControlObj = NULL;
    }
  }
  return pPageControlObj;
}

/**
  * @brief  Create and initialize a RadioButton Group object
  * @param  oName: Object Name
  * @retval GL_RadioButtonGrp_TypeDef* - The created Object pointer
  */
GL_RadioButtonGrp_TypeDef* NewRadioButtonGrp (uint16_t ID)
{
  GL_RadioButtonGrp_TypeDef *pRbuttonGrp = NULL;

  pRbuttonGrp = (GL_RadioButtonGrp_TypeDef *)malloc(sizeof(GL_RadioButtonGrp_TypeDef));

  if (pRbuttonGrp)
  {
    pRbuttonGrp->ID = ID;
    /* Create the RadioButton object */
    Create_RadioButtonGrp(pRbuttonGrp);
  }
  return pRbuttonGrp;
}

/**
  * @brief  Create and initialize a RadioButton Option object
  * @param  GL_RadioButtonGrp_TypeDef* pThis: RadioButton Group object
  * @param  label: Label for the RadioButton Option
  * @param  pEventHandler: pointer to function associated to its touch event
  * @retval GL_PageControls_TypeDef* - The created Object pointer
  */
GL_PageControls_TypeDef* AddRadioOption (GL_RadioButtonGrp_TypeDef* pThis, const uint8_t* label, void (*pEventHandler)(void))
{
  GL_RadioOption_TypeDef *pOptionObj = NULL;
  GL_PageControls_TypeDef* pPageControlObj = NULL;

  if (!pThis || pThis->RadioOptionCount == 3)
  {
    return GL_ERROR;
  }

  pOptionObj = (GL_RadioOption_TypeDef *)malloc(sizeof(GL_RadioOption_TypeDef));

  if (pOptionObj)
  {
    pOptionObj->RadioButtonGrp = pThis;

    GL_SetStringFieldValue(pOptionObj->label, (uint8_t *)label, MAX_RADIO_OPTION_LABEL_LENGTH);

    pOptionObj->EventHandler = pEventHandler;

    pPageControlObj = (GL_PageControls_TypeDef*)malloc(sizeof(GL_PageControls_TypeDef));
    if ( pPageControlObj )
    {
      pPageControlObj->objPTR = (void*)pOptionObj;
      pPageControlObj->objType = GL_RADIO_BUTTON;

      /* Create the RadioButton Option object */
      Create_RadioButtonOption(pOptionObj);

      pThis->RadioOptions[pThis->RadioOptionCount] = pPageControlObj;
      pThis->RadioOptionCount++;
      if (pThis->RadioOptionCount == 1)
      {
        ((GL_RadioOption_TypeDef*)(pThis->RadioOptions[0]->objPTR))->IsChecked = GL_TRUE;
      }

    }
    else
    {
      free(pOptionObj);
      pOptionObj = NULL;
    }
  }
  return pPageControlObj;
}

/**
  * @brief  Create and initialize a ComboBox Group object
  * @param  oName: Object Name
  * @retval GL_PageControls_TypeDef* - The created Object pointer
  */
GL_PageControls_TypeDef* NewComboBoxGrp (uint16_t ID)
{
  GL_ComboBoxGrp_TypeDef *pComboBoxGrp = NULL;
  GL_PageControls_TypeDef*  pPageControlObj = NULL;

  pComboBoxGrp = (GL_ComboBoxGrp_TypeDef *)malloc(sizeof(GL_ComboBoxGrp_TypeDef));

  if (pComboBoxGrp)
  {
    pComboBoxGrp->ID = ID;
    /* Create the ComboBox object */
    Create_ComboBoxGrp(pComboBoxGrp);

    pPageControlObj = (GL_PageControls_TypeDef*)malloc(sizeof(GL_PageControls_TypeDef));
    if ( pPageControlObj )
    {
      pPageControlObj->objPTR = (void*)pComboBoxGrp;
      pPageControlObj->objType = GL_COMBOBOX;
    }
    else
    {
      free(pComboBoxGrp);
      pComboBoxGrp = NULL;
    }
  }

  return pPageControlObj;
}

/**
  * @brief  Create and initialize a ComboBox Option object
  * @param  GL_ComboBoxGrp_TypeDef* pThis: ComboBox Group object
  * @param  label: Label for the ComboOption
  * @param  pEventHandler: pointer to function associated to its touch event
  * @retval GL_OK if successful, GL_ERROR otherwise
  */
GL_ErrStatus AddComboOption (GL_ComboBoxGrp_TypeDef* pThis, const uint8_t* label, void (*pEventHandler)(void))
{
  GL_ComboOption_TypeDef *pOptionObj = NULL;

  if (!pThis || ( pThis->ComboOptionCount == MAX_COMBO_OPTIONS))
  {
    return GL_ERROR;
  }

  pOptionObj = (GL_ComboOption_TypeDef *)malloc(sizeof(GL_ComboOption_TypeDef));

  if (pOptionObj)
  {
    pOptionObj->ComboBoxGrp = pThis;

    GL_SetStringFieldValue(pOptionObj->label, (uint8_t *)label, MAX_COMBO_LABEL_LENGTH);

    pOptionObj->EventHandler = pEventHandler;

    /* Create the ComboBox Option object */
    Create_ComboBoxOption(pOptionObj);

    pThis->ComboOptions[pThis->ComboOptionCount] = pOptionObj;
    pThis->ComboOptionCount++;

    if (pThis->ComboOptionCount == 1)
    {
      ((GL_ComboOption_TypeDef*)(pThis->ComboOptions[0]))->IsActive = GL_TRUE;
    }
  }

  return GL_OK;
}

/**
  * @brief  Create and initialize a Switch Button object
  * @param  oName: Object Name
  * @param  label1: Label on unclicked Switch button
  * @param  label2: Label on clicked Switch button
  * @param  pEventHandler: pointer to function associated to its touch event
  * @retval GL_PageControls_TypeDef* - The created Object pointer
  */
GL_PageControls_TypeDef* NewSwitch (uint16_t ID, const uint8_t* label_1, const uint8_t* label_2, void (*pEventHandler)(void))
{
  GL_Switch_TypeDef *pControlObj = NULL;
  GL_PageControls_TypeDef* pPageControlObj = NULL;

  pControlObj = (GL_Switch_TypeDef *)malloc(sizeof(GL_Switch_TypeDef));

  if (pControlObj)
  {
    pControlObj->ID = ID;

#ifndef USE_2D_OBJECTS
    pControlObj->ImageUnClickedPTR = (uint8_t*)SwitchNormal;
    pControlObj->ImageClickedPTR   = (uint8_t*)SwitchPressed;
#endif
    GL_SetStringFieldValue(pControlObj->label_1, (uint8_t *)label_1, MAX_SWITCH_LABEL_LENGTH);
    GL_SetStringFieldValue(pControlObj->label_2, (uint8_t *)label_2, MAX_SWITCH_LABEL_LENGTH);

    pControlObj->EventHandler = pEventHandler;
    /* Create the Checkbox object */
    Create_Switch(pControlObj);

    pPageControlObj = (GL_PageControls_TypeDef*)malloc(sizeof(GL_PageControls_TypeDef));
    if ( pPageControlObj )
    {
      pPageControlObj->objPTR = (void*)pControlObj;
      pPageControlObj->objType = GL_SWITCH;
    }
    else
    {
      free(pControlObj);
      pControlObj = NULL;
    }
  }
  return pPageControlObj;
}

/**
  * @brief  Create and initialize a Icon object
  * @param  oName: Object Name
  * @param  Image_PTR: Icon image pointer
  * @param  Width: Image Width
  * @param  Height: Image Height
  * @param  pEventHandler: pointer to function associated to its touch event
  * @retval GL_PageControls_TypeDef* - The created Object pointer
  */
GL_PageControls_TypeDef* NewIcon (uint16_t ID, const uint8_t* Image_PTR, uint16_t Width, uint8_t Height, void (*pEventHandler)(void))
{
  GL_Icon_TypeDef *pControlObj = NULL;
  GL_PageControls_TypeDef* pPageControlObj = NULL;

  pControlObj = (GL_Icon_TypeDef *)malloc(sizeof(GL_Icon_TypeDef));
  if (pControlObj)
  {
    pControlObj->ID = ID;
    pControlObj->ImagePTR = (uint8_t*)Image_PTR;
    pControlObj->ImageWidth = Width;
    pControlObj->ImageHeight = Height;
    pControlObj->EventHandler = pEventHandler;

    /* Create the Icon object */
    Create_Icon(pControlObj);

    pPageControlObj = (GL_PageControls_TypeDef*)malloc(sizeof(GL_PageControls_TypeDef));
    if ( pPageControlObj )
    {
      pPageControlObj->objPTR = (void*)pControlObj;
      pPageControlObj->objType = GL_ICON;
    }
    else
    {
      free(pControlObj);
      pControlObj = NULL;
    }
  }
  return pPageControlObj;
}

/**
  * @brief  Create and initialize a Slidebar object
  * @param  oName: Object Name
  * @param  label: Label for Slidebar meaning
  * @param  direction: The printing orientation.
  *         This parameter can be one of the following values:
  *     @arg  GL_Vertical
  *     @arg  GL_Horizontal
  * @param  pEventHandler: pointer to function associated to its touch event
  * @retval GL_PageControls_TypeDef* - The created Object pointer
  */
GL_PageControls_TypeDef* NewSlidebar (uint16_t ID, const uint8_t* label, GL_Direction direction, void (*pEventHandler)(void))
{
  GL_Slidebar_TypeDef *pControlObj = NULL;
  GL_PageControls_TypeDef* pPageControlObj = NULL;

  pControlObj = (GL_Slidebar_TypeDef *)malloc(sizeof(GL_Slidebar_TypeDef));

  if (pControlObj)
  {
    pControlObj->ID = ID;
    
#ifndef USE_2D_OBJECTS
    pControlObj->ImageCursorPTR      = (uint8_t*)SlidebarCursor;
    pControlObj->ImageBackgroundPTR  = (uint8_t*)SlidebarCentral;
#endif
    GL_SetStringFieldValue(pControlObj->label, (uint8_t *)label, MAX_SLIDE_LABEL_LENGTH);

    pControlObj->Direction = direction;
    pControlObj->Width = 18;
    pControlObj->Length = 118;
    pControlObj->PrevValue = 100;
    pControlObj->CurrentValue = 100;
    pControlObj->MinValue = 0;
    pControlObj->MaxValue = 100;
    pControlObj->EventHandler = pEventHandler;

    /* Create the Slidebar object */
    Create_Slidebar(pControlObj);

    pPageControlObj = (GL_PageControls_TypeDef*)malloc(sizeof(GL_PageControls_TypeDef));
    if ( pPageControlObj )
    {
      pPageControlObj->objPTR = (void*)pControlObj;
      pPageControlObj->objType = GL_SLIDEBAR;
    }
    else
    {
      free(pControlObj);
      pControlObj = NULL;
    }
  }
  return pPageControlObj;
}

/**
  * @brief  Create and initialize a Histogram object
  * @param  oName: Object Name
  * @param  labelX: Label of the X axis
  * @param  labelY: Label of the Y axis
  * @param  data_points[]: The array of points to be plot on the LCD
  * @param  n_points: Number of points to be plot
  * @retval GL_PageControls_TypeDef* - The created Object pointer
  */
GL_PageControls_TypeDef* NewHistogram ( uint16_t ID, const uint8_t* labelX, const uint8_t* labelY, int16_t data_points[], uint8_t n_points )
{
  GL_PageControls_TypeDef *pPageControlObj = NULL;
  GL_Histogram_TypeDef *pControlObj = NULL;

  pControlObj = (GL_Histogram_TypeDef *)malloc(sizeof(GL_Histogram_TypeDef));

  if (pControlObj)
  {
    pControlObj->n_points = 0;
    pControlObj->ID = ID;

    GL_SetStringFieldValue(pControlObj->label_X, (uint8_t *)labelX, MAX_HIST_LABEL_LENGTH);
    GL_SetStringFieldValue(pControlObj->label_Y, (uint8_t *)labelY, MAX_HIST_LABEL_LENGTH);

    /* Determine the length of the array of points and copy the content into the
       internal points array limiting the number of points to the maximum allowed */
    if ( n_points > MAX_HIST_POINTS )
    {
      memcpy(pControlObj->points, data_points, MAX_HIST_POINTS * sizeof(int16_t));
      pControlObj->n_points = MAX_HIST_POINTS;
    }
    else
    {
      memcpy(pControlObj->points, data_points, n_points * sizeof(int16_t));
      pControlObj->n_points =  n_points;
    }

    /* Create the Histogram object */
    Create_Histogram(pControlObj);

    pPageControlObj = (GL_PageControls_TypeDef*)malloc(sizeof(GL_PageControls_TypeDef));
    if ( pPageControlObj )
    {
      pPageControlObj->objPTR = (void*)pControlObj;
      pPageControlObj->objType = GL_HISTOGRAM;
    }
    else
    {
      free(pControlObj);
      pControlObj = NULL;
    }
  }
  return pPageControlObj;
}

/**
  * @brief  Create and initialize a Graphical Chart object
  * @param  oName: Object Name
  * @param  labelX: Label of the X axis
  * @param  labelY: Label of the Y axis
  * @param  data_points[]: The array of points to be plot on the LCD
  * @param  n_points: Number of points to be plot
  * @param  Background: Select if the black background has to be used
  *         This parameter can be one of the following values:
  *     @arg  GL_TRUE
  *     @arg  GL_FALSE
  * @retval GL_PageControls_TypeDef* - The created Object pointer
  */
GL_PageControls_TypeDef* NewGraphChart ( uint16_t ID, const uint8_t* labelX, const uint8_t* labelY, int16_t data_points[], uint8_t n_points, GL_bool Background )
{
  GL_PageControls_TypeDef *pPageControlObj = NULL;
  GL_GraphChart_TypeDef *pControlObj = NULL;

  pControlObj = (GL_GraphChart_TypeDef *)malloc(sizeof(GL_GraphChart_TypeDef));

  if (pControlObj)
  {
    pControlObj->n_points = 0;
    pControlObj->ID = ID;
    GL_SetStringFieldValue(pControlObj->label_X, (uint8_t *)labelX, MAX_GRAPH_LABEL_LENGTH);
    GL_SetStringFieldValue(pControlObj->label_Y, (uint8_t *)labelY, MAX_GRAPH_LABEL_LENGTH);

    /* Determine the length of the array of points and copy the content into the
       internal points array limiting the number of points to the maximum allowed */
    if ( n_points > MAX_GRAPH_POINTS )
    {
      memcpy(pControlObj->points, data_points, MAX_GRAPH_POINTS*2);
      pControlObj->n_points = MAX_GRAPH_POINTS;
    }
    else
    {
      memcpy(pControlObj->points, data_points, n_points*2);
      pControlObj->n_points = n_points;
    }

    pControlObj->Background = Background;

    /* Create the Graphical Chart object */
    Create_GraphChart(pControlObj);

    pPageControlObj = (GL_PageControls_TypeDef*)malloc(sizeof(GL_PageControls_TypeDef));
    if ( pPageControlObj )
    {
      pPageControlObj->objPTR = (void*)pControlObj;
      pPageControlObj->objType = GL_GRAPH_CHART;
    }
    else
    {
      free(pControlObj);
      pControlObj = NULL;
    }
  }
  return pPageControlObj;
}

/**
  * @brief  Add a new Control object to the page
  * @param  PosX: Coordinate for X axis
  * @param  PosY: Coordinate for Y axis
  * @param  *objPTR: Pointer to Object Structure
  * @param  *pagePTR: Pointer to Page Structure
  * @retval GL_ErrStatus - GL_OK if successful, GL_ERROR otherwise
  */
GL_ErrStatus AddPageControlObj (uint16_t PosX, uint16_t PosY, GL_PageControls_TypeDef* objPTR, GL_Page_TypeDef* pagePTR)
{
  if (pagePTR->ControlCount < MAX_CTRL_X_PAGE)
  {
    GL_Coordinate_TypeDef objCoordinates;
    GL_ComboBoxGrp_TypeDef* pTmpComboBoxGrp = NULL;
    GL_ComboBoxGrp_TypeDef* pTmpComboBoxGrp1 = NULL;

    memset(&objCoordinates, 0x00, sizeof(GL_Coordinate_TypeDef));
    if ((PosX > LCD_Width) || (PosY > LCD_Height))
    {
      return GL_ERROR;
    }

    objCoordinates.MaxX = PosX;
    objCoordinates.MinY = PosY;

    if (objPTR->objType == GL_LABEL)
    {
	   GL_Label_TypeDef* pTmp = ((GL_Label_TypeDef*)(objPTR->objPTR));
      objPTR->SetObjVisible = SetLabelVisible;
      objPTR->ID = pTmp->ID;
      if ( pTmp->Direction == GL_LEFT_VERTICAL || pTmp->Direction == GL_RIGHT_VERTICAL )
      {
        objCoordinates.MaxX = PosY;
        objCoordinates.MinY = PosX;
      } 
    }
    else if (objPTR->objType == GL_COMBOBOX)
    {
      GL_PageControls_TypeDef* tmpComboBoxGrpBrother;
      objCoordinates.MinX = PosX - COMBOBOX_SIZE;
      objCoordinates.MaxY = PosY + COMBOBOX_SIZE;
      objPTR->SetObjVisible = SetComboBoxVisible;
      objPTR->objCoordinates = objCoordinates;
      pagePTR->PageControls[pagePTR->ControlCount] = objPTR;
      pagePTR->ControlCount++;
      pTmpComboBoxGrp1 = ((GL_ComboBoxGrp_TypeDef*)(objPTR->objPTR));
      objPTR->ID = pTmpComboBoxGrp1->ID;

      if ( pTmpComboBoxGrp1->Secondary == GL_FALSE)
      {
        tmpComboBoxGrpBrother = NewComboBoxGrp(00);
        pTmpComboBoxGrp = ((GL_ComboBoxGrp_TypeDef*)(tmpComboBoxGrpBrother->objPTR));
        pTmpComboBoxGrp->PrimaryComboOpt = objPTR;
        pTmpComboBoxGrp->Secondary = GL_TRUE;
#ifndef USE_2D_OBJECTS
        pTmpComboBoxGrp->ImageUnClickedPTR = (uint8_t*)ArrowDownUntouched;
        pTmpComboBoxGrp->ImageClickedPTR = (uint8_t*)ArrowDownTouched;
#endif
        AddPageControlObj( (uint16_t)(PosX - COMBOBOX_SIZE),
                           (uint8_t)PosY,
                           tmpComboBoxGrpBrother,
                           pagePTR );
      }
      return GL_OK;
    }
    else
    {
      if (objPTR->objType == GL_BUTTON || objPTR->objType == GL_SWITCH)
      {
        objCoordinates.MinX = PosX - BUTTON_PIECE_LENGTH;
        objCoordinates.MaxY = PosY + BUTTON_HEIGHT;
        if(objPTR->objType == GL_BUTTON)
        {
		  GL_Button_TypeDef* pTmp = ((GL_Button_TypeDef*)(objPTR->objPTR));
          objPTR->SetObjVisible = SetButtonVisible;
          objPTR->ID = pTmp->ID;
        }
        else
        {
		  GL_Switch_TypeDef* pTmp = ((GL_Switch_TypeDef*)(objPTR->objPTR));
          objPTR->SetObjVisible = SetSwitchVisible;
          objPTR->ID = pTmp->ID;
        }
      }
      else if (objPTR->objType == GL_RADIO_BUTTON)
      {
	    GL_RadioOption_TypeDef* pTmp = ((GL_RadioOption_TypeDef*)(objPTR->objPTR));
        objPTR->SetObjVisible = SetRadioButtonVisible;
        objPTR->ID = pTmp->RadioButtonGrp->ID;
        objCoordinates.MinX = PosX - RBUTTON_OPT_SIZE;
        objCoordinates.MaxY = PosY + RBUTTON_OPT_SIZE;
      }
      else if (objPTR->objType == GL_CHECKBOX)
      {
	    GL_Checkbox_TypeDef* pTmp = ((GL_Checkbox_TypeDef*)(objPTR->objPTR));
        objPTR->SetObjVisible = SetCheckboxVisible;
        objPTR->ID = pTmp->ID;
        objCoordinates.MinX = PosX - CHECKBOX_SIZE;
        objCoordinates.MaxY = PosY + CHECKBOX_SIZE;
      }
      else if (objPTR->objType == GL_ICON)
      {
	    GL_Icon_TypeDef* pTmp = ((GL_Icon_TypeDef*)(objPTR->objPTR));
        objPTR->SetObjVisible = SetIconVisible;
        objPTR->ID = pTmp->ID;
        objCoordinates.MinX = PosX - pTmp->ImageWidth;
        objCoordinates.MaxY = PosY + pTmp->ImageHeight;
      }
      else if (objPTR->objType == GL_SLIDEBAR)
      {
	    GL_Slidebar_TypeDef* pTmp = ((GL_Slidebar_TypeDef*)(objPTR->objPTR));
        objPTR->SetObjVisible = SetSlidebarVisible;
        objPTR->ID = pTmp->ID;
        if ( pTmp->Direction == GL_HORIZONTAL)
        {
          objCoordinates.MinX = PosX - SLIDEBAR_PIECE_LENGTH;
          objCoordinates.MaxY = PosY + SLIDEBAR_HEIGHT;
        }
        else if ( pTmp->Direction == GL_LEFT_VERTICAL)
        {
          objCoordinates.MaxX = PosX;
          objCoordinates.MinX = PosX - SLIDEBAR_HEIGHT;
          objCoordinates.MaxY = PosY + (SLIDEBAR_CENTRAL_LENGTH + 2) * SLIDEBAR_PIECE_LENGTH;
          objCoordinates.MinY = PosY;
        }
        else if ( pTmp->Direction == GL_RIGHT_VERTICAL)
        {
          objCoordinates.MaxX = PosX;
          objCoordinates.MinX = PosX - SLIDEBAR_HEIGHT;
          objCoordinates.MaxY = PosY + (SLIDEBAR_CENTRAL_LENGTH + 2) * SLIDEBAR_PIECE_LENGTH + SLIDEBAR_PIECE_LENGTH;
          objCoordinates.MinY = PosY + SLIDEBAR_PIECE_LENGTH;
        }
      }
      else if(objPTR->objType == GL_HISTOGRAM)
      {
	    GL_Histogram_TypeDef* pTmp = ((GL_Histogram_TypeDef*)(objPTR->objPTR));
        objPTR->SetObjVisible = SetHistogramVisible;
        objPTR->ID = pTmp->ID;
      }
      else if(objPTR->objType == GL_GRAPH_CHART)
      {
	    GL_GraphChart_TypeDef* pTmp = ((GL_GraphChart_TypeDef*)(objPTR->objPTR));
        objPTR->SetObjVisible = SetGraphChartVisible;
        objPTR->ID = pTmp->ID;
      }
    }

    objPTR->objCoordinates = objCoordinates;

    pagePTR->PageControls[pagePTR->ControlCount] = objPTR;
    pagePTR->ControlCount++;
    return GL_OK;
  }
  else
    return GL_ERROR;
}

/**
  * @brief  Modify the label in a Label object
  * @param  *pPage: Pointer to Page Structure
  * @param  ID: Object Identifier
  * @param  label: Text on a Label Object
  * @retval GL_ErrStatus - GL_OK if successful, GL_ERROR otherwise
  */
GL_ErrStatus Set_Label( GL_Page_TypeDef* pPage, uint16_t ID, const uint8_t* label )
{
  GL_Label_TypeDef* pTmp;
  uint32_t index = 0;

  if (!pPage)
  {
    return GL_ERROR;
  }

  if (p_strlen(label) > 0)
  {
    /* search for the required label */
    while ( index < pPage->ControlCount )
    {
      if (pPage->PageControls[index]->objType == GL_LABEL)
      {
        pTmp = (GL_Label_TypeDef*)(pPage->PageControls[index]->objPTR);

        if ( pTmp->ID == ID)
        {
          GL_SetStringFieldValue(pTmp->label, (uint8_t *)label, MAX_LABEL_LENGTH);
          return GL_OK;
        }
      }
      index++;
    }
  }
  return GL_ERROR;
}

/**
  * @brief  Return the label of a Label object
  * @param  *pPage: Pointer to Page Structure
  * @param  ID: Object Identifier
  * @param  label: Variable where to copy the label of a Label Object
  * @retval GL_ErrStatus - GL_OK if successful, GL_ERROR otherwise
  */
GL_ErrStatus Get_Label ( GL_Page_TypeDef* pPage, uint16_t ID, uint8_t* label )
{
  GL_Label_TypeDef* pTmp;
  uint32_t index = 0;

  if (!pPage)
  {
    return GL_ERROR;
  }

  while ( index < pPage->ControlCount ) /* search for the required label */
  {
    if (pPage->PageControls[index]->objType == GL_LABEL)
    {
      pTmp = (GL_Label_TypeDef*)(pPage->PageControls[index]->objPTR);

      if ( pTmp->ID == ID)
      {
        GL_SetStringFieldValue(label, (uint8_t *)pTmp->label, MAX_LABEL_LENGTH);
        return GL_OK;
      }
    }
    index++;
  }
  return GL_ERROR;
}

/**
  * @brief  Return the current value of cursor position in a slidebar object
  * @param  *pPage: Pointer to Page Structure
  * @param  ID: Object Identifier
  * @retval uint8_t - The Cursor Position value (0-100 percentage)
  */
uint8_t Get_SlidebarValue ( GL_Page_TypeDef* pPage, uint16_t ID )
{
  GL_Slidebar_TypeDef* pTmp = NULL;
  uint32_t index = 0;

  if (!pPage)
  {
    return GL_ERROR;
  }

  while ( index < pPage->ControlCount ) /* search for the required label */
  {
    if (pPage->PageControls[index]->objType == GL_SLIDEBAR)
    {
      pTmp = (GL_Slidebar_TypeDef*)(pPage->PageControls[index]->objPTR);

      if ( pTmp->ID == ID )
      {
        return pTmp->CurrentValue;
      }
    }
    index++;
  }
  return GL_NULL;
}

/**
  * @brief  Assign Label Methods/Properties members.
  * @param  *pThis: Pointer to Object Structure
  * @retval GL_ErrStatus - GL_OK if successful, GL_ERROR otherwise
  */
static GL_ErrStatus Create_Label (GL_Label_TypeDef* pThis)
{
  if (!pThis)
  {
    return GL_ERROR;
  }
  pThis->Control_Visible = GL_TRUE;
  return GL_OK;
}

/**
  * @brief  Assign Button Methods/Properties members.
  * @param  *pThis: Pointer to Object Structure
  * @retval GL_ErrStatus - GL_OK if successful, GL_ERROR otherwise
  */
static GL_ErrStatus Create_Button (GL_Button_TypeDef* pThis)
{
  if (!pThis)
  {
    return GL_ERROR;
  }
  pThis->isObjectTouched = GL_FALSE;
  pThis->Control_Visible = GL_TRUE;
  return GL_OK;
}

/**
  * @brief  Assign RadioButtonGrp Methods/Properties members.
  * @param  *pThis: Pointer to Object Structure
  * @retval GL_ErrStatus - GL_OK if successful, GL_ERROR otherwise
  */
static GL_ErrStatus Create_RadioButtonGrp (GL_RadioButtonGrp_TypeDef* pThis)
{
  if (!pThis)
  {
    return GL_ERROR;
  }
  pThis->RadioOptionCount = 0;
  /* Assign member functions */
  pThis->AddRadioOption = AddRadioOption;
  return GL_OK;
}

/**
  * @brief  Assign RadioButton Methods/Properties members.
  * @param  *pThis: Pointer to Object Structure
  * @retval GL_ErrStatus - GL_OK if successful, GL_ERROR otherwise
  */
static GL_ErrStatus Create_RadioButtonOption (GL_RadioOption_TypeDef* pThis)
{
  if (!pThis)
  {
    return GL_ERROR;
  }
  pThis->Control_Visible = GL_TRUE;
#ifndef USE_2D_OBJECTS
  pThis->ImageCheckedPTR   = (uint8_t*)RButtonA;
  pThis->ImageUnCheckedPTR = (uint8_t*)RButtonB;
#endif
  pThis->IsChecked = GL_FALSE;

  return GL_OK;
}

/**
  * @brief  Assign ComboBoxGrp Methods/Properties members.
  * @param  *pThis: Pointer to Object Structure
  * @retval GL_ErrStatus - GL_OK if successful, GL_ERROR otherwise
  */
static GL_ErrStatus Create_ComboBoxGrp (GL_ComboBoxGrp_TypeDef* pThis)
{
  if (!pThis)
  {
    return GL_ERROR;
  }
#ifndef USE_2D_OBJECTS
  pThis->ImageUnClickedPTR = (uint8_t*)ArrowUpUntouched;
  pThis->ImageClickedPTR = (uint8_t*)ArrowUpTouched;
#endif
  pThis->Control_Visible = GL_TRUE;
  pThis->Secondary = GL_FALSE;
  pThis->ComboOptionCount = 0;
  /* Assign member functions */
  pThis->AddComboOption = AddComboOption;

  return GL_OK;
}

/**
  * @brief  Assign ComboBox Option Methods/Properties members.
  * @param  *pThis: Pointer to Object Structure
  * @retval GL_ErrStatus - GL_OK if successful, GL_ERROR otherwise
  */
static GL_ErrStatus Create_ComboBoxOption (GL_ComboOption_TypeDef* pThis)
{
  if (!pThis)
  {
    return GL_ERROR;
  }
  pThis->BackColor = GL_White;
  pThis->TextColor = GL_Blue;
  pThis->IsActive = GL_FALSE;

  return GL_OK;
}

/**
  * @brief  Assign Checkbox Methods/Properties members.
  * @param  *pThis: Pointer to Object Structure
  * @retval GL_ErrStatus - GL_OK if successful, GL_ERROR otherwise
  */
static GL_ErrStatus Create_Checkbox (GL_Checkbox_TypeDef* pThis)
{
  if (!pThis)
  {
    return GL_ERROR;
  }
  pThis->IsChecked = GL_FALSE;
  pThis->Control_Visible = GL_TRUE;
  return GL_OK;
}

/**
  * @brief  Assign Slidebar Methods/Properties members.
  * @param  *pThis: Pointer to Object Structure
  * @retval GL_ErrStatus - GL_OK if successful, GL_ERROR otherwise
  */
static GL_ErrStatus Create_Slidebar (GL_Slidebar_TypeDef* pThis)
{
  if (!pThis)
  {
    return GL_ERROR;
  }
  pThis->Control_Visible = GL_TRUE;
  /*Allocate memory for cursor behind array. Actual behind of cursor is stored here*/
  pThis->BehindCursor = (uint16_t *)malloc(SlidebarCursorPointer[2] * sizeof(uint16_t));
  return GL_OK;
}

/**
  * @brief  Assign Switch Methods/Properties members.
  * @param  *pThis: Pointer to Object Structure
  * @retval GL_ErrStatus - GL_OK if successful, GL_ERROR otherwise
  */
static GL_ErrStatus Create_Switch (GL_Switch_TypeDef* pThis)
{
  if (!pThis)
  {
    return GL_ERROR;
  }
  pThis->isObjectTouched = GL_FALSE;
  pThis->Control_Visible = GL_TRUE;
  return GL_OK;
}

/**
  * @brief  Assign Icon Methods/Properties members.
  * @param  *pThis: Pointer to Object Structure
  * @retval GL_ErrStatus - GL_OK if successful, GL_ERROR otherwise
  */
static GL_ErrStatus Create_Icon (GL_Icon_TypeDef* pThis)
{
  if (!pThis)
  {
    return GL_ERROR;
  }
  pThis->isObjectTouched = GL_FALSE;
  pThis->Control_Visible = GL_TRUE;
  return GL_OK;
}

/**
  * @brief  Assign Histogram Methods/Properties members.
  * @param  *pThis: Pointer to Object Structure
  * @retval GL_ErrStatus - GL_OK if successful, GL_ERROR otherwise
  */
static GL_ErrStatus Create_Histogram (GL_Histogram_TypeDef* pThis)
{
  if (!pThis)
  {
    return GL_ERROR;
  }
  pThis->Control_Visible = GL_TRUE;
  return GL_OK;
}

/**
  * @brief  Assign GraphChart Methods/Properties members.
  * @param  *pThis: Pointer to Object Structure
  * @retval GL_ErrStatus - GL_OK if successful, GL_ERROR otherwise
  */
static GL_ErrStatus Create_GraphChart (GL_GraphChart_TypeDef* pThis)
{
  if (!pThis)
  {
    return GL_ERROR;
  }
  pThis->Control_Visible = GL_TRUE;
  return GL_OK;
}

/**
  * @brief  Assign PageObj Methods/Properties members.
  * @param  *pThis: Pointer to Object Structure
  * @retval GL_ErrStatus - GL_OK if successful, GL_ERROR otherwise
  */
GL_ErrStatus Create_PageObj (GL_Page_TypeDef* pThis)
{
  if (!pThis)
  {
    return GL_ERROR;
  }
  else
  {
    pThis->Page_Active       = GL_FALSE;
    pThis->Page_Visible      = GL_FALSE;
    /* Assign member functions */
    pThis->ShowPage          = ShowPage;
    pThis->SetPage           = SetPage;
    pThis->GetObjStatus      = GetObjStatus;
    pThis->GetObjCoordinates = GetObjCoordinates;
    pThis->ControlCount      = 0;
  
    memset(pThis->PageControls,0x00,sizeof(GL_PageControls_TypeDef*)*MAX_CTRL_X_PAGE);
  
    /* pThis->GetObjSize = GetObjSize; */
    pThis->SetPage(pThis, GL_FALSE);
    if ( PageCount < PAGE_MAX_NUM )
    {
      PagesList[PageCount] = pThis;
      PageCount++;
    }
    else
    {
      return GL_ERROR;
    }
  }
  return GL_OK;
}

/**
  * @brief  Free the allocated memory for a PageControl object
  * @param  *pPageControlObj: Pointer to Object Structure
  * @retval GL_ErrStatus - GL_OK if successful, GL_ERROR otherwise
  */
GL_ErrStatus DestroyPageControl ( GL_Page_TypeDef* pPage, uint16_t ID )
{
  uint32_t index = 0;
  if (!&pPage)
  {
    return GL_ERROR;
  }

  /* search for the required object */
  while ( index < pPage->ControlCount )
  {
    if (pPage->PageControls[index]->objType == GL_LABEL)
    {
      GL_Label_TypeDef* pTmp;
      pTmp = (GL_Label_TypeDef*)(pPage->PageControls[index]->objPTR);

      if ( pTmp->ID == ID )
      {
        free(pPage->PageControls[index]->objPTR);
        pPage->PageControls[index]->objPTR = GL_NULL;
        free(pPage->PageControls[index]);
        if (index != pPage->ControlCount - 1)
        {
          pPage->PageControls[index] = pPage->PageControls[pPage->ControlCount-1];
          pPage->PageControls[pPage->ControlCount-1] = GL_NULL;
          pPage->ControlCount--;
        }
        else
        {
          pPage->PageControls[index] = GL_NULL;
          pPage->ControlCount--;
        }
        return GL_OK;
      }
    }
    else if (pPage->PageControls[index]->objType == GL_BUTTON)
    {
      GL_Button_TypeDef* pTmp;
      pTmp = (GL_Button_TypeDef*)(pPage->PageControls[index]->objPTR);

      if ( pTmp->ID == ID )
      {
        free(pPage->PageControls[index]->objPTR);
        pPage->PageControls[index]->objPTR = GL_NULL;
        free(pPage->PageControls[index]);
        if (index != pPage->ControlCount - 1)
        {
          pPage->PageControls[index] = pPage->PageControls[pPage->ControlCount-1];
          pPage->PageControls[pPage->ControlCount-1] = GL_NULL;
          pPage->ControlCount--;
        }
        else
        {
          pPage->PageControls[index] = GL_NULL;
          pPage->ControlCount--;
        }
        return GL_OK;
      }
    }
    else if (pPage->PageControls[index]->objType == GL_SWITCH)
    {
      GL_Switch_TypeDef* pTmp;
      pTmp = (GL_Switch_TypeDef*)(pPage->PageControls[index]->objPTR);

      if ( pTmp->ID == ID )
      {
        free(pPage->PageControls[index]->objPTR);
        pPage->PageControls[index]->objPTR = GL_NULL;
        free(pPage->PageControls[index]);
        if (index != pPage->ControlCount - 1)
        {
          pPage->PageControls[index] = pPage->PageControls[pPage->ControlCount-1];
          pPage->PageControls[pPage->ControlCount-1] = GL_NULL;
          pPage->ControlCount--;
        }
        else
        {
          pPage->PageControls[index] = GL_NULL;
          pPage->ControlCount--;
        }
        return GL_OK;
      }
    }
    else if (pPage->PageControls[index]->objType == GL_CHECKBOX)
    {
      GL_Checkbox_TypeDef* pTmp;
      pTmp = (GL_Checkbox_TypeDef*)(pPage->PageControls[index]->objPTR);

      if ( pTmp->ID == ID )
      {
        free(pPage->PageControls[index]->objPTR);
        pPage->PageControls[index]->objPTR = GL_NULL;
        free(pPage->PageControls[index]);
        if (index != pPage->ControlCount - 1)
        {
          pPage->PageControls[index] = pPage->PageControls[pPage->ControlCount-1];
          pPage->PageControls[pPage->ControlCount-1] = GL_NULL;
          pPage->ControlCount--;
        }
        else
        {
          pPage->PageControls[index] = GL_NULL;
          pPage->ControlCount--;
        }
        return GL_OK;
      }
    }
    else if (pPage->PageControls[index]->objType == GL_RADIO_BUTTON)
    {
      GL_RadioButtonGrp_TypeDef* pTmp;
      pTmp = (GL_RadioButtonGrp_TypeDef*)(((GL_RadioOption_TypeDef*)pPage->PageControls[index]->objPTR)->RadioButtonGrp);

      if (pTmp->ID == ID )
      {
        if (--pTmp->RadioOptionCount == 0)
        {
          free(pTmp);
        }
        free(pPage->PageControls[index]->objPTR);
        pPage->PageControls[index]->objPTR = GL_NULL;
        free(pPage->PageControls[index]);
        if (index != pPage->ControlCount - 1)
        {
          pPage->PageControls[index] = pPage->PageControls[pPage->ControlCount-1];
          pPage->PageControls[pPage->ControlCount-1] = GL_NULL;
          pPage->ControlCount--;
        }
        else
        {
          pPage->PageControls[index] = GL_NULL;
          pPage->ControlCount--;
        }
        return GL_OK;
      }
    }
    else if (pPage->PageControls[index]->objType == GL_COMBOBOX)
    {
      uint8_t free_counter = 0;
      GL_ComboBoxGrp_TypeDef* pTmp;
      pTmp = (GL_ComboBoxGrp_TypeDef*)(pPage->PageControls[index]->objPTR);

      if ( pTmp->ID == ID )
      {
        for ( free_counter = 0; free_counter < pTmp->ComboOptionCount; free_counter++)
        {
          free(pTmp->ComboOptions[free_counter]);
        }
        pTmp->ComboOptionCount = 0;
        free(pPage->PageControls[index]->objPTR);
        pPage->PageControls[index]->objPTR = GL_NULL;
        free(pPage->PageControls[index]);
        if (index != pPage->ControlCount - 1)
        {
          pPage->PageControls[index] = pPage->PageControls[pPage->ControlCount-1];
          pPage->PageControls[pPage->ControlCount-1] = GL_NULL;
          pPage->ControlCount--;
        }
        else
        {
          pPage->PageControls[index] = GL_NULL;
          pPage->ControlCount--;
        }
        return GL_OK;
      }
    }
    else if (pPage->PageControls[index]->objType == GL_ICON)
    {
      GL_Icon_TypeDef* pTmp;
      pTmp = (GL_Icon_TypeDef*)(pPage->PageControls[index]->objPTR);

      if ( pTmp->ID == ID )
      {
        free(pPage->PageControls[index]->objPTR);
        pPage->PageControls[index]->objPTR = GL_NULL;
        free(pPage->PageControls[index]);
        if (index != pPage->ControlCount - 1)
        {
          pPage->PageControls[index] = pPage->PageControls[pPage->ControlCount-1];
          pPage->PageControls[pPage->ControlCount-1] = GL_NULL;
          pPage->ControlCount--;
        }
        else
        {
          pPage->PageControls[index] = GL_NULL;
          pPage->ControlCount--;
        }
        return GL_OK;
      }
    }
    else if (pPage->PageControls[index]->objType == GL_SLIDEBAR)
    {
      GL_Slidebar_TypeDef* pTmp;
      pTmp = (GL_Slidebar_TypeDef*)(pPage->PageControls[index]->objPTR);

      if ( pTmp->ID == ID )
      {
        if (pTmp->BehindCursor != NULL)
        {
          free(pTmp->BehindCursor);
          pTmp->BehindCursor = NULL;
        }
        memset(pPage->PageControls[index]->objPTR, 0x00, sizeof(GL_Slidebar_TypeDef));
        free(pPage->PageControls[index]->objPTR);
        pPage->PageControls[index]->objPTR = GL_NULL;
        free(pPage->PageControls[index]);
        if (index != pPage->ControlCount - 1)
        {
          pPage->PageControls[index] = pPage->PageControls[pPage->ControlCount-1];
          pPage->PageControls[pPage->ControlCount-1] = GL_NULL;
          pPage->ControlCount--;
        }
        else
        {
          pPage->PageControls[index] = GL_NULL;
          pPage->ControlCount--;
        }
        return GL_OK;
      }
    }
    else if (pPage->PageControls[index]->objType == GL_HISTOGRAM)
    {
      GL_Histogram_TypeDef* pTmp;
      pTmp = (GL_Histogram_TypeDef*)(pPage->PageControls[index]->objPTR);

      if ( pTmp->ID == ID )
      {
        free(pPage->PageControls[index]->objPTR);
        pPage->PageControls[index]->objPTR = GL_NULL;
        free(pPage->PageControls[index]);
        if (index != pPage->ControlCount - 1)
        {
          pPage->PageControls[index] = pPage->PageControls[pPage->ControlCount-1];
          pPage->PageControls[pPage->ControlCount-1] = GL_NULL;
          pPage->ControlCount--;
        }
        else
        {
          pPage->PageControls[index] = GL_NULL;
          pPage->ControlCount--;
        }
        return GL_OK;
      }
    }
    else if (pPage->PageControls[index]->objType == GL_GRAPH_CHART)
    {
      GL_GraphChart_TypeDef* pTmp;
      pTmp = (GL_GraphChart_TypeDef*)(pPage->PageControls[index]->objPTR);

      if ( pTmp->ID == ID )
      {
        free(pPage->PageControls[index]->objPTR);
        pPage->PageControls[index]->objPTR = GL_NULL;
        free(pPage->PageControls[index]);
        if (index != pPage->ControlCount - 1)
        {
          pPage->PageControls[index] = pPage->PageControls[pPage->ControlCount-1];
          pPage->PageControls[pPage->ControlCount-1] = GL_NULL;
          pPage->ControlCount--;
        }
        else
        {
          pPage->PageControls[index] = GL_NULL;
          pPage->ControlCount--;
        }
        return GL_OK;
      }
    }
    index++;
  }
  return GL_ERROR;
}

/**
  * @brief  Free the allocated memory for all the PageControl objects belonging to a Page
  * @param  *pThis: Pointer to Page Structure
  * @retval GL_ErrStatus - GL_OK if successful, GL_ERROR otherwise
  */
GL_ErrStatus DestroyPage (GL_Page_TypeDef * pPage)
{
  int32_t index = 0;

  if (!pPage)
  {
    return GL_ERROR;
  }

  index = pPage->ControlCount - 1;
  while ( index >= 0 ) /* destroy all the PageControl object of the given Page */
  {
    if (pPage->PageControls[index]->objType == GL_LABEL)
    {
      GL_Label_TypeDef* pTmp;
      pTmp = (GL_Label_TypeDef*)(pPage->PageControls[index]->objPTR);
      DestroyPageControl( pPage, pTmp->ID );
    }
    else if (pPage->PageControls[index]->objType == GL_BUTTON)
    {
      GL_Button_TypeDef* pTmp;
      pTmp = (GL_Button_TypeDef*)(pPage->PageControls[index]->objPTR);
      DestroyPageControl( pPage, pTmp->ID );
    }
    else if (pPage->PageControls[index]->objType == GL_SWITCH)
    {
      GL_Switch_TypeDef* pTmp;
      pTmp = (GL_Switch_TypeDef*)(pPage->PageControls[index]->objPTR);
      DestroyPageControl( pPage, pTmp->ID );
    }
    else if (pPage->PageControls[index]->objType == GL_CHECKBOX)
    {
      GL_Checkbox_TypeDef* pTmp;
      pTmp = (GL_Checkbox_TypeDef*)(pPage->PageControls[index]->objPTR);
      DestroyPageControl( pPage, pTmp->ID );
    }
    else if (pPage->PageControls[index]->objType == GL_RADIO_BUTTON)
    {
      GL_RadioButtonGrp_TypeDef* pTmp;
      pTmp = (GL_RadioButtonGrp_TypeDef*)(((GL_RadioOption_TypeDef*)pPage->PageControls[index]->objPTR)->RadioButtonGrp);
      DestroyPageControl( pPage, pTmp->ID );
    }
    else if (pPage->PageControls[index]->objType == GL_COMBOBOX)
    {
      GL_ComboBoxGrp_TypeDef* pTmp;
      pTmp = (GL_ComboBoxGrp_TypeDef*)(pPage->PageControls[index]->objPTR);
      DestroyPageControl( pPage, pTmp->ID );
    }
    else if (pPage->PageControls[index]->objType == GL_ICON)
    {
      GL_Icon_TypeDef* pTmp;
      pTmp = (GL_Icon_TypeDef*)(pPage->PageControls[index]->objPTR);
      DestroyPageControl( pPage, pTmp->ID );
    }
    else if (pPage->PageControls[index]->objType == GL_SLIDEBAR)
    {
      GL_Slidebar_TypeDef* pTmp;
      pTmp = (GL_Slidebar_TypeDef*)(pPage->PageControls[index]->objPTR);
      DestroyPageControl( pPage, pTmp->ID );
    }
    else if (pPage->PageControls[index]->objType == GL_HISTOGRAM)
    {
      GL_Histogram_TypeDef* pTmp;
      pTmp = (GL_Histogram_TypeDef*)(pPage->PageControls[index]->objPTR);
      DestroyPageControl( pPage, pTmp->ID );
    }
    else if (pPage->PageControls[index]->objType == GL_GRAPH_CHART)
    {
      GL_GraphChart_TypeDef* pTmp;
      pTmp = (GL_GraphChart_TypeDef*)(pPage->PageControls[index]->objPTR);
      DestroyPageControl( pPage, pTmp->ID );
    }
    index--;
  }
  pPage->ControlCount = 0;
  return GL_OK;
}

/**
  * @brief  Show the Control Object at the specified coordinates.
  * @param  *pThis: Pointer to Object Structure
  * @param  *objCoordinates: Pointer to objCoordinates Structure
  * @retval GL_ErrStatus - GL_OK if successful, GL_ERROR otherwise
  */
static GL_ErrStatus SetLabelVisible(GL_PageControls_TypeDef* pTmp, GL_Coordinate_TypeDef objCoordinates)
{
  GL_Label_TypeDef* pThis = (GL_Label_TypeDef*)(pTmp->objPTR);
  if (!pThis)
  {
    return GL_ERROR;
  }
  pThis->Control_Visible = GL_TRUE;
  if (pTmp->objType == GL_LABEL)
  {
    GL_SetTextColor(pThis->Colour);
    if (pThis->FontSize == GL_FONT_SMALL)
    {
      GL_SetFont(GL_FONT_SMALL);
      if ( pThis->Direction == GL_HORIZONTAL )
      {
        GL_DisplayAdjStringLine( (uint8_t)(objCoordinates.MinY), (uint16_t)(objCoordinates.MaxX),
                                 (uint8_t*)pThis->label,
                                 GL_TRUE);
      }
      else if ( pThis->Direction == GL_LEFT_VERTICAL )
      {
        LCD_Change_Direction(_270_degree);
        LCD_PrintStringLine( (uint16_t)(LCD_Width - objCoordinates.MinY),
                             (uint16_t)(objCoordinates.MaxX + p_strlen((pThis->label))*GL_FONT_SMALL_WIDTH),
                             (uint8_t*)pThis->label);
        LCD_Change_Direction(_0_degree);
      }
      else if ( pThis->Direction == GL_RIGHT_VERTICAL )
      {
        LCD_Change_Direction(_90_degree);
        LCD_PrintStringLine( (uint16_t)(objCoordinates.MinY),
                             (uint16_t)(objCoordinates.MaxX),
                             (uint8_t*)pThis->label);
        LCD_Change_Direction(_0_degree);
      }

      GL_SetFont(GL_FONT_BIG);
    }
    else
    {
      if ( pThis->Direction == GL_HORIZONTAL )
      {
        GL_DisplayAdjStringLine( (uint8_t)(objCoordinates.MinY), (uint16_t)(objCoordinates.MaxX), (uint8_t*)pThis->label, GL_TRUE);
      }
      else if ( pThis->Direction == GL_LEFT_VERTICAL )
      {
        LCD_Change_Direction(_270_degree);
        LCD_PrintStringLine( (uint16_t)(LCD_Width - objCoordinates.MinY),
                             (uint16_t)( objCoordinates.MaxX + p_strlen((pThis->label))*(GL_FONT_BIG_WIDTH - 1) ),
                             (uint8_t*)pThis->label);
        LCD_Change_Direction(_0_degree);
      }
      else if ( pThis->Direction == GL_RIGHT_VERTICAL )
      {
        LCD_Change_Direction(_90_degree);
        LCD_PrintStringLine( (uint16_t)(objCoordinates.MinY),
                             (uint16_t)(objCoordinates.MaxX),
                             (uint8_t*)pThis->label);
        LCD_Change_Direction(_0_degree);
      }
    }
  }

  return GL_OK;
}

/**
  * @brief  Show the Control Object at the specified coordinates.
  * @param  *pThis: Pointer to Object Structure
  * @param  *objCoordinates: Pointer to objCoordinates Structure
  * @retval GL_ErrStatus - GL_OK if successful, GL_ERROR otherwise
  */
static GL_ErrStatus SetButtonVisible(GL_PageControls_TypeDef* pTmp, GL_Coordinate_TypeDef objCoordinates)
{
  uint8_t btn_length = 0;
  uint32_t LabelLength = 0;
  GL_Button_TypeDef* pThis = (GL_Button_TypeDef*)(pTmp->objPTR);
#ifndef USE_2D_OBJECTS
  uint32_t n = 1;
  uint8_t* ptrBitmapLeft = NULL;
  uint8_t* ptrBitmapCenter = NULL;
  uint8_t* ptrBitmapRight = NULL;
#endif
  if (!pThis)
  {
    return GL_ERROR;
  }
  pThis->Control_Visible = GL_TRUE;
  LabelLength = p_strlen(pThis->label);

  if (pThis->isObjectTouched == GL_FALSE)
  {
#ifndef USE_2D_OBJECTS
    if ( pThis->ImageUnClickedPTR == BtnNormal)
    {
      ptrBitmapLeft = (uint8_t*)BtnNormalLeft;
      ptrBitmapCenter = pThis->ImageUnClickedPTR;
      ptrBitmapRight = (uint8_t*)BtnNormalRight;
    }
    else
    {
      ptrBitmapLeft = (uint8_t*)BtnPressedLeft;
      ptrBitmapCenter = pThis->ImageUnClickedPTR;
      ptrBitmapRight = (uint8_t*)BtnPressedRight;
    }
#else
    GL_SetTextColor(GL_Black);
    if (LabelLength>MAX_BUTTON_LABEL_LENGTH)
      GL_DrawFilledRectangle((uint16_t)(objCoordinates.MaxX), 
                             (uint16_t)(objCoordinates.MaxX-((MAX_BUTTON_LABEL_LENGTH-1)*FONT_LENGTH)), 
                             (uint8_t)(objCoordinates.MaxY), 
                             (uint8_t)(objCoordinates.MinY), GL_Blue);
    else
      GL_DrawFilledRectangle((uint16_t)(objCoordinates.MaxX), 
                             (uint16_t)(objCoordinates.MaxX-((LabelLength+1)*FONT_LENGTH)), 
                             (uint8_t)(objCoordinates.MaxY), 
                             (uint8_t)(objCoordinates.MinY), GL_Blue);
#endif
  }
  else if (pThis->isObjectTouched == GL_TRUE)
  {
#ifndef USE_2D_OBJECTS
    if ( pThis->ImageClickedPTR == BtnPressed)
    {
      ptrBitmapLeft = (uint8_t*)BtnPressedLeft;
      ptrBitmapCenter = pThis->ImageClickedPTR;
      ptrBitmapRight = (uint8_t*)BtnPressedRight;
    }
    else
    {
      ptrBitmapLeft = (uint8_t*)BtnNormalLeft;
      ptrBitmapCenter = pThis->ImageClickedPTR;
      ptrBitmapRight = (uint8_t*)BtnNormalRight;
    }
#else
    GL_SetTextColor(GL_Black);
    if (LabelLength>MAX_BUTTON_LABEL_LENGTH)
      GL_DrawFilledRectangle((uint16_t)(objCoordinates.MaxX), 
                             (uint16_t)(objCoordinates.MaxX-((MAX_BUTTON_LABEL_LENGTH-1)*FONT_LENGTH)), 
                             (uint8_t)(objCoordinates.MaxY), 
                             (uint8_t)(objCoordinates.MinY), GL_Grey);
    else
      GL_DrawFilledRectangle((uint16_t)(objCoordinates.MaxX), 
                             (uint16_t)(objCoordinates.MaxX-((LabelLength+1)*FONT_LENGTH)), 
                             (uint8_t)(objCoordinates.MaxY), 
                             (uint8_t)(objCoordinates.MinY), GL_Grey);
#endif
  }

#ifndef USE_2D_OBJECTS
  GL_DrawButtonBMP( (uint16_t)(objCoordinates.MaxX),
                    (uint16_t)(objCoordinates.MaxX - BUTTON_SLICE_LENGTH),
                    (uint8_t)(objCoordinates.MaxY),
                    (uint8_t)(objCoordinates.MinY),
                    ptrBitmapLeft );

  if (LabelLength < 10)
  {
    for ( ; n < LabelLength && (objCoordinates.MaxX - ((n + 1)*BUTTON_SLICE_LENGTH)) > BUTTON_SLICE_LENGTH + 1; n++)
    {
      GL_DrawButtonBMP( (uint16_t)(objCoordinates.MaxX - (n*BUTTON_SLICE_LENGTH)),
                        (uint16_t)(objCoordinates.MaxX - ((n + 1)*BUTTON_SLICE_LENGTH)),
                        (uint8_t)(objCoordinates.MaxY),
                        (uint8_t)(objCoordinates.MinY),
                        ptrBitmapCenter );
    }
  }
  else
  {
    for ( ; n < LabelLength - 1 && (objCoordinates.MaxX - ((n + 1)*BUTTON_SLICE_LENGTH)) > BUTTON_SLICE_LENGTH + 1; n++)
    {
      GL_DrawButtonBMP( (uint16_t)(objCoordinates.MaxX - (n*BUTTON_SLICE_LENGTH)),
                        (uint16_t)(objCoordinates.MaxX - ((n + 1)*BUTTON_SLICE_LENGTH)),
                        (uint8_t)(objCoordinates.MaxY), (uint8_t)(objCoordinates.MinY),
                        ptrBitmapCenter );
    }
  }
  if ( (objCoordinates.MaxX - ((n + 1)*BUTTON_SLICE_LENGTH)) > 0 )
  {
    GL_DrawButtonBMP( (uint16_t)(objCoordinates.MaxX - (n*BUTTON_SLICE_LENGTH)),
                      (uint16_t)(objCoordinates.MaxX - ((n + 1)*BUTTON_SLICE_LENGTH)),
                      (uint8_t)(objCoordinates.MaxY), (uint8_t)(objCoordinates.MinY),
                      ptrBitmapRight );
  }
#endif
  GL_SetTextColor(GL_White);
  GL_SetFont(GL_FONT_SMALL);

  btn_length = LabelLength * FONT_LENGTH + 2 * FONT_LENGTH;
  if (LabelLength < 7)
  {
    GL_DisplayAdjStringLine( (uint8_t)(objCoordinates.MaxY) - 18,
                             (uint16_t)(objCoordinates.MaxX) - ((btn_length - (LabelLength + 1)*FONT_LENGTH) / 2) - 2,
                             (uint8_t*)pThis->label,
                             GL_TRUE);
  }
  else
  {
    GL_DisplayAdjStringLine( (uint8_t)(objCoordinates.MaxY) - 18,
                             (uint16_t)(objCoordinates.MaxX) - ((btn_length - (LabelLength + 1)*FONT_LENGTH) / 2) - 4,
                             (uint8_t*)pThis->label,
                             GL_TRUE);
  }

  GL_SetFont(GL_FONT_BIG);
  GL_SetBackColor(GL_White);
  GL_SetTextColor(GL_Blue);

  return GL_OK;
}

/**
  * @brief  Show the Control Object at the specified coordinates.
  * @param  *pThis: Pointer to Object Structure
  * @param  *objCoordinates: Pointer to objCoordinates Structure
  * @retval GL_ErrStatus - GL_OK if successful, GL_ERROR otherwise
  */
static GL_ErrStatus SetSwitchVisible(GL_PageControls_TypeDef* pTmp, GL_Coordinate_TypeDef objCoordinates)
{
  uint32_t maxLabelLength = 0, btn_length = 0;
  GL_Switch_TypeDef* pThis = (GL_Switch_TypeDef*)(pTmp->objPTR);
#ifndef USE_2D_OBJECTS
  uint32_t n = 1;
  uint8_t* ptrBitmapLeft = NULL;
  uint8_t* ptrBitmapCenter = NULL;
  uint8_t* ptrBitmapRight = NULL;
#endif
  if (!pThis)
  {
    return GL_ERROR;
  }

  pThis->Control_Visible = GL_TRUE;

  if ( p_strlen(pThis->label_1) > p_strlen(pThis->label_2) )
  {
    maxLabelLength = p_strlen(pThis->label_1);
  }
  else
  {
    maxLabelLength = p_strlen(pThis->label_2);
  }

  GL_SetFont(GL_FONT_SMALL);
  if (pThis->isObjectTouched == GL_FALSE)
  {
#ifndef USE_2D_OBJECTS
    if ( pThis->ImageUnClickedPTR == SwitchNormal)
    {
      ptrBitmapLeft = (uint8_t*)SwitchNormalLeft;
      ptrBitmapCenter = pThis->ImageUnClickedPTR;
      ptrBitmapRight = (uint8_t*)SwitchNormalRight;
    }
    else
    {
      ptrBitmapLeft = (uint8_t*)SwitchPressedLeft;
      ptrBitmapCenter = pThis->ImageUnClickedPTR;
      ptrBitmapRight = (uint8_t*)SwitchPressedRight;
    }
#else
    GL_SetTextColor(GL_Black);
    if (maxLabelLength>MAX_SWITCH_LABEL_LENGTH)
      GL_DrawFilledRectangle((uint16_t)(objCoordinates.MaxX), 
                             (uint16_t)(objCoordinates.MaxX-((MAX_SWITCH_LABEL_LENGTH-1)*FONT_LENGTH)), 
                             (uint8_t)(objCoordinates.MaxY), 
                             (uint8_t)(objCoordinates.MinY), GL_Blue);
    else
      GL_DrawFilledRectangle((uint16_t)(objCoordinates.MaxX), 
                             (uint16_t)(objCoordinates.MaxX-((maxLabelLength+1)*FONT_LENGTH)), 
                             (uint8_t)(objCoordinates.MaxY), 
                             (uint8_t)(objCoordinates.MinY), GL_Blue);
#endif
  }
  else if (pThis->isObjectTouched == GL_TRUE)
  {
#ifndef USE_2D_OBJECTS
    if ( pThis->ImageClickedPTR == SwitchPressed)
    {
      GL_SetBackColor(0xDEFB);
      ptrBitmapLeft = (uint8_t*)SwitchPressedLeft;
      ptrBitmapCenter = pThis->ImageClickedPTR;
      ptrBitmapRight = (uint8_t*)SwitchPressedRight;
    }
    else
    {
      ptrBitmapLeft = (uint8_t*)SwitchNormalLeft;
      ptrBitmapCenter = pThis->ImageClickedPTR;
      ptrBitmapRight = (uint8_t*)SwitchNormalRight;
    }
#else
    GL_SetTextColor(GL_Black);
    if (maxLabelLength>MAX_SWITCH_LABEL_LENGTH)
      GL_DrawFilledRectangle((uint16_t)(objCoordinates.MaxX), 
                             (uint16_t)(objCoordinates.MaxX-((MAX_SWITCH_LABEL_LENGTH-1)*FONT_LENGTH)), 
                             (uint8_t)(objCoordinates.MaxY), 
                             (uint8_t)(objCoordinates.MinY), GL_Grey);
    else
      GL_DrawFilledRectangle((uint16_t)(objCoordinates.MaxX), 
                             (uint16_t)(objCoordinates.MaxX-((maxLabelLength+1)*FONT_LENGTH)), 
                             (uint8_t)(objCoordinates.MaxY), 
                             (uint8_t)(objCoordinates.MinY), GL_Grey);
#endif
  }
  
#ifndef USE_2D_OBJECTS
  GL_DrawButtonBMP( (uint16_t)(objCoordinates.MaxX), 
                    (uint16_t)(objCoordinates.MaxX - BUTTON_SLICE_LENGTH), 
                    (uint8_t)(objCoordinates.MaxY), 
                    (uint8_t)(objCoordinates.MinY), ptrBitmapLeft );
  for ( ; n < maxLabelLength && n < MAX_SWITCH_LABEL_LENGTH; n++)
  {
    GL_DrawButtonBMP( (uint16_t)(objCoordinates.MaxX - (n*BUTTON_SLICE_LENGTH)), 
                      (uint16_t)(objCoordinates.MaxX - ((n + 1)*BUTTON_SLICE_LENGTH)), 
                      (uint8_t)(objCoordinates.MaxY), 
                      (uint8_t)(objCoordinates.MinY), ptrBitmapCenter );
  }
  GL_DrawButtonBMP( (uint16_t)(objCoordinates.MaxX - (n*BUTTON_SLICE_LENGTH)), 
                    (uint16_t)(objCoordinates.MaxX - ((n + 1)*BUTTON_SLICE_LENGTH)), 
                    (uint8_t)(objCoordinates.MaxY), 
                    (uint8_t)(objCoordinates.MinY), ptrBitmapRight );
#endif
  GL_SetTextColor(GL_White);

  btn_length = maxLabelLength * FONT_LENGTH + 2 * FONT_LENGTH;
  if (pThis->isObjectTouched == GL_FALSE)
  {
    GL_DisplayAdjStringLine( (uint8_t)(objCoordinates.MaxY) - 18,
                            (uint16_t)(objCoordinates.MaxX) - ((btn_length - (p_strlen((pThis->label_1))+1)*FONT_LENGTH) / 2) - 3,
                            (uint8_t*)pThis->label_1, GL_TRUE);
  }
  else if (pThis->isObjectTouched == GL_TRUE)
  {
    GL_DisplayAdjStringLine( (uint8_t)(objCoordinates.MaxY) - 18,
                            (uint16_t)(objCoordinates.MaxX) - ((btn_length - (p_strlen((pThis->label_2))+1)*FONT_LENGTH) / 2) - 3,
                            (uint8_t*)pThis->label_2, GL_TRUE);
  }

  GL_SetFont(GL_FONT_BIG);
  GL_SetBackColor(GL_White);
  GL_SetTextColor(GL_Blue);

  return GL_OK;
}

/**
  * @brief  Show the Control Object at the specified coordinates.
  * @param  *pThis: Pointer to Object Structure
  * @param  *objCoordinates: Pointer to objCoordinates Structure
  * @retval GL_ErrStatus - GL_OK if successful, GL_ERROR otherwise
  */
static GL_ErrStatus SetRadioButtonVisible(GL_PageControls_TypeDef* pTmp, GL_Coordinate_TypeDef objCoordinates)
{
#ifndef USE_2D_OBJECTS
  uint8_t* ptrBitmap;
#endif
  uint8_t n = 0;
  uint32_t label_length = 0;
  GL_RadioOption_TypeDef* pThis = (GL_RadioOption_TypeDef*)(pTmp->objPTR);

  if (!pThis)
  {
    return GL_ERROR;
  }
  pThis->Control_Visible = GL_TRUE;

  if ( pThis->IsChecked == GL_TRUE )
  {
#ifndef USE_2D_OBJECTS
    ptrBitmap = pThis->ImageCheckedPTR;
#else
    GL_SetTextColor( GL_Black );
    GL_DrawFilledCircle( (uint16_t)(objCoordinates.MaxX), (uint8_t)(objCoordinates.MaxY), RADIO_BUTTON_RADIUS, GL_White );
    GL_DrawFilledCircle( (uint16_t)(objCoordinates.MaxX-(RADIO_BUTTON_RADIUS/2 -1)), 
                         (uint8_t)(objCoordinates.MaxY-(RADIO_BUTTON_RADIUS/2 -1)), 
                         RADIO_BUTTON_RADIUS/2 +2, GL_Black);
    GL_DrawFilledRectangle( (uint16_t)(objCoordinates.MaxX-(RADIO_BUTTON_RADIUS/2)-2), 
                            (uint16_t)(objCoordinates.MinX+(RADIO_BUTTON_RADIUS/2)+4), 
                            (uint8_t)(objCoordinates.MaxY-(RADIO_BUTTON_RADIUS/2)-2), 
                            (uint8_t)(objCoordinates.MinY+(RADIO_BUTTON_RADIUS/2)+4), GL_Black);
#endif
  }
  else
  {
#ifndef USE_2D_OBJECTS
    ptrBitmap = pThis->ImageUnCheckedPTR;
#else
    GL_SetTextColor( GL_Black );
    GL_DrawFilledCircle((uint16_t)(objCoordinates.MaxX), (uint8_t)(objCoordinates.MaxY), RADIO_BUTTON_RADIUS, GL_White);
    GL_DrawFilledRectangle( (uint16_t)(objCoordinates.MaxX-(RADIO_BUTTON_RADIUS/2)-2), 
                               (uint16_t)(objCoordinates.MinX+(RADIO_BUTTON_RADIUS/2)+4), 
                               (uint8_t)(objCoordinates.MaxY-(RADIO_BUTTON_RADIUS/2)-2), 
                               (uint8_t)(objCoordinates.MinY+(RADIO_BUTTON_RADIUS/2)+4), GL_White);
#endif
  }
  
#ifndef USE_2D_OBJECTS
  GL_DrawButtonBMP( (uint16_t)(objCoordinates.MaxX),
                    (uint16_t)(objCoordinates.MinX),
                    (uint8_t)(objCoordinates.MaxY),
                    (uint8_t)(objCoordinates.MinY),
                    ptrBitmap );
#endif
  GL_SetFont(GL_FONT_SMALL);
  label_length = p_strlen(pThis->label);
  for (; n <= label_length; n++)
  {
    GL_DisplayAdjStringLine( (uint8_t)(objCoordinates.MaxY) - (RADIO_BUTTON_RADIUS-2)*2,
                             (uint16_t)(objCoordinates.MaxX) - (RBUTTON_OPT_SIZE + 2) - n*FONT_LENGTH,
                             (uint8_t*)' ',
                             GL_FALSE);
  }
  GL_SetTextColor( GL_Black );
  GL_DisplayAdjStringLine( (uint8_t)(objCoordinates.MaxY) - (RADIO_BUTTON_RADIUS-2)*2,
                           (uint16_t)(objCoordinates.MaxX) - (RBUTTON_OPT_SIZE + 2),
                           (uint8_t*)pThis->label,
                           GL_TRUE);

  GL_SetFont(GL_FONT_BIG);

  return GL_OK;
}

/**
  * @brief  Show the Control Object at the specified coordinates.
  * @param  *pThis: Pointer to Object Structure
  * @param  *objCoordinates: Pointer to objCoordinates Structure
  * @retval GL_ErrStatus - GL_OK if successful, GL_ERROR otherwise
  */
static GL_ErrStatus SetComboBoxVisible(GL_PageControls_TypeDef* pTmp, GL_Coordinate_TypeDef objCoordinates)
{
#ifndef USE_2D_OBJECTS
  uint8_t* ptrBitmap;
#endif
  uint32_t index = 0;
  GL_ComboBoxGrp_TypeDef* pThis = (GL_ComboBoxGrp_TypeDef*)(pTmp->objPTR);

  if (!pThis)
  {
    return GL_ERROR;
  }

  pThis->Control_Visible = GL_TRUE;
#ifndef USE_2D_OBJECTS
  ptrBitmap = pThis->ImageUnClickedPTR;
#endif
  GL_SetFont(GL_FONT_SMALL);
  GL_SetBackColor(GL_White);
  GL_SetTextColor(GL_Black);
#ifndef USE_2D_OBJECTS
  GL_DrawButtonBMP( (uint16_t)(objCoordinates.MaxX),
                    (uint16_t)(objCoordinates.MaxX - COMBOBOX_SIZE),
                    (uint8_t)(objCoordinates.MaxY),
                    (uint8_t)(objCoordinates.MinY),
                    ptrBitmap );
#else
  GL_SetTextColor(GL_Black);
  GL_DrawFilledRectangle( (uint16_t)(objCoordinates.MaxX), 
                          (uint16_t)(objCoordinates.MaxX-COMBOBOX_SIZE), 
                          (uint8_t)(objCoordinates.MaxY), 
                          (uint8_t)(objCoordinates.MinY+1), GL_Grey);
#endif

  if (pThis->Secondary == GL_FALSE)
  {
#ifdef USE_2D_OBJECTS
    GL_SetFont(GL_FONT_BIG);
    GL_SetTextColor(GL_Black);
    GL_DisplayAdjStringLine( (uint8_t)(objCoordinates.MinY-1), (uint16_t)(objCoordinates.MaxX-1), "-", GL_TRUE);
    GL_DisplayAdjStringLine( (uint8_t)(objCoordinates.MinY-1), (uint16_t)(objCoordinates.MaxX-4), "-", GL_TRUE);
#endif
    for (; index < pThis->ComboOptionCount && pThis->ComboOptions[index]->IsActive == GL_FALSE; index++)
    {}

    GL_DrawFilledRectangle( (uint16_t)(objCoordinates.MinX-COMBOBOX_SIZE), 
                            (uint16_t)(objCoordinates.MinX -(COMBOBOX_SIZE+(MAX_COMBO_LABEL_LENGTH-2)*FONT_LENGTH)), 
                            (uint8_t)(objCoordinates.MaxY), 
                            (uint8_t)(objCoordinates.MinY+1), GL_White );
    GL_SetFont(GL_FONT_SMALL);
    GL_SetTextColor(GL_Blue);
    if(pThis->ComboOptionCount>0)
    {
      GL_DisplayAdjStringLine( (uint8_t)(objCoordinates.MaxY - 16),
                               (uint16_t)(objCoordinates.MaxX) - (COMBOBOX_SIZE*2 + 4),
                               (uint8_t*)pThis->ComboOptions[index]->label, GL_TRUE);
    }
  }
  else
  {
#ifdef USE_2D_OBJECTS
    GL_SetFont(GL_FONT_BIG);
    GL_SetTextColor(GL_Black);
    GL_DisplayAdjStringLine( (uint8_t)(objCoordinates.MinY), (uint16_t)(objCoordinates.MaxX-3), "+", GL_TRUE);
#endif
    GL_ComboBoxGrp_TypeDef* pTmp = (GL_ComboBoxGrp_TypeDef*)(pThis->PrimaryComboOpt->objPTR);
    for (; index < pTmp->ComboOptionCount-1 && pTmp->ComboOptions[index]->IsActive == GL_FALSE; index++)
    {}
    
    GL_DrawFilledRectangle( (uint16_t)(objCoordinates.MinX), 
                            (uint16_t)(objCoordinates.MinX -((MAX_COMBO_LABEL_LENGTH-2)*FONT_LENGTH)), 
                            (uint8_t)(objCoordinates.MaxY), 
                            (uint8_t)(objCoordinates.MinY+1), 
                            White );
    GL_SetFont(GL_FONT_SMALL);
    GL_SetTextColor(GL_Blue);
    if ( pTmp->ComboOptionCount > 0)
    {
      GL_DisplayAdjStringLine( (uint8_t)(objCoordinates.MaxY - 16),
                               (uint16_t)(objCoordinates.MaxX) - (COMBOBOX_SIZE + 4),
                               (uint8_t*)pTmp->ComboOptions[index]->label,
                               GL_TRUE);
    }
  }

  GL_SetFont(GL_FONT_BIG);
  GL_SetBackColor(White);
  GL_SetTextColor(Blue);

  return GL_OK;
}

/**
  * @brief  Show the Control Object at the specified coordinates.
  * @param  *pThis: Pointer to Object Structure
  * @param  *objCoordinates: Pointer to objCoordinates Structure
  * @retval GL_ErrStatus - GL_OK if successful, GL_ERROR otherwise
  */
static GL_ErrStatus SetCheckboxVisible(GL_PageControls_TypeDef* pTmp, GL_Coordinate_TypeDef objCoordinates)
{
#ifndef USE_2D_OBJECTS
  uint8_t* ptrBitmap;
#endif
  uint8_t n = 0;
  uint32_t label_length = 0;
  GL_Checkbox_TypeDef* pThis = (GL_Checkbox_TypeDef*)(pTmp->objPTR);

  if (!pThis)
  {
    return GL_ERROR;
  }
  pThis->Control_Visible = GL_TRUE;

  if (pThis->IsChecked == GL_FALSE)
  {
#ifndef USE_2D_OBJECTS
    ptrBitmap = pThis->ImageUnCheckedPTR;
#else
    GL_SetTextColor(GL_Black);
    GL_DrawFilledRectangle((uint16_t)(objCoordinates.MaxX), 
                           (uint16_t)(objCoordinates.MinX), 
                           (uint8_t)(objCoordinates.MaxY), 
                           (uint8_t)(objCoordinates.MinY), GL_White);
    GL_SetTextColor(GL_Black);
    GL_DrawFilledRectangle((uint16_t)(objCoordinates.MaxX-1), 
                           (uint16_t)(objCoordinates.MinX+1), 
                           (uint8_t)(objCoordinates.MaxY-1), 
                           (uint8_t)(objCoordinates.MinY+1), GL_White);
#endif
  }
  else
  {
#ifndef USE_2D_OBJECTS
    ptrBitmap = pThis->ImageCheckedPTR;
#else
    GL_SetTextColor(GL_Black);
    GL_DrawFilledRectangle((uint16_t)(objCoordinates.MaxX), 
                           (uint16_t)(objCoordinates.MinX), 
                           (uint8_t)(objCoordinates.MaxY), 
                           (uint8_t)(objCoordinates.MinY), GL_White);
    GL_SetTextColor(GL_Black);
    GL_DrawFilledRectangle((uint16_t)(objCoordinates.MaxX-1), 
                           (uint16_t)(objCoordinates.MinX+1), 
                           (uint8_t)(objCoordinates.MaxY-1), 
                           (uint8_t)(objCoordinates.MinY+1), GL_White);
    GL_SetTextColor(GL_Black);
    GL_DisplayAdjStringLine( (uint8_t)(objCoordinates.MaxY)-19, (uint16_t)(objCoordinates.MaxX-2), "X", GL_TRUE);
#endif
  }
#ifndef USE_2D_OBJECTS
  GL_DrawButtonBMP( (uint16_t)(objCoordinates.MaxX),
                    (uint16_t)(objCoordinates.MinX),
                    (uint8_t)(objCoordinates.MaxY),
                    (uint8_t)(objCoordinates.MinY),
                    ptrBitmap );
#endif
  GL_SetFont(GL_FONT_SMALL);

  label_length = p_strlen(pThis->label);

  if (label_length > 0)
  {
    label_length -= 1;
  }

  for (n = 0; n < label_length; n++)
  {
    GL_DisplayAdjStringLine( (uint8_t)(objCoordinates.MaxY) - 15,
                             (uint16_t)(objCoordinates.MaxX) - (CHECKBOX_SIZE + 1) - n*FONT_LENGTH,
                             (uint8_t*)' ',
                             GL_FALSE);
  }
  GL_SetTextColor(GL_Black);
  GL_DisplayAdjStringLine( (uint8_t)(objCoordinates.MaxY) - 15,
                           (uint16_t)(objCoordinates.MaxX) - (CHECKBOX_SIZE + 1),
                           (uint8_t*)pThis->label,
                           GL_TRUE);
  GL_SetFont(GL_FONT_BIG);

  return GL_OK;
}

/**
  * @brief  Show the Control Object at the specified coordinates.
  * @param  *pThis: Pointer to Object Structure
  * @param  *objCoordinates: Pointer to objCoordinates Structure
  * @retval GL_ErrStatus - GL_OK if successful, GL_ERROR otherwise
  */
static GL_ErrStatus SetIconVisible(GL_PageControls_TypeDef* pTmp, GL_Coordinate_TypeDef objCoordinates)
{
  GL_Icon_TypeDef* pThis = (GL_Icon_TypeDef*)(pTmp->objPTR);
  if (!pThis)
  {
    return GL_ERROR;
  }
  pThis->Control_Visible = GL_TRUE;

  GL_DrawButtonBMP( (uint16_t)(objCoordinates.MaxX),
                    (uint16_t)(objCoordinates.MinX),
                    (uint8_t)(objCoordinates.MaxY),
                    (uint8_t)(objCoordinates.MinY),
                    (uint8_t*)pThis->ImagePTR );

  return GL_OK;
}

/**
  * @brief  Show the Control Object at the specified coordinates.
  * @param  *pThis: Pointer to Object Structure
  * @param  *objCoordinates: Pointer to objCoordinates Structure
  * @retval GL_ErrStatus - GL_OK if successful, GL_ERROR otherwise
  */
static GL_ErrStatus SetSlidebarVisible(GL_PageControls_TypeDef* pTmp, GL_Coordinate_TypeDef objCoordinates)
{
#ifndef USE_2D_OBJECTS
  uint8_t* ptrBmpLeft;
  uint8_t* ptrBmpCenter;
  uint8_t* ptrBmpRight;
  uint8_t* ptrCursorBmp;
#endif
  uint16_t maxX = 0, minX = 0, maxY = 0, minY = 0;
  uint8_t btn_length = 0;
  uint8_t counter = 1;
  __IO uint16_t TempColor = 0;
  uint32_t label_length = 0;
  GL_Slidebar_TypeDef* pThis = (GL_Slidebar_TypeDef*)(pTmp->objPTR);

  if (!pThis)
  {
    return GL_ERROR;
  }
  if ( pThis->Control_Visible != GL_TRUE )
  {
    pThis->Control_Visible = GL_TRUE;
    CursorDraw(Cursor->X, Cursor->Y, CUR_DRAW_BEH);
    if (pThis->Direction == GL_HORIZONTAL)
    {
      SlidebarCursorPreDraw(pTmp, GL_FALSE);
    }
  }

  btn_length = pThis->Length;

  GL_SetFont(GL_FONT_SMALL);

#ifndef USE_2D_OBJECTS
  ptrBmpLeft   = (uint8_t*)SlidebarLeft;
  ptrBmpCenter = pThis->ImageBackgroundPTR;
  ptrBmpRight  = (uint8_t*)SlidebarRight;
  ptrCursorBmp = pThis->ImageCursorPTR;
#endif
  if (pThis->Direction == GL_HORIZONTAL)
  {
    maxX = pTmp->objCoordinates.MaxX;
    maxY = pTmp->objCoordinates.MaxY;
    minY = pTmp->objCoordinates.MinY;
#ifndef USE_2D_OBJECTS
    GL_DrawButtonBMP( maxX, maxX - SLIDEBAR_PIECE_LENGTH, maxY, minY, ptrBmpLeft );
    for ( ; counter < SLIDEBAR_CENTRAL_LENGTH; counter++)
    {
      GL_DrawButtonBMP( maxX - (counter*SLIDEBAR_PIECE_LENGTH),
                        maxX - ((counter + 1)*SLIDEBAR_PIECE_LENGTH),
                        maxY,
                        minY,
                        ptrBmpCenter );
    }
    GL_DrawButtonBMP( maxX - (counter*SLIDEBAR_PIECE_LENGTH),
                      maxX - ((counter + 1)*SLIDEBAR_PIECE_LENGTH),
                      maxY,
                      minY,
                      ptrBmpRight );
#else
    /* Drawing the SlideBar main body */
    GL_SetTextColor(GL_Black);
    GL_DrawFilledRectangle(maxX, maxX - ((SLIDEBAR_CENTRAL_LENGTH+1)*SLIDEBAR_PIECE_LENGTH), 
                           maxY, minY+1, GL_Blue2);
#endif
  }
  else if (pThis->Direction == GL_LEFT_VERTICAL)
  {
    TempColor = GL_TextColor;
    /* Cleaning the Slidebar Main Body contour */
    GL_SetTextColor(GL_White);
    GL_DrawRectangle( pTmp->objCoordinates.MaxX,
                      pTmp->objCoordinates.MinX - 2,
                      pTmp->objCoordinates.MaxY - 1,
                      pTmp->objCoordinates.MinY );
    GL_SetTextColor(TempColor);
    
    maxX = pTmp->objCoordinates.MaxY - SLIDEBAR_PIECE_LENGTH;
    minX = pTmp->objCoordinates.MinY - SLIDEBAR_PIECE_LENGTH;
    maxY = pTmp->objCoordinates.MaxX;
    minY = pTmp->objCoordinates.MinX;
#ifndef USE_2D_OBJECTS
    LCD_Change_Direction(_270_degree);
    LCD_DrawColorBMP( ptrBmpLeft, maxX, minY, SLIDEBAR_HEIGHT, SLIDEBAR_PIECE_LENGTH);

    for ( ; counter < SLIDEBAR_CENTRAL_LENGTH; counter++)
    {
      LCD_DrawColorBMP( ptrBmpCenter,
                        maxX - (counter*SLIDEBAR_PIECE_LENGTH),
                        minY,
                        SLIDEBAR_HEIGHT,
                        SLIDEBAR_PIECE_LENGTH);
    }
    LCD_DrawColorBMP( ptrBmpRight,
                      maxX - (counter*SLIDEBAR_PIECE_LENGTH),
                      minY,
                      SLIDEBAR_HEIGHT,
                      SLIDEBAR_PIECE_LENGTH);
    LCD_Change_Direction(_0_degree);
#else
    /* Drawing the SlideBar main body */
    GL_SetTextColor(GL_Black);
    GL_DrawFilledRectangle(maxY-1, minY-1, 
                           maxX, maxX - ((SLIDEBAR_CENTRAL_LENGTH+1)*SLIDEBAR_PIECE_LENGTH), 
                           GL_Blue2);
#endif
  }
  else if (pThis->Direction == GL_RIGHT_VERTICAL)
  {
    TempColor = GL_TextColor;
    /* Cleaning the Slidebar Main Body contour */
    GL_SetTextColor(GL_White);
    GL_DrawRectangle( pTmp->objCoordinates.MaxX + 1,
                      pTmp->objCoordinates.MinX - 1,
                      pTmp->objCoordinates.MaxY - SLIDEBAR_PIECE_LENGTH - 2,
                      pTmp->objCoordinates.MinY - SLIDEBAR_PIECE_LENGTH - 1 );
    GL_SetTextColor(TempColor);
    
    maxX = pTmp->objCoordinates.MaxY - SLIDEBAR_PIECE_LENGTH;
    minX = pTmp->objCoordinates.MinY - SLIDEBAR_PIECE_LENGTH;
    maxY = pTmp->objCoordinates.MaxX;
    minY = pTmp->objCoordinates.MinX;
#ifndef USE_2D_OBJECTS
    LCD_Change_Direction(_90_degree);
    LCD_DrawColorBMP( ptrBmpLeft, minX, maxY, SLIDEBAR_HEIGHT, SLIDEBAR_PIECE_LENGTH);
    for ( ; counter < SLIDEBAR_CENTRAL_LENGTH; counter++)
    {
      LCD_DrawColorBMP( ptrBmpCenter,
                        minX + (counter*SLIDEBAR_PIECE_LENGTH),
                        maxY,
                        SLIDEBAR_HEIGHT,
                        SLIDEBAR_PIECE_LENGTH);
    }
    LCD_DrawColorBMP( ptrBmpRight,
                      minX + (counter*SLIDEBAR_PIECE_LENGTH),
                      maxY,
                      SLIDEBAR_HEIGHT,
                      SLIDEBAR_PIECE_LENGTH);
    LCD_Change_Direction(_0_degree);
#else
    /* Drawing the SlideBar main body */
    GL_SetTextColor(GL_Black);
    GL_DrawFilledRectangle(maxY, minY, 
                           maxX - SLIDEBAR_OFFSET_LENGTH, 
                           maxX - SLIDEBAR_OFFSET_LENGTH - ((SLIDEBAR_CENTRAL_LENGTH+1)*SLIDEBAR_PIECE_LENGTH), 
                           GL_Blue2);
#endif
  }

  if ( pThis->PrevValue != pThis->CurrentValue && pThis->Direction == GL_HORIZONTAL)
  {
    SlidebarCursorPreDraw(pTmp, GL_FALSE);
    SlidebarCursorPreDraw(pTmp, GL_TRUE);
  }
  if (pThis->Direction == GL_HORIZONTAL)
  {
#ifndef USE_2D_OBJECTS
    GL_DrawButtonBMP( maxX - pThis->CurrentValue - SLIDEBAR_OFFSET_LENGTH,
                      maxX - pThis->CurrentValue - SLIDEBAR_OFFSET_LENGTH - SLIDEBAR_CURSOR_LENGTH,
                      maxY + 1,
                      minY - 1,
                      ptrCursorBmp );
#else
    GL_SetTextColor(GL_Red);
    /* Drawing the SlideBar cursor */
    GL_DrawFilledRectangle(maxX - pThis->CurrentValue - SLIDEBAR_OFFSET_LENGTH, 
                           maxX - pThis->CurrentValue - SLIDEBAR_OFFSET_LENGTH - SLIDEBAR_CURSOR_LENGTH, 
                           maxY+1, minY, GL_Grey);
    GL_SetTextColor(GL_Blue);
#endif
  }
  else if (pThis->Direction == GL_LEFT_VERTICAL)
  {
#ifndef USE_2D_OBJECTS
    LCD_Change_Direction(_270_degree);
    LCD_DrawColorBMP( ptrCursorBmp,
                      maxX - pThis->CurrentValue - SLIDEBAR_OFFSET_LENGTH,
                      minY - 1,
                      SLIDEBAR_HEIGHT + 2,
                      SLIDEBAR_CURSOR_LENGTH);
    LCD_Change_Direction(_0_degree);
#else
    /* Drawing the SlideBar cursor */
    GL_SetTextColor(GL_Red);
    GL_DrawFilledRectangle(maxY, minY-2, 
                           maxX - pThis->CurrentValue - SLIDEBAR_OFFSET_LENGTH -1, 
                           maxX - pThis->CurrentValue - SLIDEBAR_OFFSET_LENGTH - SLIDEBAR_CURSOR_LENGTH, 
                           GL_Grey);
    GL_SetTextColor(GL_Blue);
#endif
  }
  else if (pThis->Direction == GL_RIGHT_VERTICAL)
  {
#ifndef USE_2D_OBJECTS
    LCD_Change_Direction(_90_degree);
    LCD_DrawColorBMP( ptrCursorBmp,
                      minX + pThis->CurrentValue + SLIDEBAR_OFFSET_LENGTH,
                      maxY + 1,
                      SLIDEBAR_HEIGHT + 2,
                      SLIDEBAR_CURSOR_LENGTH);
    LCD_Change_Direction(_0_degree);
#else
    /* Drawing the SlideBar cursor */
    GL_SetTextColor(GL_Red);
    GL_DrawFilledRectangle(maxY+1, minY-1,  
                           minX + pThis->CurrentValue + SLIDEBAR_OFFSET_LENGTH + SLIDEBAR_CURSOR_LENGTH,
                           minX + pThis->CurrentValue + SLIDEBAR_OFFSET_LENGTH + 1, 
                           GL_Grey);
    GL_SetTextColor(GL_Blue);
#endif
  }

  pThis->PrevValue = pThis->CurrentValue;

  /* Only the first 15 (MAX_SLIDE_LABEL_LENGTH) characters of the slidebar label can be printed */
  label_length = p_strlen(pThis->label);
  if ( label_length > 0 )
  {
    counter = 0;
    if (pThis->Direction == GL_HORIZONTAL)
    {
      for (; counter < label_length - 1; counter++)
      {
        GL_DisplayAdjStringLine( maxY + 2,
                                 maxX - ((btn_length - label_length*FONT_LENGTH) / 2) - 3 - counter*FONT_LENGTH,
                                 (uint8_t*)" ",
                                 GL_FALSE);
      }
      GL_DisplayAdjStringLine( maxY + 2,
                               maxX - ((btn_length - label_length*FONT_LENGTH) / 2) - 3,
                               (uint8_t*)pThis->label,
                               GL_TRUE);
    }
    else if (pThis->Direction == GL_LEFT_VERTICAL)
    {
      LCD_Change_Direction(_270_degree);
      for (; counter < label_length - 1; counter++)
      {
        LCD_PrintStringLine( LCD_Width - minY + 2,
                             maxX - ((btn_length - label_length*FONT_LENGTH) / 2) + FONT_LENGTH / 2 - counter*FONT_LENGTH,
                             (uint8_t*)" " );
      }
      LCD_PrintStringLine( LCD_Width - minY + 2,
                           maxX - ((btn_length - label_length*FONT_LENGTH) / 2) + FONT_LENGTH / 2,
                           (uint8_t*)pThis->label );
      LCD_Change_Direction(_0_degree);
    }
    else if (pThis->Direction == GL_RIGHT_VERTICAL)
    {
      LCD_Change_Direction(_90_degree);
      for (; counter < label_length - 1; counter++)
      {
        LCD_PrintStringLine( maxY + 2,
                             maxX - ((btn_length - label_length*FONT_LENGTH) / 2) - (counter + 1)*FONT_LENGTH,
                             (uint8_t*)" " );
      }
      LCD_PrintStringLine( maxY + 2,
                           maxX - ((btn_length - label_length*FONT_LENGTH) / 2) - (counter + 1)*FONT_LENGTH,
                           (uint8_t*)pThis->label );
      LCD_Change_Direction(_0_degree);
    }
  }

  GL_SetFont(GL_FONT_BIG);
  return GL_OK;
}

/**
  * @brief  Show the Control Object at the specified coordinates.
  * @param  *pThis: Pointer to Object Structure
  * @retval GL_ErrStatus - GL_OK if successful, GL_ERROR otherwise
  */
static GL_ErrStatus SetHistogramVisible( GL_PageControls_TypeDef* pTmp, GL_Coordinate_TypeDef objCoordinates )
{
  uint32_t index = 0;
  uint8_t x_step = 0;
  uint16_t height, width, max_x, offset_x, offset_y;
  double base_y, y_step, max_value = 0, y_resolution = 0;
  uint8_t value[7];
  GL_Histogram_TypeDef* pThis = (GL_Histogram_TypeDef*)(pTmp->objPTR);

  if (!pThis)
  {
    return GL_ERROR;
  }
  pThis->Control_Visible = GL_TRUE;
  max_x = LCD_Width - HIST_MARGIN_LENGTH * 5 / 2;
  base_y = LCD_Height - 3 * HIST_MARGIN_LENGTH - 12;

  offset_x = (uint16_t)(max_x + 4);
  offset_y = (uint16_t)(base_y - 4);

  width  = LCD_Width  - 3 * HIST_MARGIN_LENGTH;
  height = LCD_Height - 4 * HIST_MARGIN_LENGTH;

  x_step = width / (pThis->n_points);
  y_step = (height / 10);

  GL_SetTextColor(GL_Black);
  GL_SetFont(GL_FONT_SMALL);
  GL_DrawLine( (uint16_t)base_y, max_x, width, GL_Horizontal );        /* X Axis */
  GL_DrawLine( (uint16_t)(base_y - height), max_x, height, GL_Vertical );  /* Y Axis */
  GL_DisplayAdjStringLine( (uint16_t)(base_y + 6), max_x + 5, "0", GL_TRUE );
  GL_DisplayAdjStringLine( offset_y - 5, offset_x + HIST_MARGIN_LENGTH - 9, "0", GL_TRUE );

  for (; index < (pThis->n_points); index++)
  {
    if ( pThis->points[index] > max_value )
    {
      max_value = pThis->points[index];
    }
  }

  y_resolution = max_value / (height - y_step);

  /* Print the Y axis scale */
  for (index = 0; index < 10; index++)
  {
    GL_SetTextColor(GL_Black);
    GL_SetTextColor(GL_Black);
    GL_DrawLine( (uint16_t)(offset_y - y_step*index), offset_x, 4, GL_Horizontal );
    if ( index == 9)
    {
      GL_DisplayAdjStringLine( (uint16_t)(offset_y - y_step*index - 2 - FONT_LENGTH*2),
                               offset_x + FONT_LENGTH*5,
                               (uint8_t*) pThis->label_Y,
                               GL_TRUE );

      sprintf((char*)value, "%4.1f", max_value);

      if (max_value < 1000)
      {
        GL_DisplayAdjStringLine( (uint16_t)(offset_y - y_step*index - 5),
                                 offset_x + HIST_MARGIN_LENGTH*2,
                                 (uint8_t*) value,
                                 GL_TRUE );
      }
      else
      {
        GL_DisplayAdjStringLine( (uint16_t)(offset_y - y_step*index - 5),
                                 offset_x + FONT_LENGTH + HIST_MARGIN_LENGTH*2,
                                 (uint8_t*) value,
                                 GL_TRUE );
      }
    }
  }

  /* Print the X axis scale */
  for (index = 0; index < (pThis->n_points); index++)
  {
    GL_DrawFilledRectangle( max_x, max_x - x_step + 1, (uint16_t)base_y, (uint16_t)(base_y - (double)((pThis->points[index]) / y_resolution)), GL_Blue );
    max_x = max_x - x_step;
    GL_SetTextColor(GL_Black);
    GL_DrawLine( (uint16_t)base_y, max_x + 1, 4, GL_Vertical );

    if (pThis->n_points <= 15)
    {
      sprintf((char*)value, "%d", index + 1);
      GL_DisplayAdjStringLine( (uint16_t)base_y + 6, max_x + 6, (uint8_t*) value, GL_TRUE );
    }
    else if ( pThis->n_points > 15 && pThis->n_points <= 40)
    {
      if ((index + 1) % 3 == 0)
      {
        sprintf((char*)value, "%d", index + 1);
        GL_DisplayAdjStringLine( (uint16_t)(base_y + 6), max_x + 6, (uint8_t*) value, GL_TRUE );
      }
    }
    else
    {
      if ((index + 1) % 4 == 0)
      {
        sprintf((char*)value, "%d", index + 1);
        GL_DisplayAdjStringLine( (uint16_t)(base_y + 6), max_x + 6, (uint8_t*) value, GL_TRUE );
      }
    }
    if (index == pThis->n_points - 1)
    {
      GL_DisplayAdjStringLine((uint16_t)(base_y + 3 + FONT_LENGTH*2),
                              max_x + FONT_LENGTH*3,
                              (uint8_t*) (pThis->label_X),
                              GL_TRUE );
    }
  }
  GL_SetFont(GL_FONT_BIG);

  return GL_OK;
}

/**
  * @brief  Show the Control Object at the specified coordinates.
  * @param  *pThis: Pointer to Object Structure
  * @retval GL_ErrStatus - GL_OK if successful, GL_ERROR otherwise
  */
static GL_ErrStatus SetGraphChartVisible( GL_PageControls_TypeDef* pTmp, GL_Coordinate_TypeDef objCoordinates )
{
  uint32_t index = 0;
  double height, width, offset_x, offset_y;
  __IO uint16_t signal_color = GL_Red;
  double base_y, y_step, x_step, max_value, y_resolution = 0, max_x;
  uint8_t value[7];
  GL_GraphChart_TypeDef* pThis = (GL_GraphChart_TypeDef*)(pTmp->objPTR);

  if (!pThis)
  {
    return GL_ERROR;
  }
  pThis->Control_Visible = GL_TRUE;

  max_value = 0;
  max_x = LCD_Width - GRAPH_MARGIN_LENGTH * 5 / 2 - FONT_LENGTH;
  base_y = LCD_Height - GRAPH_MARGIN_LENGTH * 7 / 2;

  offset_x = max_x + 4;
  offset_y = base_y;

  width  = LCD_Width  - GRAPH_MARGIN_LENGTH * 4;
  height = LCD_Height - GRAPH_MARGIN_LENGTH * 9 / 2;

  x_step = (double)(width) / (pThis->n_points);
  y_step = (double)(height / 11);

  GL_SetTextColor(GL_Black);
  GL_SetFont(GL_FONT_SMALL);
  GL_DrawLine( (uint16_t)base_y, (uint16_t)max_x, (uint16_t)(width + 2), GL_Horizontal );      /* X Axis */
  GL_DrawLine( (uint16_t)(base_y - height), (uint16_t)max_x, (uint16_t)height, GL_Vertical );  /* Y Axis */
  GL_DisplayAdjStringLine( (uint16_t)(base_y + 6), (uint16_t)(max_x + 5), "0", GL_TRUE ); /* min X value*/
  GL_DisplayAdjStringLine( (uint16_t)(offset_y - 5), (uint16_t)(offset_x + GRAPH_MARGIN_LENGTH - 9), "0", GL_TRUE ); /* min Y value*/

  if (pThis->Background == GL_TRUE)
  {
    /* Display the grid background color (black) */
    GL_SetDisplayWindow((uint16_t)(base_y - 1), (uint16_t)max_x, (uint16_t)height, (uint16_t)(width + 2));
    GL_Clear(GL_Black);
    GL_SetDisplayWindow(LCD_Height - 1, LCD_Width - 1, LCD_Height, LCD_Width);
    signal_color = GL_Yellow;
  }

  for (; index < (pThis->n_points); index++)
  {
    if ( pThis->points[index] > max_value )
      max_value = pThis->points[index];
  }

  y_resolution = (double)max_value / (height - y_step);

  /* Print the Y axis scale */
  for (index = 0; index <= 10; index++)
  {
    if (index > 0)
    {
      GL_SetTextColor(GL_Black);

      GL_DrawLine( (uint16_t)(offset_y - y_step*index),
                   (uint16_t)offset_x,
                   4,
                   GL_Horizontal );

      GL_SetTextColor(GL_Grey);

      GL_DrawLine( (uint16_t)(offset_y - y_step*index),
                   (uint16_t)(offset_x - 4),
                   (uint16_t)(width + 2),
                   GL_Horizontal );
    }
    /* Print the Y axis Label */
    if ( index == 10)
    {
      GL_SetTextColor(GL_Black);
      GL_DisplayAdjStringLine( (uint16_t)(offset_y - y_step*index - 2 - FONT_LENGTH*2),
                               (uint16_t)(offset_x + FONT_LENGTH*9 / 2),
                               (uint8_t*) (pThis->label_Y),
                               GL_TRUE );

      sprintf((char*)value, "%4.1f", max_value);

      if (max_value < 1000)
      {
        GL_DisplayAdjStringLine( (uint16_t)(offset_y - y_step*index - 5),
                                 (uint16_t)(offset_x + GRAPH_MARGIN_LENGTH*2),
                                 (uint8_t*) value,
                                 GL_TRUE );
      }
      else
      {
        GL_DisplayAdjStringLine( (uint16_t)(offset_y - y_step*index - 5),
                                 (uint16_t)(offset_x + FONT_LENGTH + GRAPH_MARGIN_LENGTH*2),
                                 (uint8_t*) value,
                                 GL_TRUE );
      }
    }
  }

  /* Print the X axis scale */
  for (index = 0; index < (pThis->n_points); index++)
  {
    max_x = max_x - x_step;
    GL_SetTextColor(GL_Black);
    GL_DrawLine( (uint16_t)base_y, (uint16_t)(max_x + 1), 4, GL_Vertical );
    if (pThis->n_points <= 15)
    {
      sprintf((char*)value, "%d", index + 1);
      GL_DisplayAdjStringLine( (uint16_t)(base_y + 6), (uint16_t)(max_x + 6), (uint8_t*) value, GL_TRUE );
      GL_SetTextColor(GL_Grey);
      GL_DrawLine( (uint16_t)(base_y - height), (uint16_t)(max_x + 1), (uint16_t)height, GL_Vertical );
    }
    else if ( pThis->n_points > 15 && pThis->n_points <= 40)
    {
      if ((index + 1) % 3 == 0)
      {
        sprintf((char*)value, "%d", index + 1);
        GL_DisplayAdjStringLine( (uint16_t)(base_y + 6), (uint16_t)(max_x + 6), (uint8_t*) value, GL_TRUE );
        GL_SetTextColor(GL_Grey);
        GL_DrawLine( (uint16_t)(base_y - height), (uint16_t)(max_x + 1), (uint16_t)height, GL_Vertical );
      }
    }
    else if ( pThis->n_points > 40 && pThis->n_points <= 70)
    {
      if ((index + 1) % 4 == 0)
      {
        sprintf((char*)value, "%d", index + 1);
        GL_DisplayAdjStringLine( (uint16_t)(base_y + 6), (uint16_t)(max_x + 6), (uint8_t*) value, GL_TRUE );
        GL_SetTextColor(GL_Grey);
        GL_DrawLine( (uint16_t)(base_y - height), (uint16_t)(max_x + 1), (uint16_t)height, GL_Vertical );
      }
    }
    else
    {
      if ((index + 1) % 10 == 0)
      {
        sprintf((char*)value, "%d", index + 1);
        GL_DisplayAdjStringLine( (uint16_t)(base_y + 6), (uint16_t)(max_x + 6), (uint8_t*) value, GL_TRUE );
        GL_SetTextColor(GL_Grey);
        GL_DrawLine( (uint16_t)(base_y - height), (uint16_t)(max_x + 1), (uint16_t)height, GL_Vertical );
      }
    }
    /* Draw the Grid*/
    if (index == 0)
    {
      GL_DrawObliqueLine( (uint16_t)(max_x + x_step),
                          (uint16_t)base_y,
                          (uint16_t)max_x,
                          (uint16_t)(base_y - (double)((pThis->points[index]) / y_resolution)),
                          signal_color );
      GL_DrawObliqueLine( (uint16_t)(max_x + x_step),
                          (uint16_t)(base_y - 1),
                          (uint16_t)max_x,
                          (uint16_t)(base_y - (double)((pThis->points[index]) / y_resolution) - 1),
                          signal_color );
      GL_DrawObliqueLine( (uint16_t)(max_x + x_step - 1),
                          (uint16_t)base_y,
                          (uint16_t)(max_x - 1),
                          (uint16_t)(base_y - (double)((pThis->points[index]) / y_resolution)),
                          signal_color );
    }
    else
    {
      GL_DrawObliqueLine( (uint16_t)(max_x + x_step),
                          (uint16_t)(base_y - (double)((pThis->points[index-1]) / y_resolution)),
                          (uint16_t)max_x,
                          (uint16_t)(base_y - (double)((pThis->points[index]) / y_resolution)),
                          signal_color );
      GL_DrawObliqueLine( (uint16_t)(max_x + x_step),
                          (uint16_t)(base_y - (double)((pThis->points[index-1]) / y_resolution) - 1),
                          (uint16_t)(max_x - 1),
                          (uint16_t)(base_y - (double)((pThis->points[index]) / y_resolution) - 1),
                          signal_color );
      GL_DrawObliqueLine( (uint16_t)(max_x + x_step - 1),
                          (uint16_t)(base_y - (double)((pThis->points[index-1]) / y_resolution)),
                          (uint16_t)(max_x - 1),
                          (uint16_t)(base_y - (double)((pThis->points[index]) / y_resolution)),
                          signal_color );
    }
    /* Print the X axis Label */
    if (index == pThis->n_points - 1)
    {
      GL_SetTextColor(GL_Black);
      GL_DisplayAdjStringLine((uint16_t)(base_y + 3 + FONT_LENGTH*2),
                              (uint16_t)(max_x + FONT_LENGTH*3),
                              (uint8_t*) (pThis->label_X),
                              GL_TRUE );
    }
  }
  GL_SetFont(GL_FONT_BIG);

  return GL_OK;
}

/**
  * @brief  Modify the points array of the GraphChart object
  * @param  *pPage: Pointer to Page Structure
  * @param  ID: Object Identifier
  * @param  data_points[]: The array of points to be plot on the LCD
  * @param  n_points: Number of points to be plot
  * @retval GL_ErrStatus - GL_OK if successful, GL_ERROR otherwise
  */
GL_ErrStatus SetGraphChartPoints( GL_Page_TypeDef* pPage, uint16_t ID, int16_t data_points[], uint8_t n_points )
{
  uint32_t index = 0;
  if (!pPage)
  {
    return GL_ERROR;
  }

  while ( index < pPage->ControlCount ) /* search for the required object */
  {
    if (pPage->PageControls[index]->objType == GL_GRAPH_CHART)
    {
      GL_GraphChart_TypeDef* pTmp;
      pTmp = (GL_GraphChart_TypeDef*)(pPage->PageControls[index]->objPTR);

      if ( pTmp->ID == ID )
      {
        pTmp->n_points = n_points;

        /* Determine the length of the array of points and copy the content into the
         internal points array limiting the number of points to the maximum allowed */
        if ( n_points > MAX_GRAPH_POINTS )
        {
          memcpy(pTmp->points, data_points, MAX_GRAPH_POINTS*2);
          pTmp->n_points = MAX_GRAPH_POINTS;
        }
        else
        {
          memcpy(pTmp->points, data_points, n_points*2);
          pTmp->n_points = n_points;
        }

        return GL_OK;
      }
    }
    index++;
  }
  return GL_ERROR;
}

/**
  * @brief  Modify the points array of the Histogram object
  * @param  *pPage: Pointer to Page Structure
  * @param  ID: Object Identifier
  * @param  data_points[]: The array of points to be plot on the LCD
  * @param  n_points: Number of points to be plot
  * @retval GL_ErrStatus - GL_OK if successful, GL_ERROR otherwise
  */
GL_ErrStatus SetHistogramPoints( GL_Page_TypeDef* pPage, uint16_t ID, int16_t data_points[], uint8_t n_points )
{
  uint32_t index = 0;
  if (!pPage)
  {
    return GL_ERROR;
  }

  while ( index < pPage->ControlCount ) /* search for the required object */
  {
    if (pPage->PageControls[index]->objType == GL_HISTOGRAM)
    {
      GL_Histogram_TypeDef* pTmp;
      pTmp = (GL_Histogram_TypeDef*)(pPage->PageControls[index]->objPTR);

      if ( pTmp->ID == ID )
      {
        pTmp->n_points = n_points;

        /* Determine the length of the array of points and copy the content into the
         internal points array limiting the number of points to the maximum allowed */
        if ( n_points > MAX_HIST_POINTS )
        {
          memcpy(pTmp->points, data_points, MAX_HIST_POINTS*2);
          pTmp->n_points = MAX_HIST_POINTS;
        }
        else
        {
          memcpy(pTmp->points, data_points, n_points*2);
          pTmp->n_points = n_points;
        }

        return GL_OK;
      }
    }
    index++;
  }
  return GL_ERROR;
}


/**
  * @brief  Display/Hide the cursor at current position. In depence to Draw parameter
  *     one of the following action is performed:
  *  - Read behind cursors at current position.
  *  - Draw cursor at current position - display cursor.
  * @param  *pControl: pointer to the cursor structure
  * @param  Draw: specifies if the cursor has to be displayed or hidden
  * @retval None
  */
static void SlidebarCursorPreDraw(GL_PageControls_TypeDef* pControl, GL_bool Draw)
{
  if (pControl->objType == GL_SLIDEBAR )
  {
    uint8_t tmpX = 0x00, tmpY = 0x00;
    uint32_t Temp = 0x00;
    uint32_t Mask = 0;
    uint32_t* CursorImagePointer;
    uint16_t* CursorBehindPointer;
    uint16_t cursor_offset1 = 0, cursor_offset2 = 0;

    /*Read new cursors behind at specified position.*/
    /*Initiate pointers on Cursor bit map image and on array of cursors behind to draw*/
    CursorImagePointer = &SlidebarCursorPointer[3] ;
    CursorBehindPointer = ((GL_Slidebar_TypeDef*)pControl->objPTR)->BehindCursor;

    /* Touching at the right of the cursor*/
    if ( ((GL_Slidebar_TypeDef*)pControl->objPTR)->PrevValue < ((GL_Slidebar_TypeDef*)pControl->objPTR)->CurrentValue )
    {
      cursor_offset1 = ((GL_Slidebar_TypeDef*)pControl->objPTR)->CurrentValue + SLIDEBAR_PIECE_LENGTH + SLIDEBAR_OFFSET_LENGTH + 2;
      cursor_offset2 = ((GL_Slidebar_TypeDef*)pControl->objPTR)->PrevValue + SLIDEBAR_PIECE_LENGTH + SLIDEBAR_OFFSET_LENGTH + 1;
    }

    /* Touching at the left of the cursor*/
    if ( ((GL_Slidebar_TypeDef*)pControl->objPTR)->PrevValue > ((GL_Slidebar_TypeDef*)pControl->objPTR)->CurrentValue )
    {
      cursor_offset1 = ((GL_Slidebar_TypeDef*)pControl->objPTR)->CurrentValue + SLIDEBAR_PIECE_LENGTH + SLIDEBAR_OFFSET_LENGTH;
      cursor_offset2 = ((GL_Slidebar_TypeDef*)pControl->objPTR)->PrevValue + SLIDEBAR_PIECE_LENGTH + SLIDEBAR_OFFSET_LENGTH + 1;
    }

    if (Draw == GL_FALSE)
    {
      /*Repeat for all lines of cursor symbol*/
      for (tmpY = 0x00 ; tmpY <= SlidebarCursorPointer[1]; tmpY++)
      {
        Mask = 0x00;
        /*Repeat for all columns of cursor symbol*/
        for (tmpX = 0x00 ; tmpX < SlidebarCursorPointer[0]; tmpX++)
        {
          if (Mask == 0)
          {
            /*Read next byte of bitmap image and initiate mask*/
            Temp = *CursorImagePointer;
            CursorImagePointer++;
            Mask = 0x80000;
          }
          /*Mask actual byte of cursor bitmap image*/
          if (Temp & Mask)
          {
            /*Read pixel which is going to be behind cursor*/
            *CursorBehindPointer = LCD_GetPixel( (pControl->objCoordinates.MaxY) - SLIDEBAR_HEIGHT + tmpX, pControl->objCoordinates.MaxX + tmpY - cursor_offset1);
            CursorBehindPointer++;
          }
          /*Shift a mask right*/
          Mask >>= 1;
        }
      }
    }

    else
    {
      if ( ((GL_Slidebar_TypeDef*)pControl->objPTR)->PrevValue != ((GL_Slidebar_TypeDef*)pControl->objPTR)->CurrentValue )
      {
        /*Draw last cursors behind at last position - hide cursor*/
        /*Initiate pointers on Cursor bit map image and on array of cursors behind to draw*/
        CursorBehindPointer = ((GL_Slidebar_TypeDef*)pControl->objPTR)->BehindCursor;

        /*Repeat for all lines of cursor symbol*/
        for (tmpY = 0x00 ; tmpY < SlidebarCursorPointer[1]; tmpY++)
        {
          Mask = 0x00;
          /*Repeat for all columns of cursor symbol*/
          for (tmpX = 0x00 ; tmpX < SlidebarCursorPointer[0]; tmpX++)
          {
            if (Mask == 0)
            {
              /*Read next byte of bit map image and initiate mask*/
              Temp = *CursorImagePointer;
              CursorImagePointer++;
              Mask = 0x80000;
            }

            /*Mask actual byte of cursor bitmap image*/
            if (Temp & Mask)
            {
              /*Draw pixel of cursor behind*/
              LCD_PutPixel( pControl->objCoordinates.MaxY - SLIDEBAR_HEIGHT + tmpX, pControl->objCoordinates.MaxX + tmpY - cursor_offset2, *CursorBehindPointer++, SinglePixel);
            }
            /*Shift a mask right*/
            Mask >>= 1;
          }
        }
      }
    }
  }
}

/**
  * @brief  Return the status of the object:
  *            - GL_FALSE: the object is not Hit
  *            - GL_TRUE: the object has been Hit
  *            - GL_NULL represent that no object are there with that ID
  * @param  *pPage: pointer to the Page structure
  * @param  ID: Object Identifier
  * @retval GL_bool - GL_FALSE, GL_TRUE
  */
GL_bool GetObjStatus(GL_Page_TypeDef* pPage, uint16_t ID)
{
  uint32_t index = 0;

  if (pPage)
  {
    while ( index < pPage->ControlCount ) /* search for the required object */
    {
      if (pPage->PageControls[index]->objType == GL_BUTTON)
      {
        GL_Button_TypeDef* pTmp;
        pTmp = (GL_Button_TypeDef*)(pPage->PageControls[index]->objPTR);

        if ( pTmp->ID == ID )
        {
          return pTmp->isObjectTouched;
        }
      }
      else if (pPage->PageControls[index]->objType == GL_SWITCH)
      {
        GL_Switch_TypeDef* pTmp;
        pTmp = (GL_Switch_TypeDef*)(pPage->PageControls[index]->objPTR);

        if ( pTmp->ID == ID )
        {
          return pTmp->isObjectTouched;
        }
      }
      else if (pPage->PageControls[index]->objType == GL_CHECKBOX)
      {
        GL_Checkbox_TypeDef* pTmp;
        pTmp = (GL_Checkbox_TypeDef*)(pPage->PageControls[index]->objPTR);

        if ( pTmp->ID == ID )
        {
          return pTmp->IsChecked;
        }
      }

      else if (pPage->PageControls[index]->objType == GL_ICON)
      {
        GL_Icon_TypeDef* pTmp;
        pTmp = (GL_Icon_TypeDef*)(pPage->PageControls[index]->objPTR);

        if ( pTmp->ID == ID )
        {
          return pTmp->isObjectTouched;
        }
      }
      index++;
    }
  }
  return (GL_bool)GL_NULL;
}

/**
  * @brief  Return the active combo option number
  * @param  *pPage: pointer to the Page structure
  * @param  ID: Object Identifier
  * @retval uint8_t - j: The active combo option index if the object was found
  *                 GL_NULL - if the object was not found
  */
uint8_t GetComboOptionActive(GL_Page_TypeDef* pPage, uint16_t ID)
{
  uint32_t index = 0, combobox_index = 0;

  if (!pPage)
  {
    return GL_NULL;
  }

  while ( index < pPage->ControlCount ) /* search for the required combobox */
  {
    if (pPage->PageControls[index]->objType == GL_COMBOBOX)
    {
      GL_ComboBoxGrp_TypeDef* pTmp;
      pTmp = (GL_ComboBoxGrp_TypeDef*)(pPage->PageControls[index]->objPTR);

      if ( pTmp->ID == ID )
      {
        while ( combobox_index < pTmp->ComboOptionCount )
        {
          if (pTmp->ComboOptions[combobox_index]->IsActive == GL_TRUE)
          {
            return combobox_index + 1;
          }
          combobox_index++;
        }
      }
    }
    index++;
  }
  return GL_NULL;
}

/**
  * @brief  Reset the active combo option to the first one
  * @param  *pPage: pointer to the Page structure
  * @param  ID: Object Identifier
  * @retval GL_ErrStatus - GL_OK if successful, GL_ERROR otherwise
  */
GL_ErrStatus ResetComboOptionActive(GL_Page_TypeDef* pPage, uint16_t ID)
{
  uint32_t index = 0, combobox_index = 0;

  if (pPage)
  {
    while ( index < pPage->ControlCount ) /* search for the required combobox */
    {
      if (pPage->PageControls[index]->objType == GL_COMBOBOX)
      {
        GL_ComboBoxGrp_TypeDef* pTmp;
        pTmp = (GL_ComboBoxGrp_TypeDef*)(pPage->PageControls[index]->objPTR);

        if ( pTmp->ID == ID )
        {
          while ( combobox_index < pTmp->ComboOptionCount )
          {
            if (combobox_index == 0)
            {
              pTmp->ComboOptions[combobox_index]->IsActive = GL_TRUE;
            }
            else
            {
              pTmp->ComboOptions[combobox_index]->IsActive = GL_FALSE;
            }
            combobox_index++;
          }
          return GL_OK;
        }
      }
      index++;
    }
  }
  return GL_ERROR;
}

/**
  * @brief  Return the active combo option number
  * @param  *pPage: pointer to the Page structure
  * @param  ID: Object Identifier
  * @retval uint8_t* - label: The active combo option label if the object was found
  *                 GL_NULL - if the object was not found
  */
const uint8_t* GetComboOptionLabel(GL_Page_TypeDef* pPage, uint16_t ID)
{
  uint32_t index = 0, combobox_index = 0;
  if (!pPage)
  {
    return GL_NULL;
  }

  while ( index < pPage->ControlCount ) /* search for the required combobox */
  {
    if (pPage->PageControls[index]->objType == GL_COMBOBOX)
    {
      GL_ComboBoxGrp_TypeDef* pTmp;
      pTmp = (GL_ComboBoxGrp_TypeDef*)(pPage->PageControls[index]->objPTR);

      if ( pTmp->ID == ID )
        while ( combobox_index < pTmp->ComboOptionCount )
        {
          if (pTmp->ComboOptions[combobox_index]->IsActive == GL_TRUE)
          {
            return pTmp->ComboOptions[combobox_index]->label;
          }
          combobox_index++;
        }
    }
    index++;
  }
  return GL_NULL;
}

/**
  * @brief  Set the label for the active ComboOption
  * @param  *pPage: pointer to the Page structure
  * @param  ID: Object Identifier
  * @param  label: New label of the Active ComboOption Object
  * @retval GL_ErrStatus - GL_OK if successful, GL_ERROR otherwise
  */
GL_ErrStatus SetComboOptionLabel(GL_Page_TypeDef* pPage, uint16_t ID, const uint8_t* label)
{
  uint32_t index = 0, combobox_index = 0;
  if (!pPage)
  {
    return GL_ERROR;
  }

  while ( index < pPage->ControlCount ) /* search for the required combobox */
  {
    if (pPage->PageControls[index]->objType == GL_COMBOBOX)
    {
      GL_ComboBoxGrp_TypeDef* pTmp;
      pTmp = (GL_ComboBoxGrp_TypeDef*)(pPage->PageControls[index]->objPTR);

      if ( pTmp->ID == ID )
      {
        while ( combobox_index < pTmp->ComboOptionCount )
        {
          if (pTmp->ComboOptions[combobox_index]->IsActive == GL_TRUE)
          {
            if (p_strlen(label) > 0)
            {
              GL_SetStringFieldValue(pTmp->ComboOptions[combobox_index]->label, (uint8_t *)label, MAX_COMBO_LABEL_LENGTH);

              return GL_OK;
            }
          }
          combobox_index++;
        }
      }
    }
    index++;
  }
  return GL_ERROR;
}

/**
  * @brief  Set the image pointer of the icon object
  * @param  *pPage: pointer to the Page structure
  * @param  ID: Object Identifier
  * @param  *pImage: Pointer to the image
  * @param  Width: Image Width
  * @param  Height: Image Height
  * @retval GL_ErrStatus - GL_OK if successful, GL_ERROR otherwise
  */
GL_ErrStatus SetIconImage(GL_Page_TypeDef* pPage, uint16_t ID, const uint8_t* pImage, uint16_t Width, uint8_t Height)
{
  uint32_t index = 0;
  if (!pPage)
  {
    return GL_ERROR;
  }
  while ( index < pPage->ControlCount ) /* search for the required Icon object */
  {
    if (pPage->PageControls[index]->objType == GL_ICON)
    {
      GL_Icon_TypeDef* pTmp = NULL;

      pTmp = (GL_Icon_TypeDef*)(pPage->PageControls[index]->objPTR);

      if ( pTmp->ID == ID )
      {
        pTmp->ImagePTR = pImage;
        pTmp->ImageWidth = Width;
        pTmp->ImageHeight = Height;
        return GL_OK;
      }
    }
    index++;
  }
  return GL_ERROR;
}

/**
  * @brief  This function return the object coordinates (X & Y)
  * @param  *pPage: pointer to the Page structure
  * @param  ID: Object Identifier
  * @retval GL_Coordinate_TypeDef - objCoordinates: The Object printing coordinate
  */
static GL_Coordinate_TypeDef GetObjCoordinates(GL_Page_TypeDef* pPage, uint16_t ID)
{
  GL_Coordinate_TypeDef null;
  uint32_t index = 0;
  
  /* Init the structure with zeros */
  memset(&null,0x00,sizeof(GL_Coordinate_TypeDef));

  if (!pPage)
  {
    return null;
  }

  while ( index < pPage->ControlCount ) /* search for the required object */
  {
    if (pPage->PageControls[index]->objType == GL_LABEL)
    {
      GL_Label_TypeDef* pTmp;
      pTmp = (GL_Label_TypeDef*)(pPage->PageControls[index]->objPTR);

      if ( pTmp->ID == ID )
      {
        return pPage->PageControls[index]->objCoordinates;
      }
      index++;
    }
    if (pPage->PageControls[index]->objType == GL_BUTTON)
    {
      GL_Button_TypeDef* pTmp;
      pTmp = (GL_Button_TypeDef*)(pPage->PageControls[index]->objPTR);

      if ( pTmp->ID == ID )
      {
        return pPage->PageControls[index]->objCoordinates;
      }
      index++;
    }
    if (pPage->PageControls[index]->objType == GL_SWITCH)
    {
      GL_Switch_TypeDef* pTmp;
      pTmp = (GL_Switch_TypeDef*)(pPage->PageControls[index]->objPTR);

      if ( pTmp->ID == ID )
      {
        return pPage->PageControls[index]->objCoordinates;
      }
      index++;
    }
    if (pPage->PageControls[index]->objType == GL_CHECKBOX)
    {
      GL_Checkbox_TypeDef* pTmp;
      pTmp = (GL_Checkbox_TypeDef*)(pPage->PageControls[index]->objPTR);

      if ( pTmp->ID == ID )
      {
        return pPage->PageControls[index]->objCoordinates;
      }
      index++;
    }
    if (pPage->PageControls[index]->objType == GL_RADIO_BUTTON)
    {
      GL_RadioButtonGrp_TypeDef* pTmp;
      pTmp = (GL_RadioButtonGrp_TypeDef*)(pPage->PageControls[index]->objPTR);

      if ( pTmp->ID == ID )
      {
        return pPage->PageControls[index]->objCoordinates;
      }
      index++;
    }
    if (pPage->PageControls[index]->objType == GL_COMBOBOX)
    {
      GL_ComboBoxGrp_TypeDef* pTmp;
      pTmp = (GL_ComboBoxGrp_TypeDef*)(pPage->PageControls[index]->objPTR);

      if ( pTmp->ID == ID )
      {
        return pPage->PageControls[index]->objCoordinates;
      }
      index++;
    }
    if (pPage->PageControls[index]->objType == GL_ICON)
    {
      GL_Icon_TypeDef* pTmp;
      pTmp = (GL_Icon_TypeDef*)(pPage->PageControls[index]->objPTR);

      if ( pTmp->ID == ID )
      {
        return pPage->PageControls[index]->objCoordinates;
      }
      index++;
    }
    if (pPage->PageControls[index]->objType == GL_SLIDEBAR)
    {
      GL_Slidebar_TypeDef* pTmp;
      pTmp = (GL_Slidebar_TypeDef*)(pPage->PageControls[index]->objPTR);

      if ( pTmp->ID == ID )
      {
        return pPage->PageControls[index]->objCoordinates;
      }
      index++;
    }
  }
  return null;
}

/**
  * @brief  This function return the object Lenght size (X & Y)
  * @param  *pPageControl: Pointer to PageControls object
  * @retval GL_ObjDimensions_TypeDef - dimensions: Control Object Lenght
  */
static GL_ObjDimensions_TypeDef GetObjSize(GL_PageControls_TypeDef* pPageControl)
{
  GL_ObjDimensions_TypeDef dimensions;

  memset(&dimensions, 0x00, sizeof(GL_ObjDimensions_TypeDef));

  if (pPageControl->objType == GL_BUTTON)
  {
    GL_Button_TypeDef* pTmp;
    pTmp = (GL_Button_TypeDef*)(pPageControl->objPTR);
    dimensions.Length = BUTTON_SLICE_LENGTH * 2 + (BUTTON_SLICE_LENGTH * (p_strlen((pTmp->label)) - 1));
    dimensions.Height  = BUTTON_HEIGHT;
  }
  else if (pPageControl->objType == GL_SWITCH)
  {
    GL_Switch_TypeDef* pTmp;
    pTmp = (GL_Switch_TypeDef*)(pPageControl->objPTR);
    if ( p_strlen((pTmp->label_1)) > p_strlen((pTmp->label_2)) )
      dimensions.Length = BUTTON_SLICE_LENGTH * 2 + (BUTTON_SLICE_LENGTH * (p_strlen((pTmp->label_1)) - 1));
    else
      dimensions.Length = BUTTON_SLICE_LENGTH * 2 + (BUTTON_SLICE_LENGTH * (p_strlen((pTmp->label_2)) - 1));
    dimensions.Height  = BUTTON_HEIGHT;
  }
  else if (pPageControl->objType == GL_CHECKBOX)
  {
    dimensions.Length = CHECKBOX_SIZE;
    dimensions.Height = CHECKBOX_SIZE;
  }
  else if (pPageControl->objType == GL_RADIO_BUTTON)
  {
    dimensions.Length = RBUTTON_OPT_SIZE;
    dimensions.Height = RBUTTON_OPT_SIZE;
  }
  else if (pPageControl->objType == GL_COMBOBOX)
  {
    dimensions.Length = COMBOBOX_SIZE;
    dimensions.Height = COMBOBOX_SIZE;
  }
  else if (pPageControl->objType == GL_SLIDEBAR)
  {
    GL_Slidebar_TypeDef* pTmp;
    pTmp = (GL_Slidebar_TypeDef*)(pPageControl->objPTR);
    if (pTmp->Direction == GL_HORIZONTAL)
    {
      dimensions.Length = pTmp->Length;
      dimensions.Height = pTmp->Width;
    }
    else
    {
      dimensions.Height = pTmp->Length;
      dimensions.Length = pTmp->Width;
    }
  }
  return dimensions;
}

/**
  * @brief  This function Enable or Disable (Hide) the Page.
  * @param  *pPage: Pointer to Page Structure
  * @param  bVal: GL_TRUE Enable / GL_FALSE Disable (Hide) the Page.
  * @retval GL_ErrStatus - GL_OK if successful, GL_ERROR otherwise
  */
static GL_ErrStatus SetPage(GL_Page_TypeDef* pPage, GL_bool bVal)
{
  if (!pPage)
  {
    return GL_ERROR;
  }
  pPage->Page_Active = bVal;
  return GL_OK;
}

/**
  * @brief  This function Show or Hide the Page.
  * @param  *pPage: Pointer to Page Structure
  * @param  bVal: GL_TRUE Show / GL_FALSE Hide the Page.
  * @retval GL_ErrStatus - GL_OK if successful, GL_ERROR otherwise
  */
GL_ErrStatus ShowPage(GL_Page_TypeDef* pPage, GL_bool bVal)
{
  uint32_t i = 0;
  if (!pPage)
  {
    return GL_ERROR;
  }
  pPage->Page_Visible = bVal;
  pPage->SetPage(pPage, bVal);
  if (bVal == GL_TRUE)
  {
    while ( i < pPage->ControlCount ) /* search for the required button */
    {
      pPage->PageControls[i]->SetObjVisible( pPage->PageControls[i], pPage->PageControls[i]->objCoordinates);
      i++;
    }
    /*Read behind the cursor at specified position*/
    CursorDraw(Cursor->X, Cursor->Y, CUR_READ_BEH);
  }
  else
  {
    GL_Clear(GL_White);
    /* GL_Clear(GL_Cyan); */
    CursorDraw(Cursor->X, Cursor->Y, CUR_READ_BEH);
  }
  pPage->SetPage(pPage, bVal);

  return GL_OK;
}

/**
  * @brief  Refresh the Page if the status of any object was changed.
  * @param  *pPage: Pointer to Page Structure
  * @retval None
  */
void RefreshPage(GL_Page_TypeDef* pPage)
{
  ShowPage( pPage, GL_FALSE );
  ShowPage( pPage, GL_TRUE );
}

/**
  * @brief  Refresh the visualization of a Page Control Object.
  * @param  *pPage: Pointer to Page Structure
  * @param  *pPage: Pointer to Page Structure
  * @retval GL_ErrStatus - GL_OK if successful, GL_ERROR otherwise
  */
GL_ErrStatus RefreshPageControl( GL_Page_TypeDef* pPage, uint16_t ID)
{
  uint32_t index = 0;

  if (!pPage)
  {
    return GL_ERROR;
  }

  while ( index < pPage->ControlCount ) /* search for the required object */
  {
    if ( pPage->PageControls[index]->ID == ID )
    {
      pPage->PageControls[index]->SetObjVisible( pPage->PageControls[index], pPage->PageControls[index]->objCoordinates);
      return GL_OK;
    }
    index++;
  }
  return GL_ERROR;
}

/**
  * @brief  ChangePage the current Page showed on the LCD.
  * @param  *pPageOld: Pointer to Page Structure of the current page
  * @param  *pPageNew: Pointer to Page Structure of the new page
  * @retval None
  */
void ChangePage(GL_Page_TypeDef* pPageOld, GL_Page_TypeDef* pPageNew)
{
  ShowPage( pPageOld, GL_FALSE );
  ShowPage( pPageNew, GL_TRUE );
}

/**
  * @brief  Manage the the process of printing and changing behaviour of a touched
  *         control object
  * @param  *pControl: Pointer to PageControl object
  * @retval None
  */
static void CallPreEvents(GL_PageControls_TypeDef* pControl)
{
  uint32_t index = 0;
  uint32_t active_index = 0; /* needed for ComboBox */
#ifndef USE_2D_OBJECTS
  uint8_t* ptrBitmap = GL_NULL;
#endif  
  GL_Coordinate_TypeDef tmpCoord;
  void* pTmp;
  GL_RadioButtonGrp_TypeDef* pTmpGrp = GL_NULL;
  GL_ComboBoxGrp_TypeDef* pTmpComboGrp = GL_NULL;

  switch (pControl->objType)
  {
    case GL_BUTTON:
      pTmp = (GL_Button_TypeDef*)(pControl->objPTR);
      ((GL_Button_TypeDef*)(pTmp))->isObjectTouched = GL_TRUE;
      pControl->SetObjVisible(pControl, pControl->objCoordinates);
      while (index < 800000)
        index++;
      ((GL_Button_TypeDef*)(pTmp))->isObjectTouched = GL_FALSE;
      pControl->SetObjVisible(pControl, pControl->objCoordinates);
      break;
    case GL_CHECKBOX:
      pTmp = (GL_Checkbox_TypeDef*)(pControl->objPTR);
      if ( ((GL_Checkbox_TypeDef*)(pTmp))->IsChecked == GL_TRUE )
        ((GL_Checkbox_TypeDef*)(pTmp))->IsChecked = GL_FALSE;
      else
        ((GL_Checkbox_TypeDef*)(pTmp))->IsChecked = GL_TRUE;
      pControl->SetObjVisible(pControl, pControl->objCoordinates);
      break;
    case GL_SWITCH:
      pTmp = (GL_Switch_TypeDef*)(pControl->objPTR);
      if ( ((GL_Switch_TypeDef*)(pTmp))->isObjectTouched == GL_TRUE )
        ((GL_Switch_TypeDef*)(pTmp))->isObjectTouched = GL_FALSE;
      else
        ((GL_Switch_TypeDef*)(pTmp))->isObjectTouched = GL_TRUE;
      pControl->SetObjVisible(pControl, pControl->objCoordinates);
      break;
    case GL_RADIO_BUTTON:
      pTmpGrp = ((GL_RadioOption_TypeDef*)(pControl->objPTR))->RadioButtonGrp;
      pTmp = (GL_RadioOption_TypeDef*)(pControl->objPTR);
      if ( ((GL_RadioOption_TypeDef*)(pTmp))->IsChecked == GL_FALSE )
      {
        for (index = 0; index < pTmpGrp->RadioOptionCount; index++)
        {
          ( (GL_RadioOption_TypeDef*)(pTmpGrp->RadioOptions[index]->objPTR))->IsChecked = GL_FALSE;
        }
        ((GL_RadioOption_TypeDef*)(pTmp))->IsChecked = GL_TRUE;
        for (index = 0; index < (pTmpGrp->RadioOptionCount); index++)
        {
          pControl = pTmpGrp->RadioOptions[index];
          pControl->SetObjVisible(pControl, pControl->objCoordinates);
        }
      }
      break;
    case GL_COMBOBOX:
      pTmpComboGrp = (GL_ComboBoxGrp_TypeDef*)(pControl->objPTR);
#ifndef USE_2D_OBJECTS
      ptrBitmap = pTmpComboGrp->ImageClickedPTR;
      GL_DrawButtonBMP( (uint16_t)(pControl->objCoordinates.MaxX),
                        (uint16_t)(pControl->objCoordinates.MaxX - COMBOBOX_SIZE),
                        (uint8_t)(pControl->objCoordinates.MaxY),
                        (uint8_t)(pControl->objCoordinates.MinY), ptrBitmap );
      GL_Delay(20);
#else
      GL_DrawFilledRectangle( (uint16_t)(pControl->objCoordinates.MaxX), 
                              (uint16_t)(pControl->objCoordinates.MaxX-COMBOBOX_SIZE), 
                              (uint8_t)(pControl->objCoordinates.MaxY), 
                              (uint8_t)(pControl->objCoordinates.MinY+1), GL_Cyan);
#endif
      
      if (((GL_ComboBoxGrp_TypeDef*)(pTmpComboGrp))->Secondary == GL_FALSE)
      {
#ifdef USE_2D_OBJECTS
        GL_SetTextColor(GL_Grey);
        GL_DisplayAdjStringLine( (uint8_t)(pControl->objCoordinates.MinY-1), (uint16_t)(pControl->objCoordinates.MaxX-1), "-", GL_TRUE);
        GL_DisplayAdjStringLine( (uint8_t)(pControl->objCoordinates.MinY-1), (uint16_t)(pControl->objCoordinates.MaxX-4), "-", GL_TRUE);
        GL_Delay(20);
#endif
        for (index = 0; index < ((GL_ComboBoxGrp_TypeDef*)(pTmpComboGrp))->ComboOptionCount; index++)
        {
          if ( ((GL_ComboBoxGrp_TypeDef*)(pTmpComboGrp))->ComboOptions[index]->IsActive == GL_TRUE )
          {
            active_index = index;
          }
          ((GL_ComboBoxGrp_TypeDef*)(pTmpComboGrp))->ComboOptions[index]->IsActive = GL_FALSE;
        }
        if (active_index == 0)
          pTmpComboGrp->ComboOptions[active_index]->IsActive = GL_TRUE;
        if (active_index > 0)
          pTmpComboGrp->ComboOptions[active_index-1]->IsActive = GL_TRUE;
        pControl->SetObjVisible(pControl, pControl->objCoordinates);
      }
      else
      {
#ifdef USE_2D_OBJECTS
        GL_SetTextColor(GL_Grey);
        GL_DisplayAdjStringLine( (uint8_t)(pControl->objCoordinates.MinY),
                                 (uint16_t)(pControl->objCoordinates.MaxX-3), 
                                 "+", 
                                 GL_TRUE);
        GL_Delay(20);
#endif
        GL_ComboBoxGrp_TypeDef* pTmpComboGrp2 = ((GL_ComboBoxGrp_TypeDef*)(pControl->objPTR))->PrimaryComboOpt->objPTR;
        for (index = 0; index < pTmpComboGrp2->ComboOptionCount; index++)
        {
          if ( pTmpComboGrp2->ComboOptions[index]->IsActive == GL_TRUE )
          {
            active_index = index;
          }
          pTmpComboGrp2->ComboOptions[index]->IsActive = GL_FALSE;
        }
    
        if ( active_index == pTmpComboGrp2->ComboOptionCount - 1 )
          pTmpComboGrp2->ComboOptions[active_index]->IsActive = GL_TRUE;
        else if ( active_index < pTmpComboGrp2->ComboOptionCount - 1 )
          pTmpComboGrp2->ComboOptions[active_index+1]->IsActive = GL_TRUE;

        pControl->SetObjVisible(pControl, pControl->objCoordinates);
      }
      break;

    case GL_SLIDEBAR:
      pTmp = (GL_Slidebar_TypeDef*)(pControl->objPTR);

      /* GL_ObjDimensions_TypeDef tmpSize; */
      tmpCoord = pControl->objCoordinates;
      if ( ((GL_Slidebar_TypeDef*)pTmp)->Direction == GL_HORIZONTAL )
      {
        /* Click on the left of the cursor - decreasing the value */
        if ( u32_TSXCoordinate >= (tmpCoord.MaxX - ((GL_Slidebar_TypeDef*)pTmp)->CurrentValue - SLIDEBAR_OFFSET_LENGTH) && ((GL_Slidebar_TypeDef*)pTmp)->CurrentValue > 0 )
        {
          /* Calculate a new cursor position from new value */
          ((GL_Slidebar_TypeDef*)pTmp)->PrevValue = ((GL_Slidebar_TypeDef*)pTmp)->CurrentValue;
          ((GL_Slidebar_TypeDef*)pTmp)->CurrentValue = ((GL_Slidebar_TypeDef*)pTmp)->CurrentValue - 5;
        }

        /* Click on the right of the cursor - increasing value */
        else if ( u32_TSXCoordinate <= (tmpCoord.MinX - ((GL_Slidebar_TypeDef*)pTmp)->CurrentValue - SLIDEBAR_CURSOR_LENGTH - SLIDEBAR_OFFSET_LENGTH) && ((GL_Slidebar_TypeDef*)pTmp)->CurrentValue < 100)
        {
          /* Calculate a new cursor position from new value */
          ((GL_Slidebar_TypeDef*)pTmp)->PrevValue = ((GL_Slidebar_TypeDef*)pTmp)->CurrentValue;
          ((GL_Slidebar_TypeDef*)pTmp)->CurrentValue = ((GL_Slidebar_TypeDef*)pTmp)->CurrentValue + 5;
        }
      }
      else if ( ((GL_Slidebar_TypeDef*)pTmp)->Direction == GL_LEFT_VERTICAL )
      {
        /* Click on the LEFT of the cursor - decreasing the value */
        if ( u32_TSYCoordinate > (tmpCoord.MaxY - ((GL_Slidebar_TypeDef*)pTmp)->CurrentValue - SLIDEBAR_OFFSET_LENGTH) && ((GL_Slidebar_TypeDef*)pTmp)->CurrentValue > 0 )
        {
          /* Calculate a new cursor position from new value */
          ((GL_Slidebar_TypeDef*)pTmp)->PrevValue = ((GL_Slidebar_TypeDef*)pTmp)->CurrentValue;
          ((GL_Slidebar_TypeDef*)pTmp)->CurrentValue = ((GL_Slidebar_TypeDef*)pTmp)->CurrentValue - 5;
        }

        /* Click on the RIGHT of the cursor - increasing value */
        else if ( u32_TSYCoordinate < (tmpCoord.MaxY - ((GL_Slidebar_TypeDef*)pTmp)->CurrentValue - SLIDEBAR_CURSOR_LENGTH*2 - SLIDEBAR_OFFSET_LENGTH) && ((GL_Slidebar_TypeDef*)pTmp)->CurrentValue < 100)
        {
          /* Calculate a new cursor position from new value */
          ((GL_Slidebar_TypeDef*)pTmp)->PrevValue = ((GL_Slidebar_TypeDef*)pTmp)->CurrentValue;
          ((GL_Slidebar_TypeDef*)pTmp)->CurrentValue = ((GL_Slidebar_TypeDef*)pTmp)->CurrentValue + 5;
        }
      }
      else if ( ((GL_Slidebar_TypeDef*)pTmp)->Direction == GL_RIGHT_VERTICAL )
      {
        /* Click on the RIGHT of the cursor - decreasing the value */
        if ( u32_TSYCoordinate >= (tmpCoord.MinY + ((GL_Slidebar_TypeDef*)pTmp)->CurrentValue + SLIDEBAR_CURSOR_LENGTH + SLIDEBAR_OFFSET_LENGTH) && ((GL_Slidebar_TypeDef*)pTmp)->CurrentValue < 100 )
        {
          /* Calculate a new cursor position from new value */
          ((GL_Slidebar_TypeDef*)pTmp)->PrevValue = ((GL_Slidebar_TypeDef*)pTmp)->CurrentValue;
          ((GL_Slidebar_TypeDef*)pTmp)->CurrentValue = ((GL_Slidebar_TypeDef*)pTmp)->CurrentValue + 5;
        }

        /* Click on the LEFT of the cursor - increasing value */
        else if ( u32_TSYCoordinate <= (tmpCoord.MinY + ((GL_Slidebar_TypeDef*)pTmp)->CurrentValue - SLIDEBAR_OFFSET_LENGTH) && ((GL_Slidebar_TypeDef*)pTmp)->CurrentValue > 0)
        {
          /* Calculate a new cursor position from new value */
          ((GL_Slidebar_TypeDef*)pTmp)->PrevValue = ((GL_Slidebar_TypeDef*)pTmp)->CurrentValue;
          ((GL_Slidebar_TypeDef*)pTmp)->CurrentValue = ((GL_Slidebar_TypeDef*)pTmp)->CurrentValue - 5;
        }
      }
      else
        break;
      CursorDraw(Cursor->X, Cursor->Y, CUR_READ_BEH);
      pControl->SetObjVisible(pControl, pControl->objCoordinates);
      break;

    default:
      break;
  }
}

/**
  * @brief  Depending on the Coordinates of the Touch Screen pressed, this
  *         function does the processing and executes the proper function.
  * @param  None
  * @retval None
  */
void ProcessInputData(void)
{
  uint16_t p_index, c_index;
  GL_Coordinate_TypeDef tmpCoord;
  GL_ObjDimensions_TypeDef tmpSize;

  if ( touch_done || joy_done )
  {
    if (vu8_gSleepState == 1)
    {
      GL_BackLightSwitch(GL_ON);
      vu8_gSleepState = 0;
    }
    else if ((vu8_gTouchEnable == 1) && (vu8_gSleepState == 0))
    {
      vu32_gTimeOutCount = 0;
      for ( p_index = 0; p_index < PageCount; p_index++)
      {
        if ( PagesList[p_index]->Page_Active == GL_TRUE )
        {
          for ( c_index = 0; c_index < PagesList[p_index]->ControlCount; c_index++ )
          {
            tmpCoord = PagesList[p_index]->PageControls[c_index]->objCoordinates;
            tmpSize = GetObjSize(PagesList[p_index]->PageControls[c_index]);
            if (touch_done)
            {
              if ( CompareCoordinates( tmpCoord.MaxX, tmpCoord.MaxX - tmpSize.Length + 1, tmpCoord.MaxY, tmpCoord.MaxY - tmpSize.Height ) )
              {
                CursorDraw(Cursor->X, Cursor->Y, CUR_DRAW_BEH);
                CallPreEvents(PagesList[p_index]->PageControls[c_index]);
                CallEvent(PagesList[p_index]->PageControls[c_index]);
                CursorDraw(Cursor->X, Cursor->Y, CUR_READ_DRAW_CUR);
                u32_TSYCoordinate = 0;
                touch_done = 0;
                GL_Delay(15);
                break;
              }
            }
            else
            {
              if (joy_done)
              {
                if ( CompareJoyCoordinates( (uint16_t)tmpCoord.MaxX, (uint16_t)(tmpCoord.MaxX - tmpSize.Length), (uint8_t)tmpCoord.MaxY, (uint8_t)(tmpCoord.MaxY - tmpSize.Height) ) )
                {
                  CursorDraw(Cursor->X, Cursor->Y, CUR_DRAW_BEH);
                  CallPreEvents(PagesList[p_index]->PageControls[c_index]);
                  CallEvent(PagesList[p_index]->PageControls[c_index]);
                  CursorDraw(Cursor->X, Cursor->Y, CUR_READ_DRAW_CUR);
                  u32_TSYCoordinate = 0;
                  joy_done = 0;
                  GL_Delay(20);
                  break;
                }
              }
            }
          }
        }
      }
    }
    touch_done = 0;
    joy_done = 0;
  }
}

/**
  * @brief  "Do nothing" function
  * @param  None
  * @retval None
  */
void NullFunc(void)
{}

/**
  * @brief  Run the proper function associated to the touch event of a specific
  *         control object
  * @param  *pControl: pointer to PageControl object
  * @retval None
  */
static void CallEvent(GL_PageControls_TypeDef* pControl)
{
  uint32_t index = 0;
  void* pTmp;

  switch (pControl->objType)
  {
    case GL_BUTTON:
      pTmp = (GL_Button_TypeDef*)(pControl->objPTR);
      ((GL_Button_TypeDef*)pTmp)->EventHandler();
      break;
    case GL_CHECKBOX:
      pTmp = (GL_Checkbox_TypeDef*)(pControl->objPTR);
      ((GL_Checkbox_TypeDef*)pTmp)->EventHandler();
      break;
    case GL_SWITCH:
      pTmp = (GL_Switch_TypeDef*)(pControl->objPTR);
      ((GL_Switch_TypeDef*)pTmp)->EventHandler();
      break;
    case GL_ICON:
      pTmp = (GL_Icon_TypeDef*)(pControl->objPTR);
      ((GL_Icon_TypeDef*)pTmp)->EventHandler();
      break;
    case GL_RADIO_BUTTON:
      pTmp = (GL_RadioOption_TypeDef*)(pControl->objPTR);
      ((GL_RadioOption_TypeDef*)pTmp)->EventHandler();
      break;
    case GL_COMBOBOX:
      pTmp = pControl->objPTR;
      if (((GL_ComboBoxGrp_TypeDef*)(pTmp))->Secondary == GL_FALSE )
      {
        for (; index < ((GL_ComboBoxGrp_TypeDef*)(pTmp))->ComboOptionCount-1 && ((GL_ComboBoxGrp_TypeDef*)(pTmp))->ComboOptions[index]->IsActive == GL_FALSE; index++)
        {}
        ((GL_ComboBoxGrp_TypeDef*)(pTmp))->ComboOptions[index]->EventHandler();
      }
      else
      {
        GL_ComboBoxGrp_TypeDef* pPrimaryOption = ((GL_ComboBoxGrp_TypeDef*)(((GL_ComboBoxGrp_TypeDef*)(pTmp))->PrimaryComboOpt->objPTR));
        for (; index < pPrimaryOption->ComboOptionCount-1 && pPrimaryOption->ComboOptions[index]->IsActive == GL_FALSE; index++)
        {}
        pPrimaryOption->ComboOptions[index]->EventHandler();
      }
      break;
    case GL_SLIDEBAR:
      pTmp = (GL_Slidebar_TypeDef*)(pControl->objPTR);
      ((GL_Slidebar_TypeDef*)pTmp)->EventHandler();
      break;
    default:
      break;
  }
}

/**
  * @brief  Set LCD Panel Resolution
  * @param  Lcd_Width: Width of the LCD Panel
  * @param  Lcd_Height: Height of the LCD Panel
  * @retval None
  */
void Set_LCD_Resolution( uint16_t Lcd_Width, uint16_t Lcd_Height )
{
  LCD_Width  = Lcd_Width;
  LCD_Height = Lcd_Height;
}

/**
  * @brief  This function checks the coordinates of the touch screen pressed
  *         converted in LCD ones and depending on the level of the menu
  *         checks if the press is valid or not.
  * @param  u16_XMax: Maximum X coordinate for valid touch
  * @param  u16_XMin: Minimum X coordinate for valid touch
  * @param  u8_YMax: Maximum Y coordinate for valid touch
  * @param  u8_YMin: Minimum Y coordinate for valid touch
  * @retval valid touch detected (1) or not-valid touch detected (0).
  */
uint8_t CompareCoordinates(uint16_t u16_XMax, uint16_t u16_XMin, uint16_t u16_YMax, uint16_t u16_YMin)
{
  if (( u32_TSXCoordinate <= u16_XMin) || ( u32_TSXCoordinate >= u16_XMax - 5))
  {
    ValidTouchDetected = 0;
    return 0;
  }
  else if (( u32_TSYCoordinate < u16_YMin) || ( u32_TSYCoordinate > u16_YMax))
  {
    ValidTouchDetected = 0;
    return 0;
  }
  else
  {
    ValidTouchDetected = 1;
    return 1;
  }
}

/**
  * @brief  This function checks the coordinates of the Joystick Pointer
  *         with the graphics object ones printed on the LCD and depending on
  *         the level of the menu checks if the press is valid or not.
  * @param  u16_XMax: Maximum X coordinate for valid press
  * @param  u16_XMin: Minimum X coordinate for valid press
  * @param  u8_YMax: Maximum Y coordinate for valid press
  * @param  u8_YMin: Minimum Y coordinate for valid press
  * @retval valid press detected (1) or not-valid press detected (0).
  */
uint8_t CompareJoyCoordinates(uint16_t u16_XMax, uint16_t u16_XMin, uint16_t u16_YMax, uint16_t u16_YMin)
{
  if ( ( u32_TSXCoordinate < u16_XMin) || (u32_TSXCoordinate > u16_XMax) )
  {
    ValidTouchDetected = 0;
    return 0;
  }
  else if ( ( u32_TSYCoordinate < u16_YMin) || ( u32_TSYCoordinate > u16_YMax) )
  {
    ValidTouchDetected = 0;
    return 0;
  }
  else
  {
    ValidTouchDetected = 1;
    return 1;
  }
}

/**
  * @brief  Switches the backlightOFF when power save mode is
  *         enabled and no event is detected for long time.
  * @param  None
  * @retval None
  */
void TimeOutCalculate(void)
{
  if (vu8_gSleepState == 0)
  {
    if (vu8_gPowerSaveOption == 1)
    {
      vu32_gTimeOutCount++;
      if (vu32_gTimeOutCount > TIMEOUT)
      {
        GL_BackLightSwitch(GL_OFF);
        vu8_gSleepState = 1;
        vu32_gTimeOutCount = 0;
      }
    }
  }
}

/**
  * @brief  Inserts a delay time.
  * @param  nCount: specifies the delay time length (time base 10 ms).
  * @retval None
  */
void GL_Delay(uint32_t nTime)
{
  TimingDelay = nTime;

  while (TimingDelay != 0);

}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  {
    TimingDelay--;
  }
}

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

