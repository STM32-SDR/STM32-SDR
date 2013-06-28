/*
 * TFT_Display.c
 *
 *  Created on: Dec 19, 2012
 *      Author: CharleyK
 */
#include 	"TFT_Display.h"
#include	"stm32f4xx_gpio.h"
#include	"stm32f4xx.h"
#include	"stm32f4xx_rcc.h"
#include	"stm32f4xx_fsmc.h"
#include	"stm32f4xx_tim.h"
#include	"AsciiLib.h"
#include	"touch_pad.h"
#include 	"PSKDet.h"
#define MAX_POLY_CORNERS   200
#define POLY_Y(Z)          ((int32_t)((Points + Z)->X))
#define POLY_X(Z)          ((int32_t)((Points + Z)->Y))
#define ABS(X)  ((X) > 0 ? (X) : -(X))

/* Global variables to set the written text color */
extern __IO uint16_t LCD_textColor;		// MOVED TO LcdDriver_ILI9320.c
extern __IO uint16_t LCD_backColor;		// MOVED TO LcdDriver_ILI9320.c
__IO uint16_t asciisize = 16;

//****************************************************************************//
static void LCD_PolyLineRelativeClosed(pPoint Points, uint16_t PointCount,
		uint16_t Closed);




void LCD_SetColors(__IO uint16_t _TextColor, __IO uint16_t _BackColor)
{
	LCD_textColor = _TextColor;
	LCD_backColor = _BackColor;
}

void LCD_GetColors(__IO uint16_t *_TextColor, __IO uint16_t *_BackColor)
{
	*_TextColor = LCD_textColor;
	*_BackColor = LCD_backColor;
}


/* 8x8=8 12x12=12 8x16=16 12x12=14 16x24=24 */
void LCD_CharSize(__IO uint16_t size)
{
	asciisize = size;
}



//February
void PutPixel(int16_t x, int16_t y)
{
	LCD_PutPixel(x, y, LCD_textColor);
}



//February
void LCD_StringLine(uint16_t PosX, uint16_t PosY, char *str)
{
	// The +12 is for the font height
	// (This function designed for providing the Y coord as distance from bottom of screen, relative to bottom of text.
	GL_PrintString(PosX, LCD_HEIGHT - PosY - GL_GetFontLetterHeight(), str, 0);
}

// Display the FFT data to the screen.
// Expect fftData[] to be an array of 256 ints.
void LCD_DrawFFT(uint8_t fftData[])
{
	const int OFFSET_X    =  40;
	const int OFFSET_Y    =   0;   // Was 175 in old coord system.
	const int FFT_WIDTH   = 240;
	const int FFT_HEIGHT  =  64;
	const int SELFREQ_ADJ =   4;

	float selectedFreqX = (float) (NCO_Frequency - 125) / 15.625;
	if (selectedFreqX < 0) {
		selectedFreqX = 0;
	}

	for (int x = 0; x < FFT_WIDTH; x++) {
		// Don't draw at same place as line to avoid flickering.
		if (x == (int) selectedFreqX) {
			continue;
		}

		// Plot this column of the FFT.
		for (int y = 0; y < FFT_HEIGHT; y++) {
			if (y < fftData[x + 8])
				LCD_PutPixel(x + OFFSET_X, FFT_HEIGHT - y + OFFSET_Y, LCD_COLOR_BLUE);
			else
				LCD_PutPixel(x + OFFSET_X, FFT_HEIGHT - y + OFFSET_Y, LCD_COLOR_WHITE);
		}
	}

	for (int y = 0; y < FFT_HEIGHT - SELFREQ_ADJ; y++)
		LCD_PutPixel((int) selectedFreqX + OFFSET_X, y + OFFSET_Y + SELFREQ_ADJ, LCD_COLOR_RED);
}




void LCD_DrawSquare(uint16_t Xpos, uint16_t Ypos, uint16_t a)
{
	int x, y;
	x = 0;
	y = 0;
	while (x < a + 1) {
		PutPixel(Xpos + x, Ypos);
		PutPixel(Xpos + x, Ypos + a);
		x++;
	}
	while (y < a + 1) {
		PutPixel(Xpos, Ypos + y);
		PutPixel(Xpos + a, Ypos + y);
		y++;
	}
}

