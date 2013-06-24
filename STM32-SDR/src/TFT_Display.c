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
#define LCD_REG      (*((volatile unsigned short *) 0x60000000))
#define LCD_RAM      (*((volatile unsigned short *) 0x60020000))
#define MAX_POLY_CORNERS   200
#define POLY_Y(Z)          ((int32_t)((Points + Z)->X))
#define POLY_X(Z)          ((int32_t)((Points + Z)->Y))
#define ABS(X)  ((X) > 0 ? (X) : -(X))

/* Global variables to set the written text color */
extern __IO uint16_t LCD_textColor;		// MOVED TO LcdDriver_ILI9320.c
extern __IO uint16_t LCD_backColor;		// MOVED TO LcdDriver_ILI9320.c
__IO uint16_t asciisize = 16;

uint16_t TimerPeriod = 0;
uint16_t Channel1Pulse = 0;
//****************************************************************************//
static void LCD_PolyLineRelativeClosed(pPoint Points, uint16_t PointCount,
		uint16_t Closed);
//****************************************************************************//
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_OCInitTypeDef TIM_OCInitStructure;
//****************************************************************************//
void LCD_Reset(void)
{
	/* Power Down the reset */
	GPIO_WriteBit(GPIOB, GPIO_Pin_1, Bit_RESET);

	/* wait for a delay to insure registers erasing */
	TFT_Delay(0x4FFF);

	/* Power on the reset */
	GPIO_WriteBit(GPIOB, GPIO_Pin_1, Bit_SET);
}

void LCD_Init(void)
{
	LCD_CtrlLinesConfig();
	TFT_Delay(3000);

	LCD_Reset();
	TFT_Delay(3000);

	LCD_FSMCConfig();
	TFT_Delay(3000);
	TIM_Config();
	LCD_BackLight(100);

	LCD_WriteReg(0x00E5, 0x8000);
	LCD_WriteReg(0x0000, 0x0001);
	LCD_WriteReg(0x0001, 0x0100);
	LCD_WriteReg(0x0002, 0x0700);
	LCD_WriteReg(0x0003, 0x1030);
	LCD_WriteReg(0x0004, 0x0000);
	LCD_WriteReg(0x0008, 0x0202);
	LCD_WriteReg(0x0009, 0x0000);
	LCD_WriteReg(0x000A, 0x0000);
	LCD_WriteReg(0x000C, 0x0000);
	LCD_WriteReg(0x000D, 0x0000);
	LCD_WriteReg(0x000F, 0x0000);

	LCD_WriteReg(0x0010, 0x0000);
	LCD_WriteReg(0x0011, 0x0000);
	LCD_WriteReg(0x0012, 0x0000);
	LCD_WriteReg(0x0013, 0x0000);

	LCD_WriteReg(0x0010, 0x17B0);
	LCD_WriteReg(0x0011, 0x0037);
	TFT_Delay(50); // Delay 50ms
	LCD_WriteReg(0x0012, 0x013B);
	TFT_Delay(50); // Delay 50ms
	LCD_WriteReg(0x0013, 0x1800);
	LCD_WriteReg(0x0029, 0x000F);
	TFT_Delay(50); // Delay 50ms
	LCD_WriteReg(0x0020, 0x0000);
	LCD_WriteReg(0x0021, 0x0000);

	LCD_WriteReg(0x0030, 0x0000);
	LCD_WriteReg(0x0031, 0x0007);
	LCD_WriteReg(0x0032, 0x0103);
	LCD_WriteReg(0x0035, 0x0407);
	LCD_WriteReg(0x0036, 0x090F);
	LCD_WriteReg(0x0037, 0x0404);
	LCD_WriteReg(0x0038, 0x0400);
	LCD_WriteReg(0x0039, 0x0404);
	LCD_WriteReg(0x003C, 0x0000);
	LCD_WriteReg(0x003D, 0x0400);

	LCD_WriteReg(0x0050, 0x0000);
	LCD_WriteReg(0x0051, 0x00EF);
	LCD_WriteReg(0x0052, 0x0000);
	LCD_WriteReg(0x0053, 0x013F);
	LCD_WriteReg(0x0060, 0x2700);
	LCD_WriteReg(0x0061, 0x0001);
	LCD_WriteReg(0x006A, 0x0000);

	LCD_WriteReg(0x0080, 0x0000);
	LCD_WriteReg(0x0081, 0x0000);
	LCD_WriteReg(0x0082, 0x0000);
	LCD_WriteReg(0x0083, 0x0000);
	LCD_WriteReg(0x0084, 0x0000);
	LCD_WriteReg(0x0085, 0x0000);

	LCD_WriteReg(0x0090, 0x0010);
	LCD_WriteReg(0x0092, 0x0000);
	LCD_WriteReg(0x0093, 0x0003);
	LCD_WriteReg(0x0095, 0x0110);
	LCD_WriteReg(0x0097, 0x0000);
	LCD_WriteReg(0x0098, 0x0000);
	LCD_WriteReg(0x0007, 0x0173);
}






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
	Pixel(x, y, LCD_textColor);
}

