#ifndef __LCDDRIVER_ILI9320_H
#define __LCDDRIVER_ILI9320_H

#ifdef __cplusplus
extern "C" {
#endif


#include "stm32_eval.h"



// From LcdHal.h

#define LCD_ILI9320 0x9320
#define LCD_SPFD5408 0x5408
extern __IO uint32_t LCDType;




// Actual screen size:
#define ACTUAL_SCREEN_WIDTH  ((uint16_t) 240)
#define ACTUAL_SCREEN_HEIGHT ((uint16_t) 320)

typedef enum
{
	_0_degree = 0,
	_90_degree,
	_180_degree,
	_270_degree
}LCD_Direction_TypeDef;

/**
 * LCD_Direction_TypeDef enumeration definition
 */
// Set to 0, 90, 180, or 270
#define LCD_ROTATION 270
#if LCD_ROTATION == 0
	#define LCD_Height  ACTUAL_SCREEN_HEIGHT
	#define LCD_Width   ACTUAL_SCREEN_WIDTH
#elif LCD_ROTATION == 90
	#define LCD_Height  ACTUAL_SCREEN_WIDTH
	#define LCD_Width   ACTUAL_SCREEN_HEIGHT
#elif LCD_ROTATION == 180
	#define LCD_Height  ACTUAL_SCREEN_HEIGHT
	#define LCD_Width   ACTUAL_SCREEN_WIDTH
#elif LCD_ROTATION == 270
	#define LCD_Height  ACTUAL_SCREEN_WIDTH
	#define LCD_Width   ACTUAL_SCREEN_HEIGHT
#else
	#error Must define LCD_ROTATION to be a direction
#endif

extern LCD_Direction_TypeDef LCD_Direction;

/*
 * Screen rotation for direct graphic-ram writes.
 */
// Work with the direct LCD RAM access
#define R3_CMALWAYS_SET	0x1000
#define R3_HORZ_DEC		0x0000
#define R3_HORZ_INC		0x0010
#define R3_VERT_DEC		0x0000
#define R3_VERT_INC		0x0020
#define R3_ADDR_HORIZ	0x0000
#define R3_ADDR_VERT	0x0008
#if SCREEN_ROTATED_180 == 1
// Invert
#define MAKE_R3_CMD(normal)	((normal) ^ (R3_HORZ_INC | R3_VERT_INC | R3_ADDR_VERT))
#else
// When upright
#define MAKE_R3_CMD(normal)	(normal)
#endif

/* Set GRAM write direction and BGR = 1 */
/* I/D=00 (Horizontal : decrement, Vertical : decrement) */
/* AM=1 (address is updated in vertical writing direction) */
#define LCDDRIVER_R3CMD_HDEC_VDEC_ADDRVERT	0x1008
/* Set GRAM write direction and BGR = 1 */
/* I/D = 01 (Horizontal : increment, Vertical : decrement) */
/* AM = 1 (address is updated in vertical writing direction) */
#define LCDDRIVER_R3CMD_HINC_VDEC_ADDRVERT	0x1018
#define LCDDRIVER_R3CMD_DEFAULT LCDDRIVER_R3CMD_HINC_VDEC_ADDRVERT


#define LCD_DIR_HORIZONTAL       0x0000
#define LCD_DIR_VERTICAL         0x0001



void LCD_WriteRAMWord(uint16_t RGB_Code);
void LCD_Change_Direction(LCD_Direction_TypeDef Direction);
void LCD_DrawMonoBMP(const uint8_t *Pict, uint16_t Xpos_Init, uint16_t Ypos_Init, uint16_t Height, uint16_t Width);
void LCD_DrawColorBMP(uint8_t* ptrBitmap, uint16_t Xpos_Init, uint16_t Ypos_Init, uint16_t Height, uint16_t Width);
void LCD_FillArea(uint16_t Xpos_Init, uint16_t Ypos_Init, uint16_t Height, uint16_t Width, uint16_t color);


uint16_t LCD_ReadReg(uint8_t LCD_Reg);
uint16_t LCD_ReadRAM(void);
void LCD_PutPixel(uint16_t Xpos, uint16_t Ypos, uint16_t Color);
uint16_t LCD_GetPixel(uint16_t Xpos, uint16_t Ypos);













// From TFT_Display.h
//void LCD_Reset(void);
//void LCD_Init(void);
//void TIM_Config(void);
//void LCD_CtrlLinesConfig(void);
//void LCD_FSMCConfig(void);
//
//void TFT_Delay(__IO uint32_t nTime);
//
//void PutPixel(int16_t x, int16_t y);
//
//void LCD_SetColors(__IO uint16_t _TextColor, __IO uint16_t _BackColor);
//void LCD_GetColors(__IO uint16_t *_TextColor, __IO uint16_t *_BackColor);
void LCD_SetTextColor(__IO uint16_t Color);
void LCD_SetBackColor(__IO uint16_t Color);
void LCD_Clear(uint16_t Color);
//void LCD_CharSize(__IO uint16_t size);
//void Pixel(int16_t x, int16_t y, int16_t c);
//
//void LCD_DrawFFT(uint8_t fftData[]);
//void LCD_PutChar(int16_t PosX, int16_t PosY, char c);
//void LCD_StringLine(uint16_t PosX, uint16_t PosY, char *str);
void LCD_DrawLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length, uint8_t Direction);
void LCD_DrawRect(uint16_t Xpos, uint16_t Ypos, uint8_t Height, uint16_t Width);
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
//



// Low-level access routines.
void LCD_WriteRAM_Prepare(void);
void LCD_WriteRAM(uint16_t RGB_Code);
void LCD_WriteReg(uint8_t LCD_Reg, uint16_t LCD_RegValue);
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos);
void LCD_SetDisplayWindow(uint8_t Xpos, uint16_t Ypos, uint16_t Height, uint16_t Width);

// Setup routines
void LCD_DisplayOn(void);
void LCD_DisplayOff(void);

//void LCD_BackLight(int procentai);
//
//void Display_ULong(uint16_t data, uint8_t XL, uint8_t YL);
//
//void Plot_String(uint8_t *string, uint8_t x, uint8_t y);
//void Plot_Integer(int16_t number, uint8_t x, uint8_t y);
//void Plot_Freq(uint32_t number, uint8_t x, uint8_t y, uint32_t changeRate);

uint16_t LCD_ReadReg(uint8_t LCD_Reg);

void LCD_TestDisplayScreen(void);


#ifdef __cplusplus
}
#endif

#endif
