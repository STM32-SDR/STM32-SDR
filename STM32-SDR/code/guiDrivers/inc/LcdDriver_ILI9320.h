/*
 * Header file for ILI9320 LCD driver
 *
 * STM32-SDR: A software defined HAM radio embedded system.
 * Copyright (C) 2013, STM32-SDR Group
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef __LCDDRIVER_ILI9320_H
#define __LCDDRIVER_ILI9320_H

#ifdef __cplusplus
extern "C" {
#endif


#include "stm32_eval.h"


/*
 * LCD Colours
 */
#define LCD_COLOR_WHITE          0xFFFF
#define LCD_COLOR_BLACK          0x0000
#define LCD_COLOR_GREY           0xF7DE
#define LCD_COLOR_LGRAY          0xC618
#define LCD_COLOR_DGRAY          0x7BEF
#define LCD_COLOR_DGREEN         0x03E0
#define LCD_COLOR_DCYAN          0x03EF
#define LCD_COLOR_MAROON         0x7800
#define LCD_COLOR_PURPLE         0x780F
#define LCD_COLOR_OLIVE          0x7BE0
#define LCD_COLOR_BLUE           0x001F
#define LCD_COLOR_BLUE2          0x051F
#define LCD_COLOR_NAVY           0x000F
#define LCD_COLOR_RED            0xF800
#define LCD_COLOR_MAGENTA        0xF81F
#define LCD_COLOR_GREEN          0x07E0
#define LCD_COLOR_CYAN           0x7FFF
#define LCD_COLOR_YELLOW         0xFFE0


// From LcdHal.h
#define LCD_ILI9320 0x9320
#define LCD_SPFD5408 0x5408
#define LCD_SSD1289 0x8989
extern __IO uint32_t LCDType;



// Actual screen size:
#define LCD_ACTUAL_SCREEN_WIDTH  ((uint16_t) 240)
#define LCD_ACTUAL_SCREEN_HEIGHT ((uint16_t) 320)

/**
 * LCD_Direction_TypeDef enumeration definition
 */
// Set to 0, 90, 180, or 270
#define LCD_ROTATION 270
#if LCD_ROTATION == 0
	#define LCD_HEIGHT  LCD_ACTUAL_SCREEN_HEIGHT
	#define LCD_WIDTH   LCD_ACTUAL_SCREEN_WIDTH
#elif LCD_ROTATION == 90
	#define LCD_HEIGHT  LCD_ACTUAL_SCREEN_WIDTH
	#define LCD_WIDTH   LCD_ACTUAL_SCREEN_HEIGHT
#elif LCD_ROTATION == 180
	#define LCD_HEIGHT  LCD_ACTUAL_SCREEN_HEIGHT
	#define LCD_WIDTH   LCD_ACTUAL_SCREEN_WIDTH
#elif LCD_ROTATION == 270
	#define LCD_HEIGHT  LCD_ACTUAL_SCREEN_WIDTH
	#define LCD_WIDTH   LCD_ACTUAL_SCREEN_HEIGHT
#else
	#error Must define LCD_ROTATION to be a direction
#endif



// TODO: Remove these when possible.
#define LCD_DIR_HORIZONTAL       0x0000
#define LCD_DIR_VERTICAL         0x0001

// Directions used for creating a line. Specify the start point, and then the direction.
typedef enum
{
	LCD_WriteRAMDir_Right = 0,
	LCD_WriteRAMDir_Down,
	LCD_WriteRAMDir_Left,
	LCD_WriteRAMDir_Up
} LCD_WriteRAM_Direction;


uint16_t LCD_ReadReg(uint8_t LCD_Reg);
uint16_t LCD_ReadRAM(void);
void LCD_PutPixel(uint16_t Xpos, uint16_t Ypos, uint16_t Color);
uint16_t LCD_GetPixel(uint16_t Xpos, uint16_t Ypos);


// From TFT_Display.h
void LCD_Init(void);
void LCD_SetTextColor(__IO uint16_t Color);
void LCD_SetBackColor(__IO uint16_t Color);
void LCD_Clear(uint16_t Color);
void LCD_DrawLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length, uint8_t Direction);
void LCD_DrawRect(uint16_t Xpos, uint16_t Ypos, uint8_t Height, uint16_t Width);
void LCD_DrawFilledRect(int x, int y, int height, int width);

//void LCD_DrawSquare(uint16_t Xpos, uint16_t Ypos, uint16_t a);
//void LCD_DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius);
//
//void LCD_DrawUniLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
//void LCD_DrawFullRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height);
//void LCD_DrawFullCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius);
//void LCD_PolyLine(pPoint Points, uint16_t PointCount);
//void LCD_PolyLineRelative(pPoint Points, uint16_t PointCount);
//void LCD_ClosedPolyLine(pPoint Points, uint16_t PointCount);
//void LCD_ClosedPolyLineRelative(pPoint Points, uint16_t PointCount);
//void LCD_FillPolyLine(pPoint Points, uint16_t PointCount);
//void LCD_DrawFullSquare(uint16_t Xpos, uint16_t Ypos, uint16_t a);


// Image drawing functions
void LCD_DrawBMP16Bit(int x, int y, int height, int width, const uint16_t* pBitmap, _Bool revByteOrder);
void LCD_DrawBMP1Bit(int x, int y, int height, int width, const uint16_t* pBitmap, _Bool revBitOrder);
void LCD_DrawBMP1BitTransparent(int x, int y, int height, int width, const uint16_t* pBitmap, _Bool revBitOrder);


// Low-level access routines.
void LCD_SetDisplayWindow(uint16_t Xpos, uint16_t Ypos, uint16_t Height, uint16_t Width);
void LCD_WriteRAM_PrepareDir(LCD_WriteRAM_Direction direction);
void LCD_WriteRAM(uint16_t RGB_Code);

void LCD_WriteReg(uint8_t LCD_Reg, uint16_t LCD_RegValue);
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos);


// Setup routines
void LCD_DisplayOn(void);
void LCD_DisplayOff(void);
void LCD_BackLight(int procentai);

// Test function
void LCD_TestDisplayScreen(void);



#ifdef __cplusplus
}
#endif

#endif
