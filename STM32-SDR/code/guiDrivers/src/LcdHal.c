/**
 ******************************************************************************
 * @file    LcdHal.c
 * @author  MCD Application Team
 * @version V2.0.0
 * @date    11-July-2011
 * @brief   This file contains all the LCD functions whose
 *          implementation depends on the LCD Type used in your Application.
 *          You only need to change these functions implementations
 *          in order to reuse this code with other LCD
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
#include <stdlib.h>
#include <assert.h>
#include "LcdHal.h"
#include "gl_fonts.h"
#include "LcdDriver_ILI9320.h"
#include "stm32_eval.h"


/* External variables --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
typedef struct
{
	__IO uint16_t LCD_REG;
	__IO uint16_t LCD_RAM;
} GL_LCD_TypeDef;

/* Private defines -----------------------------------------------------------*/
#ifdef USE_STM3210E_EVAL
/* Note: LCD /CS is CE4 - Bank 4 of NOR/SRAM Bank 1~4 */
#define GL_LCD_BASE ((uint32_t)(0x60000000 | 0x0C000000))
#elif USE_STM322xG_EVAL
/* Note: LCD /CS is NE3 - Bank 3 of NOR/SRAM Bank 1~4 */
#define GL_LCD_BASE ((uint32_t)(0x60000000 | 0x08000000))
#elif USE_STM32100E_EVAL
/* Note: LCD /CS is CE4 - Bank 4 of NOR/SRAM Bank 1~4 */
#define GL_LCD_BASE           ((uint32_t)(0x60000000 | 0x0C000000))
#endif
#define GL_LCD                ((__IO GL_LCD_TypeDef *) GL_LCD_BASE)
#define RCC_AHBPeriph_FSMC    ((uint32_t)0x00000100)
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/


/* LCD Hardware Parameters Structure */
LCD_HW_Parameters_TypeDef pLcdHwParam;
__IO uint16_t GL_TextColor;
__IO uint16_t GL_BackColor;

/* Private function prototypes -----------------------------------------------*/
//static void BmpBuffer32BitRead(uint32_t* ptr32BitBuffer, uint8_t* ptrBitmap);
/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Create and initialize a new Lcd Hw Parameter structure object
 * @param  None
 * @retval LCD_HW_Parameters_TypeDef* - The created Object pointer
 */
LCD_HW_Parameters_TypeDef* NewLcdHwParamObj(void)
{
	return &pLcdHwParam;
}

/**
 * @brief  Sets the Text color.
 * @param  Color: specifies the Text color code RGB(5-6-5).
 * @param  GL_TextColor: Text color global variable used by GL_DrawChar
 *         and GL_DrawPicture functions.
 * @retval None
 */
void GL_SetTextColor(__IO uint16_t GL_NewTextColor)
{
	GL_TextColor = GL_NewTextColor;
	LCD_SetTextColor(GL_TextColor);
}
uint16_t GL_GetTextColor(void)
{
	return GL_TextColor;
}

/**
 * @brief  Sets the Background color.
 * @param  color: specifies the Background color code RGB(5-6-5).
 * @retval None
 */
void GL_SetBackColor(__IO uint16_t color)
{
	GL_BackColor = color;
	LCD_SetBackColor(GL_BackColor);
}
uint16_t GL_GetBackColor(void)
{
	return GL_BackColor;
}

/**
 * @brief  Clears the whole LCD.
 * @param  color: specifies the Background color code RGB(5-6-5) for the Display.
 * @retval None
 */
void GL_Clear(uint16_t color)
{
	LCD_Clear(color);
}



void GL_DisplayAdjStringLine(uint16_t Line, uint16_t Column, uint8_t *ptr, _Bool isTransparent)
{
	GL_PrintString(Column, Line, (char*) ptr, isTransparent);
}


/**
 * @brief  Sets a display window
 * @param  Xpos: specifies the X bottom left position.
 * @param  Ypos: specifies the Y bottom left position.
 * @param  Height: display window height.
 * @param  Width: display window width.
 * @retval None
 */
void GL_SetDisplayWindow(uint16_t Xpos, uint16_t Ypos, uint16_t Height, uint16_t Width)
{
	LCD_SetDisplayWindow(Xpos, Ypos, Height, Width);
}

/**
 * @brief  Displays a line.
 * @param  Xpos: specifies the X position.
 * @param  Ypos: specifies the Y position.
 * @param  Length: line length.
 * @param  Direction: line direction.
 *         This parameter can be one of the following values:
 *     @arg  Vertical
 *     @arg  Horizontal
 * @retval None
 */
// TODO: Switch Direction to be of type LCD_WriteRAM_Direction
void GL_DrawLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length, uint8_t Direction)
{
	LCD_DrawLine(Xpos, Ypos, Length, Direction);
}

