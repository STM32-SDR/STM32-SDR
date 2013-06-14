/**
  ******************************************************************************
  * @file    cursor.c
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    11-July-2011
  * @brief   Application - cursor routines
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

/* Includes -----------------------------------------------------------------------------------------------------*/
#include <stdlib.h>
#include "JoyHal.h"
#include "LcdHal.h"
#include "cursor.h"

/** @addtogroup Embedded_GUI_Library
  * @{
  */

/** @defgroup Cursor 
  * @brief Cursor main functions
  * @{
  */ 

/* External variables --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/**
  * @brief  Cursor main structure
  */
Cursor_TypeDef* Cursor;

/**
  * @brief  Header and bitmap image of cursor's symbol
  */
static uint8_t CursorPointer[] =
{
  0x08,   /*Height of cursor symbol*/
  0x0B,   /*Width of cursor symbol*/
  0x35,   /*Count of pixels of cursor symbol*/
  0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF, 0xFF, 0xF8, 0xD8
};

/**
  * @brief  touch or cursor coordinate
  */
__IO uint32_t u32_TSXCoordinate;
__IO uint32_t u32_TSYCoordinate;



/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/** @defgroup Cursor_Private_Functions
  * @{
  */

/**
  * @brief  Shift cursor with defined step (CUR_STEP).
  * @param  CursorEvent: specifies direction of shift.
  *   This parameter can be one of the following values:
  *     @arg  CUR_DOWN: shift cursor down by step.
  *     @arg  CUR_UP: shift cursor up by step.
  *     @arg  CUR_RIGHT: shift cursor right by step.
  *     @arg  CUR_LEFT: shift cursor left by step.
  * @retval None
  */
void CursorStepMove(uint8_t CursorEvent)
{
  uint16_t tmpY, tmpX;

  /*Load current cursor position*/
  tmpY = Cursor->Y;
  tmpX = Cursor->X;

  /*Step cursor position with regard to desired direction.*/
  switch (CursorEvent)
  {
    case CUR_DOWN:
      tmpY -= (tmpY >= CUR_STEP) ? (CUR_STEP) : (tmpY);
      break;
    case CUR_LEFT:
      tmpY += (LCD_Width - tmpY - CursorPointer[1] >= CUR_STEP) ? (CUR_STEP) : (LCD_Width - tmpY - CursorPointer[1]);
      break;
    case CUR_RIGHT:
      tmpX += (LCD_Height - tmpX - CursorPointer[0] >= CUR_STEP) ? (CUR_STEP) : (LCD_Height - tmpX - CursorPointer[0]);
      break;
    case CUR_UP:
      tmpX -= (tmpX >= CUR_STEP) ? (CUR_STEP) : (tmpX);
      break;
    default:
      break;
  }

  /*Pass just calculated position to call what handle this change*/
  CursorPosition(tmpX, tmpY);
  GL_Delay(3);
}

/**
  * @brief  Handle cursor position and cursor events. Display cursor at new position and create message.
  * @param  Xpos: specifies cursor X position.
  * @param  Ypos: specifies cursor Y position.
  * @retval None
  */
void CursorPosition(uint16_t Xpos, uint16_t Ypos)
{
  CursorDraw(Xpos, Ypos, CUR_DRAW_BEH);
  CursorDraw(Xpos, Ypos, CUR_READ_DRAW_CUR);
}

/**
  * @brief  Read state of joystick. Pressed key is announced to CursorStepMove
  *     to eventually calculate move of cursor.
  * @param  mode: IOEXP_MODE or POLLING_MODE
  * @retval None
  */
