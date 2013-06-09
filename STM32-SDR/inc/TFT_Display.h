/*
 * TFT_Display.h
 *
 *  Created on: Dec 19, 2012
 *      Author: CharleyK
 */

#ifndef TFT_DISPLAY_H_
#define TFT_DISPLAY_H_

/*
 * SSD1289.h
 *
 *  Created on: Dec 16, 2012
 *      Author: CharleyK
 */

#include "stm32f4xx.h"

typedef struct
{
	int16_t X;
	int16_t Y;
} Point, *pPoint;

#define LCD_COLOR_WHITE          0xFFFF
#define LCD_COLOR_BLACK          0x0000
#define LCD_COLOR_GREY           0xF7DE
#define LCD_COLOR_BLUE           0x001F
#define LCD_COLOR_BLUE2          0x051F
#define LCD_COLOR_RED            0xF800
#define LCD_COLOR_MAGENTA        0xF81F
#define LCD_COLOR_GREEN          0x07E0
#define LCD_COLOR_CYAN           0x7FFF
#define LCD_COLOR_YELLOW         0xFFE0

#define   BLACK        0x0000
#define   NAVY         0x000F
#define   DGREEN       0x03E0
#define   DCYAN        0x03EF
#define   MAROON       0x7800
#define   PURPLE       0x780F
#define   OLIVE        0x7BE0
#define   GREY         0xF7DE
#define   LGRAY        0xC618
#define   DGRAY        0x7BEF
#define   BLUE         0x001F
#define   GREEN        0x07E0
#define   CYAN         0x07FF
#define   RED          0xF800
#define   MAGENTA      0xF81F
#define   YELLOW       0xFFE0
#define   WHITE        0xFFFF

#define LCD_DIR_HORIZONTAL       0x0000
#define LCD_DIR_VERTICAL         0x0001

#define LCD_PIXEL_WIDTH          0x0140
#define LCD_PIXEL_HEIGHT         0x00F0

#define ASSEMBLE_RGB(R ,G, B)    ((((R)& 0xF8) << 8) | (((G) & 0xFC) << 3) | (((B) & 0xF8) >> 3))

void LCD_Reset(void);
void LCD_Init(void);
void TIM_Config(void);
void LCD_CtrlLinesConfig(void);
void LCD_FSMCConfig(void);

void TFT_Delay(__IO uint32_t nTime);

void PutPixel(int16_t x, int16_t y);

void LCD_SetColors(__IO uint16_t _TextColor, __IO uint16_t _BackColor);
void LCD_GetColors(__IO uint16_t *_TextColor, __IO uint16_t *_BackColor);
void LCD_SetTextColor(__IO uint16_t Color);
void LCD_SetBackColor(__IO uint16_t Color);
void LCD_Clear(uint16_t Color);
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos);
void LCD_CharSize(__IO uint16_t size);
void Pixel(int16_t x, int16_t y, int16_t c);

void LCD_DrawFFT(uint8_t fftData[]);
void LCD_PutChar(int16_t PosX, int16_t PosY, char c);
void LCD_StringLine(uint16_t PosX, uint16_t PosY, char *str);
void LCD_DrawLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length, uint8_t Direction);
void LCD_DrawRect(uint16_t Xpos, uint16_t Ypos, uint8_t Height, uint16_t Width);
void LCD_DrawSquare(uint16_t Xpos, uint16_t Ypos, uint16_t a);
void LCD_DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius);

void LCD_DrawUniLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void LCD_DrawFullRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height);
void LCD_DrawFullCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius);
void LCD_PolyLine(pPoint Points, uint16_t PointCount);
void LCD_PolyLineRelative(pPoint Points, uint16_t PointCount);
void LCD_ClosedPolyLine(pPoint Points, uint16_t PointCount);
void LCD_ClosedPolyLineRelative(pPoint Points, uint16_t PointCount);
void LCD_FillPolyLine(pPoint Points, uint16_t PointCount);
void LCD_SetDisplayWindow(uint8_t Xpos, uint16_t Ypos, uint8_t Height, uint16_t Width);
void LCD_DrawFullSquare(uint16_t Xpos, uint16_t Ypos, uint16_t a);

void LCD_WriteRAM_Prepare(void);
void LCD_WriteRAM(uint16_t RGB_Code);
void LCD_WriteReg(uint8_t LCD_Reg, uint16_t LCD_RegValue);
void LCD_DisplayOn(void);
void LCD_DisplayOff(void);
void LCD_BackLight(int procentai);

void Display_ULong(uint16_t data, uint8_t XL, uint8_t YL);

void Plot_String(uint8_t *string, uint8_t x, uint8_t y);
void Plot_Integer(int16_t number, uint8_t x, uint8_t y);
void Plot_Freq(uint32_t number, uint8_t x, uint8_t y, uint32_t changeRate);

#endif /* TFT_DISPLAY_H_ */