/**
 * @brief  Displays a rectangle.
 * @param  Xpos: specifies the X position.
 * @param  Ypos: specifies the Y position.
 * @param  Height: display rectangle height.
 * @param  Width: display rectangle width.
 * @retval None
 */
void GL_LCD_DrawRect(uint8_t Xpos, uint16_t Ypos, uint8_t Height, uint16_t Width)
{
	LCD_DrawRect(Xpos, Ypos, Height, Width);
}
void GL_LCD_DrawFilledRect(uint8_t Xpos, uint16_t Ypos, uint8_t Height, uint16_t Width)
{
	LCD_DrawFilledRect(Xpos, Ypos, Height, Width);
}

/**
 * @brief  Displays a circle.
 * @param  Xpos: specifies the center X position.
 * @param  Ypos: specifies the center Y position.
 * @param  Radius: the radius size of the circle
 * @retval None
 */
void GL_LCD_DrawCircle(uint8_t Xpos, uint16_t Ypos, uint16_t Radius)
{
	int32_t D;/* Decision Variable */
	uint32_t CurX;/* Current X Value */
	uint32_t CurY;/* Current Y Value */

	D = 3 - (Radius << 1);
	CurX = 0;
	CurY = Radius;

	while (CurX <= CurY) {
		LCD_PutPixel(Xpos + CurX, Ypos + CurY, GL_TextColor);

		LCD_PutPixel(Xpos + CurX, Ypos - CurY, GL_TextColor);

		LCD_PutPixel(Xpos - CurX, Ypos + CurY, GL_TextColor);

		LCD_PutPixel(Xpos - CurX, Ypos - CurY, GL_TextColor);

		LCD_PutPixel(Xpos + CurY, Ypos + CurX, GL_TextColor);

		LCD_PutPixel(Xpos + CurY, Ypos - CurX, GL_TextColor);

		LCD_PutPixel(Xpos - CurY, Ypos + CurX, GL_TextColor);

		LCD_PutPixel(Xpos - CurY, Ypos - CurX, GL_TextColor);

		if (D < 0) {
			D += (CurX << 2) + 6;
		}
		else {
			D += ((CurX - CurY) << 2) + 10;
			CurY--;
		}
		CurX++;
	}
}


/**
 * @brief  Switches the backlight either ON or OFF
 * @param  state. This parameter can be one of the following values:
 *     @arg   GL_ON
 *     @arg   GL_OFF
 * @retval None
 */
void GL_BackLightSwitch(uint8_t u8_State)
{
	if (u8_State == GL_OFF) {
		LCD_DisplayOff();
	}
	else if (u8_State == GL_ON) {
		LCD_DisplayOn();
	}
}

/**
 * @brief  Initializes the LCD.
 * @param  None
 * @retval None
 */
void GL_LCD_Init(void)
{
	/* Setups the LCD */
#if defined(USE_STM3210C_EVAL)
	STM3210C_LCD_Init();
#elif defined (USE_STM3210B_EVAL)
	STM3210B_LCD_Init();
#elif  defined (USE_STM32100B_EVAL)
	STM32100B_LCD_Init();
#elif defined(USE_STM3210E_EVAL)
	STM3210E_LCD_Init();
#elif defined(USE_STM32100E_EVAL)
	STM32100E_LCD_Init();
#elif defined(USE_STM322xG_EVAL)
	STM322xG_LCD_Init();
#elif defined(USE_STM32L152_EVAL)  
	STM32L152_LCD_Init();
#endif
}

/**
 * @brief  Disables LCD Window mode.
 * @param  None
 * @retval None
 */
void GL_LCD_WindowModeDisable(void)
{
	LCD_SetDisplayWindow(0, 0, LCD_HEIGHT, LCD_WIDTH);
}


/**
 * @brief  Sets or reset LCD control lines.
 * @param  GPIOx: where x can be B or D to select the GPIO peripheral.
 * @param  CtrlPins: the Control line.
 *         This parameter can be one of the following values:
 *     @arg  - LCD_CtrlPin_NCS: Chip Select pin
 *     @arg  - CtrlPin_NWR: Read/Write Selection pin
 *     @arg  - CtrlPin_RS: Register/RAM Selection pin
 * @param  BitVal: specifies the value to be written to the selected bit.
 *         This parameter can be one of the following values:
 *     @arg  - GL_LOW: to clear the port pin
 *     @arg  - GL_HIGH: to set the port pin
 * @retval None
 */