//February
void Pixel(int16_t x, int16_t y, int16_t c)
{
	if ((x >= LCD_PIXEL_WIDTH) || (y >= LCD_PIXEL_HEIGHT))
		return;

	// Set location to write to.
	// ** CORRECTED TO BE X Y **
//	LCD_SetCursor(y, x);
	LCD_SetCursor(x, y);

	// Write one value to LCD RAM, setting the pixel.
	LCD_WriteRAM_Prepare();
	LCD_WriteRAM(c);
}

//void Display_ULong ( uint16_t data, uint8_t XL, uint8_t YL)	{

//	XL = XL+32; // Based on 16 bit being limited to 65000( 5-1 places * 8 pixels)

//	while ( data > 0 )	{

//		//LCD_Write_Character ( ( data % 10 ) + 0x30, col--, row, invert );
//		LCD_PutChar(XL, YL, ( data % 10+ 0x30 ));
//        XL  -= 8;
//		data	/=	10;
//		}
//	}

//February
void LCD_PutChar(int16_t PosX, int16_t PosY, char c)
{
	uint8_t i = 0;
	uint8_t j = 0;
	if (asciisize == 8) {
		uint8_t Buffer[8];
		uint8_t TmpChar = 0;

		GetASCIICode1(Buffer, c);
		for (i = 0; i < 8; i++) {
			TmpChar = Buffer[i];
			for (j = 0; j < 8; j++) {
				if (((TmpChar >> (7 - j)) & 0x01) == 0x01) {
					Pixel(PosX + j, PosY + (7 - i), LCD_textColor);
				}
				else {
					Pixel(PosX + j, PosY + (7 - i), LCD_backColor);
				}
			}
		}
	}
	//----------------------------------------------------------------------------
	if (asciisize == 12) {
		uint8_t Buffer[12];
		uint8_t TmpChar = 0;

		GetASCIICode2(Buffer, c);
		for (i = 0; i < 12; i++) {
			TmpChar = Buffer[i];
			for (j = 0; j < 8; j++) {
				if (((TmpChar >> (7 - j)) & 0x01) == 0x01) {
					Pixel(PosX + j, PosY + (11 - i), LCD_textColor);
				}
				else {
					Pixel(PosX + j, PosY + (11 - i), LCD_backColor);
				}
			}
		}
	}
	//----------------------------------------------------------------------------
	if (asciisize == 16) {
		unsigned char Buffer[16];
		uint8_t TmpChar = 0;

		GetASCIICode4(Buffer, c);
		for (i = 0; i < 16; i++) {
			TmpChar = Buffer[i];
			for (j = 0; j < 8; j++) {
				if (((TmpChar >> (7 - j)) & 0x01) == 0x01) {
					Pixel(PosX + j, PosY + (15 - i), LCD_textColor);
				}
				else {
					Pixel(PosX + j, PosY + (15 - i), LCD_backColor);
				}
			}
		}
	}
	//----------------------------------------------------------------------------
	if (asciisize == 14) {
		short int Buffer[12];
		uint16_t TmpChar = 0;

		GetASCIICode3(Buffer, c);
		for (i = 0; i < 12; i++) {
			TmpChar = Buffer[i];
			for (j = 0; j < 12; j++) {
				if (((TmpChar >> (11 - j)) & (0x01)) == 0x01) {
					Pixel(PosX + j, PosY + (11 - i), LCD_textColor);
				}
				else {
					Pixel(PosX + j, PosY + (11 - i), LCD_backColor);
				}
			}
		}
	}
	//----------------------------------------------------------------------------
	if (asciisize == 24) {
		short int Buffer[24];
		uint16_t TmpChar = 0;
		GetASCIICode5(Buffer, c);
		for (i = 0; i < 24; i++) {
			TmpChar = Buffer[i];
			for (j = 0; j < 16; j++) {
				if (((TmpChar >> j) & (0x01)) == 0x01) {
					Pixel(PosX + j, PosY + (23 - i), LCD_textColor);
				}
				else {
					Pixel(PosX + j, PosY + (23 - i), LCD_backColor);
				}
			}
		}
	}
	//----------------------------------------------------------------------------
}

//February
void LCD_StringLine(uint16_t PosX, uint16_t PosY, char *str)
{
	char TempChar;

	do {
		TempChar = *str++;
		LCD_PutChar(PosX, PosY, TempChar);

		// Word-wrap: If no room left on line, move to next line (if possible)
		if (PosX < 312) {
			PosX += 8;
			if (asciisize == 24) {
				PosX += 8;
			}
			else if (asciisize == 14) {
				PosX += 4;
			}
		}

		else if (PosY < 224) {
			PosX = 0;
			PosY += 16;
		}
		else {
			PosX = 0;
			PosY = 0;
		}
	} while (*str != 0);
}