void CursorReadJoystick(JOY_ReadMode mode)
{
  uint32_t joystick_move = 0;
  joystick_move = GL_JoyStickState(mode);

  if (GL_GPIO_ReadInputDataBit(pBtnHwParam.BTN_Port,
                                pBtnHwParam.BTN_Pin) == pBtnHwParam.BTN_Pin_Active)
  {
    joystick_move = GL_JOY_CENTER;
  }

  switch (joystick_move)
  {
      /* "Up" key is pressed */
    case GL_JOY_UP:
      u32_TSXCoordinate = Cursor->Y;
      u32_TSYCoordinate = Cursor->X;
      CursorStepMove(CUR_UP);
      break;

      /* "Down" key is pressed */
    case GL_JOY_RIGHT:
      u32_TSXCoordinate = Cursor->Y;
      u32_TSYCoordinate = Cursor->X;
      CursorStepMove(CUR_DOWN);
      break;

      /* "Right" key is pressed */
    case GL_JOY_DOWN:
      u32_TSXCoordinate = Cursor->Y;
      u32_TSYCoordinate = Cursor->X;
      CursorStepMove(CUR_RIGHT);
      break;

      /* "Left" key is pressed */
    case GL_JOY_LEFT:
      u32_TSXCoordinate = Cursor->Y;
      u32_TSYCoordinate = Cursor->X;
      CursorStepMove(CUR_LEFT);
      break;

      /* "Sel" key is pressed */
    case GL_JOY_CENTER:
      /*Load current cursor position*/
      u32_TSXCoordinate = Cursor->Y;
      u32_TSYCoordinate = Cursor->X;
      joy_done = 1; /* Run Touch Event Routine */
      break;

    case GL_JOY_NONE:
      break;

    default:
      break;
  }
}

/**
  * @brief  Initiate the cursor including allocating memory for structure and behind array.
  * @param  PointerMark
  * @retval Status of initializing: Return ERROR if allocating failed.
  */
ErrorStatus CursorInit(uint8_t *PointerMark)
{
  /*Allocate memory for cursor structure*/
  Cursor = (Cursor_TypeDef *)malloc(sizeof(Cursor_TypeDef));

  if (Cursor == (Cursor_TypeDef*)GL_NULL)
  {
    return ERROR;
  }

  /*Allocate memory for cursor behind array. Actual behind of cursor is stored here*/
  Cursor->BehindCursor = (uint16_t *)malloc(CursorPointer[0] * CursorPointer[1] * sizeof(uint16_t));

  if (Cursor->BehindCursor == (uint16_t*)GL_NULL)
  {
    return ERROR;
  }

  /*Fill cursor structure by default values*/
  Cursor->CursorPointer =  ((PointerMark == (uint8_t*)GL_NULL) ? (CursorPointer) : (PointerMark));
  Cursor->Color = GL_Black;
  Cursor->PressState = CUR_RELEASE;
  /*Display cursor at specified position*/
  /* CursorDraw(Xpos, Ypos, CUR_READ_DRAW_CUR); */

  return SUCCESS;
}

/**
  * @brief  Show the cursor at the specified position.
  * @param  Xpos: specifies cursor initial X position.
  * @param  Ypos: specifies cursor initial Y position.
  * @retval None
  */
void CursorShow(uint16_t Xpos, uint16_t Ypos)
{
  /*Display cursor at specified position*/
  CursorDraw(Xpos, Ypos, CUR_READ_DRAW_CUR);
}

/**
  * @brief  Display a cursor at new position. In depence to DrawPhase parameter
  *         one of the following phases is performed:
  *  - Draw last cursors behind at last position - hide cursor.
  *  - Read new cursors behind at specified position.
  *  - Draw cursor at new position - display cursor.
  * @param  Xpos: specifies cursor X position.
  * @param  Ypos: specifies cursor Y position.
  * @param  DrawPhase: drawing phase, possible values are:
  *     @arg  CUR_DRAW_ALL: entire process.
  *     @arg  CUR_DRAW_BEH: draw behind at last position.
  *     @arg  CUR_READ_BEH: read behind at new position.
  *     @arg  CUR_READ_DRAW_CUR: read behind and draw cursor at new position.
  * @retval None
  */