void LCD_DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{
	int16_t D;/* Decision Variable */
	uint16_t CurX;/* Current X Value */
	uint16_t CurY;/* Current Y Value */
	D = 3 - (Radius << 1);
	CurX = 0;
	CurY = Radius;
	while (CurX <= CurY) {
		PutPixel(Xpos + CurX, Ypos + CurY);
		PutPixel(Xpos + CurX, Ypos - CurY);
		PutPixel(Xpos - CurX, Ypos + CurY);
		PutPixel(Xpos - CurX, Ypos - CurY);
		PutPixel(Xpos + CurY, Ypos + CurX);
		PutPixel(Xpos + CurY, Ypos - CurX);
		PutPixel(Xpos - CurY, Ypos + CurX);
		PutPixel(Xpos - CurY, Ypos - CurX);
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

void LCD_DrawFullCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{
	int16_t D;
	uint16_t CurX;
	uint16_t CurY;
	D = 3 - (Radius << 1);
	CurX = 0;
	CurY = Radius;

	while (CurX <= CurY) {
		LCD_DrawUniLine(Xpos + CurX, Ypos + CurY, Xpos + CurX, Ypos - CurY);
		LCD_DrawUniLine(Xpos - CurX, Ypos + CurY, Xpos - CurX, Ypos - CurY);
		LCD_DrawUniLine(Xpos + CurY, Ypos + CurX, Xpos + CurY, Ypos - CurX);
		LCD_DrawUniLine(Xpos - CurY, Ypos + CurX, Xpos - CurY, Ypos - CurX);

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
void LCD_DrawFullRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width,
		uint16_t Height)
{
	int x, y;
	x = 0;
	y = 0;
	while (x < Width) {
		while (y < Height) {
			PutPixel(Xpos + x, Ypos + y);
			y++;
		}
		y = 0;
		x++;
	}
}

void LCD_DrawFullSquare(uint16_t Xpos, uint16_t Ypos, uint16_t a)
{
	int x, y;
	x = 0;
	y = 0;
	while (x < a) {
		while (y < a) {
			PutPixel(Xpos + x, Ypos + y);
			y++;
		}
		y = 0;
		x++;
	}
}

void LCD_DrawUniLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0, yinc1 =
			0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0,
			curpixel = 0;

	deltax = ABS(x2 - x1); /* The difference between the x's */
	deltay = ABS(y2 - y1); /* The difference between the y's */
	x = x1; /* Start x off at the first pixel */
	y = y1; /* Start y off at the first pixel */

	if (x2 >= x1) /* The x-values are increasing */
	{
		xinc1 = 1;
		xinc2 = 1;
	}
	else /* The x-values are decreasing */
	{
		xinc1 = -1;
		xinc2 = -1;
	}

	if (y2 >= y1) /* The y-values are increasing */
	{
		yinc1 = 1;
		yinc2 = 1;
	}
	else /* The y-values are decreasing */
	{
		yinc1 = -1;
		yinc2 = -1;
	}

	if (deltax >= deltay) /* There is at least one x-value for every y-value */
	{
		xinc1 = 0; /* Don't change the x when numerator >= denominator */
		yinc2 = 0; /* Don't change the y for every iteration */
		den = deltax;
		num = deltax / 2;
		numadd = deltay;
		numpixels = deltax; /* There are more x-values than y-values */
	}
	else /* There is at least one y-value for every x-value */
	{
		xinc2 = 0; /* Don't change the x for every iteration */
		yinc1 = 0; /* Don't change the y when numerator >= denominator */
		den = deltay;
		num = deltay / 2;
		numadd = deltax;
		numpixels = deltay; /* There are more y-values than x-values */
	}

	for (curpixel = 0; curpixel <= numpixels; curpixel++) {
		PutPixel(x, y); /* Draw the current pixel */
		num += numadd; /* Increase the numerator by the top of the fraction */
		if (num >= den) /* Check if numerator >= denominator */
		{
			num -= den; /* Calculate the new numerator value */
			x += xinc1; /* Change the x as appropriate */
			y += yinc1; /* Change the y as appropriate */
		}
		x += xinc2; /* Change the x as appropriate */
		y += yinc2; /* Change the y as appropriate */
	}
}





void LCD_PolyLine(pPoint Points, uint16_t PointCount)
{
	int16_t X = 0, Y = 0;

	if (PointCount < 2) {
		return;
	}

	while (--PointCount) {
		X = Points->X;
		Y = Points->Y;
		Points++;
		LCD_DrawUniLine(X, Y, Points->X, Points->Y);
	}
}

static void LCD_PolyLineRelativeClosed(pPoint Points, uint16_t PointCount,
		uint16_t Closed)
{
	int16_t X = 0, Y = 0;
	pPoint First = Points;

	if (PointCount < 2) {
		return;
	}
	X = Points->X;
	Y = Points->Y;
	while (--PointCount) {
		Points++;
		LCD_DrawUniLine(X, Y, X + Points->X, Y + Points->Y);
		X = X + Points->X;
		Y = Y + Points->Y;
	}
	if (Closed) {
		LCD_DrawUniLine(First->X, First->Y, X, Y);
	}
}

void LCD_ClosedPolyLine(pPoint Points, uint16_t PointCount)
{
	LCD_PolyLine(Points, PointCount);
	LCD_DrawUniLine(Points->X, Points->Y, (Points + PointCount - 1)->X,
			(Points + PointCount - 1)->Y);
}

void LCD_PolyLineRelative(pPoint Points, uint16_t PointCount)
{
	LCD_PolyLineRelativeClosed(Points, PointCount, 0);
}

void LCD_ClosedPolyLineRelative(pPoint Points, uint16_t PointCount)
{
	LCD_PolyLineRelativeClosed(Points, PointCount, 1);
}





void Plot_Integer(int16_t number, uint8_t x, uint8_t y)
{
	uint8_t buffer[11] = "         0", index = 9, sign = 0;
	//   "0123456,789"
	if (number < 0) {
		number = -number;
		sign = 1;
	}

	while (number > 0) {
		buffer[index--] = number % 10 + 0x30;
		number /= 10;

		if ((index == 6 || index == 2) && number > 0)
			buffer[index--] = ',';
	}

	if (sign == 1)
		buffer[index] = '-';

	//Plot_String ( buffer, x, y );
	LCD_StringLine(x, y, (char*) &buffer[0]);
}


#define BUFF_SIZE   11
#define DECIMAL_IDX 6
void writeNumberToBuffer(uint32_t number, char buffer[])
{
	int index = BUFF_SIZE - 2; // -2 = 0 indexed & room for \0.
	while (number > 0) {
		buffer[index--] = number % 10 + '0';
		number /= 10;

		if (index == DECIMAL_IDX)
			buffer[index--] = '.';
	}
}

// Return the number of characters to highlight in the frequency display
// based on how the frequency number will change as the dial spins.
int numCharChangingByDial(int changeRate) {
	int colourChangeIdx = BUFF_SIZE - 2; // -2 = 0 indexed & room for \0.
	int fUnitCopy = changeRate;
	while (fUnitCopy > 1) {
		fUnitCopy /= 10;
		colourChangeIdx--;
	}
	if (colourChangeIdx > DECIMAL_IDX) {
		colourChangeIdx++;
	}
	return colourChangeIdx;
}

void Plot_Freq(uint32_t number, uint8_t x, uint8_t y, uint32_t changeRate)
{
	char buffer[BUFF_SIZE] = "         0";
	writeNumberToBuffer(number, buffer);

	// Display the whole number (in black):
	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_StringLine(x, y, buffer);

	// Now display the part of the number changing
	buffer[numCharChangingByDial(changeRate)] = 0;
	LCD_SetTextColor(LCD_COLOR_BLUE);
	LCD_StringLine(x, y, buffer);

	// Reset color to black for next text write.
	LCD_SetTextColor(LCD_COLOR_BLACK);
}