// Display the FFT data to the screen.
// Expect fftData[] to be an array of 256 ints.
void LCD_DrawFFT(uint8_t fftData[])
{
	uint32_t i;
	uint32_t j;
	float temp;

	temp = (float) (NCO_Frequency - 125) / 15.625;

	for (i = 0; i < 240; i++) {
		if (i != temp) {

			for (j = 0; j < 64; j++) {
				if (j < fftData[i + 8])
					Pixel(i + 40, 175 + j, BLUE);
				else
					Pixel(i + 40, 175 + j, WHITE);
			}
		}
	}

	for (j = 0; j < 60; j++)
		Pixel((int) temp + 40, 179 + j, RED);
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

void LCD_CtrlLinesConfig(void)
{

	GPIO_InitTypeDef GPIO_InitStructure;

	//Added Explicit Reset on Pin B1
	/* Enable LCD Reset GPIO clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	/* LCD reset pin configuration -------------------------------------------------*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	//

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE, ENABLE);
	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);

	GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC );		// D2
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC );		// D3
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC );		// NOE -> RD
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC );		// NWE -> WR
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource7, GPIO_AF_FSMC );		// NE1 -> CS
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FSMC );		// D13
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FSMC );		// D14
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FSMC );		// D15
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource11, GPIO_AF_FSMC );		// A16 -> RS
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC );		// D0
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC );		// D1

	GPIO_PinAFConfig(GPIOE, GPIO_PinSource7, GPIO_AF_FSMC );		// D4
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource8, GPIO_AF_FSMC );		// D5
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF_FSMC );		// D6
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource10, GPIO_AF_FSMC );		// D7
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF_FSMC );		// D8
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource12, GPIO_AF_FSMC );		// D9
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF_FSMC );		// D10
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource14, GPIO_AF_FSMC );		// D11
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource15, GPIO_AF_FSMC );		// D12

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4
			| GPIO_Pin_5 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10
			| GPIO_Pin_11 | GPIO_Pin_14 | GPIO_Pin_15;

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	//GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	//GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9
			| GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13
			| GPIO_Pin_14 | GPIO_Pin_15;

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	//GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	//GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
}

void LCD_FSMCConfig(void)
{
	FSMC_NORSRAMInitTypeDef FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef FSMC_NORSRAMTimingInitStructure;
	FSMC_NORSRAMTimingInitStructure.FSMC_AddressSetupTime = 0;  //0
	FSMC_NORSRAMTimingInitStructure.FSMC_AddressHoldTime = 0;   //0
	FSMC_NORSRAMTimingInitStructure.FSMC_DataSetupTime = 2;     //3
	FSMC_NORSRAMTimingInitStructure.FSMC_BusTurnAroundDuration = 0;
	FSMC_NORSRAMTimingInitStructure.FSMC_CLKDivision = 1;     //1
	FSMC_NORSRAMTimingInitStructure.FSMC_DataLatency = 0;
	FSMC_NORSRAMTimingInitStructure.FSMC_AccessMode = FSMC_AccessMode_A;

	FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
	FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Enable; //disable
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &FSMC_NORSRAMTimingInitStructure;

	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);
	FSMC_NORSRAMTimingInitStructure.FSMC_AddressSetupTime = 0;    //0
	FSMC_NORSRAMTimingInitStructure.FSMC_AddressHoldTime = 0;	//0
	FSMC_NORSRAMTimingInitStructure.FSMC_DataSetupTime = 4;	//3
	FSMC_NORSRAMTimingInitStructure.FSMC_BusTurnAroundDuration = 0;
	FSMC_NORSRAMTimingInitStructure.FSMC_CLKDivision = 1;	//1
	FSMC_NORSRAMTimingInitStructure.FSMC_DataLatency = 0;
	FSMC_NORSRAMTimingInitStructure.FSMC_AccessMode = FSMC_AccessMode_A;
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &FSMC_NORSRAMTimingInitStructure;
	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);

	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);
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

void LCD_FillPolyLine(pPoint Points, uint16_t PointCount)
{
	/*  public-domain code by Darel Rex Finley, 2007 */
	uint16_t nodes = 0, nodeX[MAX_POLY_CORNERS], pixelX = 0, pixelY = 0, i = 0, j = 0, swap = 0;
	uint16_t IMAGE_LEFT = 0, IMAGE_RIGHT = 0, IMAGE_TOP = 0, IMAGE_BOTTOM = 0;

	IMAGE_LEFT = IMAGE_RIGHT = Points->X;
	IMAGE_TOP = IMAGE_BOTTOM = Points->Y;

	for (i = 1; i < PointCount; i++) {
		pixelX = POLY_X(i);
		if (pixelX < IMAGE_LEFT) {
			IMAGE_LEFT = pixelX;
		}
		if (pixelX > IMAGE_RIGHT) {
			IMAGE_RIGHT = pixelX;
		}

		pixelY = POLY_Y(i);
		if (pixelY < IMAGE_TOP) {
			IMAGE_TOP = pixelY;
		}
		if (pixelY > IMAGE_BOTTOM) {
			IMAGE_BOTTOM = pixelY;
		}
	}

	LCD_SetTextColor(LCD_backColor);

	/*  Loop through the rows of the image. */
	for (pixelY = IMAGE_TOP; pixelY < IMAGE_BOTTOM; pixelY++) {
		/* Build a list of nodes. */
		nodes = 0;
		j = PointCount - 1;

		for (i = 0; i < PointCount; i++) {
			if (((POLY_Y(i) < (double) pixelY) && (POLY_Y(j) >= (double) pixelY))
					|| ((POLY_Y(j) < (double) pixelY) && (POLY_Y(i) >= (double) pixelY)))
			{
				nodeX[nodes++] = (int) (POLY_X(i)
						+ ((pixelY - POLY_Y(i) )*(POLY_X(j)-POLY_X(i)))/(POLY_Y(j)-POLY_Y(i)));
			}
			j = i;
		}

		/* Sort the nodes, via a simple "Bubble" sort. */
		i = 0;
		while (i < nodes - 1) {
			if (nodeX[i] > nodeX[i + 1]) {
				swap = nodeX[i];
				nodeX[i] = nodeX[i + 1];
				nodeX[i + 1] = swap;
				if (i) {
					i--;
				}
			}
			else {
				i++;
			}
		}

		/*  Fill the pixels between node pairs. */
		for (i = 0; i < nodes; i += 2) {
			if (nodeX[i] >= IMAGE_RIGHT) {
				break;
			}
			if (nodeX[i + 1] > IMAGE_LEFT) {
				if (nodeX[i] < IMAGE_LEFT) {
					nodeX[i] = IMAGE_LEFT;
				}
				if (nodeX[i + 1] > IMAGE_RIGHT) {
					nodeX[i + 1] = IMAGE_RIGHT;
				}
				LCD_SetTextColor(LCD_backColor);
				LCD_DrawLine(pixelY, nodeX[i + 1], nodeX[i + 1] - nodeX[i],
						LCD_DIR_HORIZONTAL);
				LCD_SetTextColor(LCD_textColor);
				PutPixel(pixelY, nodeX[i + 1]);
				PutPixel(pixelY, nodeX[i]);
				/* for (j=nodeX[i]; j<nodeX[i+1]; j++) PutPixel(j,pixelY); */
			}
		}
	}

	/* draw the edges */
	LCD_SetTextColor(LCD_textColor);
}

void LCD_BackLight(int procentai)
{
	if (procentai > 100) {
		procentai = 100;
	}
	else if (procentai < 0) {
		procentai = 0;
	}
	Channel1Pulse =
			(uint16_t) (((uint32_t) procentai * (TimerPeriod - 1)) / 100);
	TIM_TimeBaseStructure.TIM_Period = TimerPeriod;
	TIM_OCInitStructure.TIM_Pulse = Channel1Pulse;
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);
}