void CursorDraw(uint16_t X, uint16_t Y, uint8_t DrawPhase)
{
  uint8_t tmpX, tmpY = 0x00;
  uint8_t Mask, Temp = 0x00;
  CursorHeader_Typedef *CursorHeader = (CursorHeader_Typedef *)Cursor->CursorPointer;
  uint8_t* CursorImagePointer;
  uint16_t* CursorBehindPointer;

  /*Draw last cursors behind at last position - hide cursor*/
  if (DrawPhase & CUR_DRAW_BEH_MASK)
  {
    /*Initiate pointers on Cursor bit map image and on array of cursors behind to draw*/
    CursorImagePointer = (uint8_t *)(Cursor->CursorPointer) + sizeof(CursorHeader_Typedef);
    CursorBehindPointer = Cursor->BehindCursor;

    /*Repeat for all lines of cursor symbol*/
    for (tmpY = 0x00 ; tmpY < CursorHeader->Height ; tmpY++)
    {
      Mask = 0x00;
      /*Repeat for all columns of cursor symbol*/
      for (tmpX = 0x00 ; tmpX < CursorHeader->Width; tmpX++)
      {
        if (Mask == 0)
        {
          /*Read next byte of bit map image and initiate mask*/
          Temp = *CursorImagePointer;
          CursorImagePointer++;
          Mask = 0x80;
        }

        /*Mask actual byte of cursor bitmap image*/
        if (Temp & Mask)
        {
          /*Draw pixel of cursor behind*/
          LCD_PutPixel(Cursor->X + tmpX, Cursor->Y + tmpY, *CursorBehindPointer++, SinglePixel);
        }
        /*Shift a mask right*/
        Mask >>= 1;
      }
    }
  }

  /*Read new cursors behind at specified position.*/
  if (DrawPhase & CUR_READ_BEH_MASK)
  {
    /*Initiate pointers on Cursor bit map image and on array of cursors behind to draw*/
    CursorImagePointer = (uint8_t *)(Cursor->CursorPointer) + sizeof(CursorHeader_Typedef);
    CursorBehindPointer = Cursor->BehindCursor;

    /*Repeat for all lines of cursor symbol*/
    for (tmpY = 0x00 ; tmpY < CursorHeader->Height ; tmpY++)
    {
      Mask = 0x00;
      /*Repeat for all columns of cursor symbol*/
      for (tmpX = 0x00 ; tmpX < CursorHeader->Width; tmpX++)
      {
        if (Mask == 0)
        {
          /*Read next byte of bitmap image and initiate mask*/
          Temp = *CursorImagePointer;
          CursorImagePointer++;
          Mask = 0x80;
        }
        /*Mask actual byte of cursor bitmap image*/
        if (Temp & Mask)
        {
          /*Read pixel which is going to be behind cursor*/
          *CursorBehindPointer = LCD_GetPixel(X + tmpX, Y + tmpY);
          CursorBehindPointer++;
        }
        /*Shift a mask right*/
        Mask >>= 1;
      }
    }
  }

  /*Draw cursor at new position - display cursor*/
  if (DrawPhase & CUR_DRAW_CUR_MASK || DrawPhase == CUR_DRAW_CUR)
  {
    CursorImagePointer = (uint8_t *)(Cursor->CursorPointer) + sizeof(CursorHeader_Typedef);

    /*Repeat for all lines of cursor symbol*/
    for (tmpY = 0x00 ; tmpY < CursorHeader->Height ; tmpY++)
    {
      Mask = 0x00;
      /*Repeat for all columns of cursor symbol*/
      for (tmpX = 0x00 ; tmpX < CursorHeader->Width; tmpX++)
      {
        if (Mask == 0)
        {
          /*Read next byte of bit map image and initiate mask*/
          Temp = *CursorImagePointer;
          CursorImagePointer++;
          Mask = 0x80;
        }
        /*Mask actual byte of cursor bitmap image*/
        if (Temp & Mask)
        {
          /*Draw pixel of cursor*/
          LCD_PutPixel(X + tmpX, Y + tmpY, Cursor->Color, SinglePixel);
        }
        /*Shift a mask right*/
        Mask >>= 1;
      }
    }
  }

  /*Save new position for future use as last position*/
  Cursor->X = X;
  Cursor->Y = Y;
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