void GL_LCD_CtrlLinesWrite(GPIO_TypeDef* GPIOx, uint16_t CtrlPins, GL_SignalActionType BitVal)
{
#if defined(USE_STM3210C_EVAL) || defined (USE_STM3210B_EVAL) ||\
  defined (USE_STM32100B_EVAL) || defined(USE_STM32L152_EVAL)
	/* Set or Reset the control line */
	LCD_CtrlLinesWrite(GPIOx, CtrlPins, (BitAction)BitVal);
#endif
}








// ***********************************************************************************
// New font routines (by Brian)
// ***********************************************************************************

static gl_sFONT *g_pCurrentFont = &GL_Font8x16;
static GL_FontOption g_CurrentFontEnum = GL_FONTOPTION_8x16;

gl_sFONT *getFontStructFromEnum(GL_FontOption font)
{
	switch (font) {
	case GL_FONTOPTION_16x24:     return &GL_Font16x24;
	case GL_FONTOPTION_12x12:     return &GL_Font12x12;
	case GL_FONTOPTION_8x16:      return &GL_Font8x16;
	case GL_FONTOPTION_8x12:      return &GL_Font8x12;
	case GL_FONTOPTION_8x12Bold:  return &GL_Font8x12_bold;
	case GL_FONTOPTION_8x8:       return &GL_Font8x8;
	default:
		assert(0);
	}
	return 0;
}

void GL_SetFont(GL_FontOption newFont)
{
	g_pCurrentFont = getFontStructFromEnum(newFont);
	g_CurrentFontEnum = newFont;
}
GL_FontOption GL_GetFont(void)
{
	return g_CurrentFontEnum;
}

uint16_t GL_GetFontLetterWidth(GL_FontOption font)
{
	return getFontStructFromEnum(font)->Width;
}
uint16_t GL_GetFontLetterHeight(GL_FontOption font)
{
	return getFontStructFromEnum(font)->Height;
}

void GL_PrintString(uint16_t x, uint16_t y, const char *str, _Bool isTransparent)
{
	uint16_t curX = x;
	for (int idx = 0; str[idx] != 0; idx++) {
		GL_PrintChar(curX, y, str[idx], isTransparent);
		curX += g_pCurrentFont->Width;
	}
}
void GL_PrintChar(uint16_t x, uint16_t y, char c, _Bool isTransparent)
{
	const char FONT_DATA_OFFEST = ' ';
	uint16_t firstRowOffset = (c - FONT_DATA_OFFEST) * g_pCurrentFont->Height;
	const uint16_t *pData = &g_pCurrentFont->table[firstRowOffset];

	// All fonts (but the 16x24) have the bits reversed.
	_Bool bitsReversed = (g_pCurrentFont != &GL_Font16x24);

	if (isTransparent) {
		LCD_DrawBMP1BitTransparent(x, y, g_pCurrentFont->Height, g_pCurrentFont->Width, pData, bitsReversed);
	} else {
		LCD_DrawBMP1Bit(x, y, g_pCurrentFont->Height, g_pCurrentFont->Width, pData, bitsReversed);
	}

}

// TODO: Remove LCD_StringLine() once all code no longer depends on it!
void LCD_StringLine(uint16_t PosX, uint16_t PosY, const char *str)
{
	// The +12 is for the font height
	// (This function designed for providing the Y coord as distance from bottom of screen, relative to bottom of text.
	GL_PrintString(PosX, LCD_HEIGHT - PosY - GL_GetFontLetterHeight(GL_GetFont()), str, 0);
}


void GL_TestDisplayScreen(void)
{
#if 0
	LCD_Clear(LCD_COLOR_BLUE);

	/*
	 * Test fonts
	 */
	_Bool isTransparent = 1;
	GL_SetBackColor(LCD_COLOR_DGREEN);
	GL_SetTextColor(LCD_COLOR_YELLOW);

	GL_SetFont(GL_FONTOPTION_16x24);
	GL_PrintString(10, 10, "|/-\\ Hello World! abc&ABC", isTransparent);

	GL_SetFont(GL_FONTOPTION_12x12);
	GL_PrintString(10, 60, "|/-\\ Hello World! abc&ABC", isTransparent);

	GL_SetFont(GL_FONTOPTION_8x12);
	GL_PrintString(10, 100, "|/-\\ Hello World! abc&ABC", isTransparent);

	GL_SetFont(GL_FONTOPTION_8x12Bold);
	GL_PrintString(10, 140, "|/-\\ Hello World! abc&ABC", isTransparent);

	GL_SetFont(GL_FONTOPTION_8x16);
	GL_PrintString(10, 160, "|/-\\ Hello World! abc&ABC", isTransparent);

	GL_SetFont(GL_FONTOPTION_8x8);
	GL_PrintString(10, 200, "|/-\\ Hello World! abc&ABC", isTransparent);




	volatile int a = 0;
	while (1)
		a ++;
#endif
}