void TIM_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_TIM1 );
	TimerPeriod = (SystemCoreClock / 17570) - 1;
	Channel1Pulse = (uint16_t) (((uint32_t) 99 * (TimerPeriod - 1)) / 100);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = TimerPeriod;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
	//TIM_OCInitStructure.TIM_Pulse = Channel3Pulse;
	TIM_OCInitStructure.TIM_Pulse = Channel1Pulse;
	TIM_OC3Init(TIM1, &TIM_OCInitStructure);
	TIM_Cmd(TIM1, ENABLE);
	TIM_CtrlPWMOutputs(TIM1, ENABLE);

}

void TFT_Delay(uint32_t nTime)
{
	static __IO uint32_t TimingDelay;
	TimingDelay = nTime;

	while (TimingDelay != 0) {
		TimingDelay--;
	}
}

void Plot_String(uint8_t *string, uint8_t x, uint8_t y)
{
	const int CHARACTER_WIDTH = 8;

	while (*string) {
		LCD_PutChar(x, y, *string++);
		x += CHARACTER_WIDTH;
	}
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
	LCD_SetTextColor(BLACK);
	LCD_StringLine(x, y, buffer);

	// Now display the part of the number changing
	buffer[numCharChangingByDial(changeRate)] = 0;
	LCD_SetTextColor(BLUE);
	LCD_StringLine(x, y, buffer);

	// Reset color to black for next text write.
	LCD_SetTextColor(BLACK);
}

