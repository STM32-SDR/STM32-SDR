/*
 * Code for LCD Driver for ILI9320 chip
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


#include <assert.h>
#include <stdlib.h>
#include "stm32_eval.h"
#include "LcdDriver_ILI9320.h"
#include "sdr_image.h"
//example error correction
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_exti.h"
#include "misc.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_syscfg.h"
#include "stm32f4xx_fsmc.h"
#include "xprintf.h"

// *********************************************************************************
// Register Settings for LCD
// *********************************************************************************
#define LCD_REG      (*((volatile unsigned short *) 0x60000000))
#define LCD_RAM      (*((volatile unsigned short *) 0x60020000))


// Work with the direct LCD RAM access & rotation
#define R3_CMALWAYS_SET	  0x1000
#define R3_VERTICAL_BIT   5		// Set 0 for Increment, 1 for decrement
#define R3_HORIZONTAL_BIT 4		// Set 0 for Increment, 1 for decrement
#define R3_ADDRESS_BIT    3		// Set 1 for vertical, 0 for horizontal

#define R3_INCRCEMENT     1		// For X and Y directions.
#define R3_DECREMENT      0
#define R3_CHANGEADDR_X   0		// Does address change in X or Y first?
#define R3_CHANGEADDR_Y   1

/* Example: */
/* Set GRAM write direction and BGR = 1 */
/* I/D=00 (Horizontal : increment, Vertical : increment) */
/* AM=1 (address is updated in horizontal writing direction) */
#if LCD_ROTATION == 0
#define LCD_ROTATE_GETX(x,y) (x)
#define LCD_ROTATE_GETY(x,y) (y)
#define BUILD_R3_CMD(horizontalIncDec, verticalIncDec, addressDir) \
		(R3_CMALWAYS_SET | (verticalIncDec << R3_VERTICAL_BIT) | (horizontalIncDec << R3_HORIZONTAL_BIT) | (addressDir << R3_ADDRESS_BIT))
#elif LCD_ROTATION == 90
#define LCD_ROTATE_GETX(x,y) (y)
#define LCD_ROTATE_GETY(x,y) (LCD_ACTUAL_SCREEN_HEIGHT - (x) - 1)
#define BUILD_R3_CMD(horizontalIncDec, verticalIncDec, addressDir) \
		((R3_CMALWAYS_SET) | (!(horizontalIncDec) << R3_VERTICAL_BIT) | ((verticalIncDec) << R3_HORIZONTAL_BIT) | (!(addressDir) << R3_ADDRESS_BIT))
#elif LCD_ROTATION == 180
#define LCD_ROTATE_GETX(x,y) (LCD_ACTUAL_SCREEN_WIDTH - (x) - 1)
#define LCD_ROTATE_GETY(x,y) (LCD_ACTUAL_SCREEN_HEIGHT - (y) - 1)
#define BUILD_R3_CMD(horizontalIncDec, verticalIncDec, addressDir) \
		((R3_CMALWAYS_SET) | (!(verticalIncDec) << R3_VERTICAL_BIT) | (!(horizontalIncDec) << R3_HORIZONTAL_BIT) | ((addressDir) << R3_ADDRESS_BIT))
#elif LCD_ROTATION == 270
#define LCD_ROTATE_GETX(x,y) (LCD_ACTUAL_SCREEN_WIDTH - (y) - 1)
#define LCD_ROTATE_GETY(x,y) (x)
#define BUILD_R3_CMD(horizontalIncDec, verticalIncDec, addressDir) \
		((R3_CMALWAYS_SET) | ((horizontalIncDec) << R3_VERTICAL_BIT) | (!(verticalIncDec) << R3_HORIZONTAL_BIT) | (!(addressDir) << R3_ADDRESS_BIT))
#else
#error Must define LCD_ROTATION to be a direction
#endif


// Colours for Monochrome images.
__IO uint16_t LCD_textColor = 0x0000;
__IO uint16_t LCD_backColor = 0xFFFF;


// Private functions
static void TFT_Delay(uint32_t nTime);
static void LCD_Reset(void);
static void LCD_CtrlLinesConfig(void);
static void LCD_FSMCConfig(void);
static void TIM_Config(void);


void __assert_func(const char *file, int line, const char *assertFunc, const char *e)
{
	xprintf("\n\n\n\n");
	xprintf("******************************\n");
	xprintf("ASSERT FAILED!\n");
	xprintf("******************************\n");
	volatile int bugger = 0;
	while (1) {
		xprintf("Assert failed:\n");
		xprintf("   File %s, Line %d\n", file, line);
		xprintf("   Function  %s\n", assertFunc);
		xprintf("   Condition %s\n", e);
		bugger ++;
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//		Initialization
//////////////////////////////////////////////////////////////////////////////////////////////////////


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

	LCD_WriteReg(0x00E5, 0x8000); //
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




static void TFT_Delay(uint32_t nTime)
{
	static volatile int threadWatch = 0;
	threadWatch++;
	assert(threadWatch == 1);

	static __IO uint32_t TimingDelay;
	TimingDelay = nTime;

	while (TimingDelay != 0) {
		TimingDelay--;
	}
	assert(threadWatch == 1);
	threadWatch--;
	assert(threadWatch == 0);
}

static void LCD_CtrlLinesConfig(void)
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


static TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
static TIM_OCInitTypeDef TIM_OCInitStructure;
static void TIM_Config(void)
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

	uint16_t TimerPeriod = (SystemCoreClock / 17570) - 1;
	uint16_t Channel1Pulse = (uint16_t) (((uint32_t) 99 * (TimerPeriod - 1)) / 100);
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


void LCD_BackLight(int procentOn)
{
	if (procentOn > 100) {
		procentOn = 100;
	}
	if (procentOn < 0) {
		procentOn = 0;
	}

	uint16_t TimerPeriod = (SystemCoreClock / 17570) - 1;
	uint16_t channel1Pulse =
			(uint16_t) (((uint32_t) procentOn * (TimerPeriod - 1)) / 100);
	TIM_TimeBaseStructure.TIM_Period = TimerPeriod;
	TIM_OCInitStructure.TIM_Pulse = channel1Pulse;
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);
}

// Configure the Flexible Static Memory Controller to map the LCD RAM to processor RAM.
static void LCD_FSMCConfig(void)
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

static void LCD_Reset(void)
{
	/* Power Down the reset */
	GPIO_WriteBit(GPIOB, GPIO_Pin_1, Bit_RESET);

	/* wait for a delay to insure registers erasing */
	TFT_Delay(0x4FFF);

	/* Power on the reset */
	GPIO_WriteBit(GPIOB, GPIO_Pin_1, Bit_SET);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//		From LcdHal.c
//////////////////////////////////////////////////////////////////////////////////////////////////////
__IO uint32_t LCDType = LCD_ILI9320;


/**
 * @brief  LCD_DrawColorBMP
 * @param  *ptrBitmap:   The pointer to the image
 * @param  Xpos_Init: The X axis position
 * @param  Ypos_Init: The Y axis position
 * @param  Height:    The Height of the image
 * @param  Width:     The Width of the image
 * @retval None
 */
void LCD_DrawColorBMP(uint8_t* ptrBitmap, uint16_t Xpos_Init, uint16_t Ypos_Init, uint16_t Height, uint16_t Width)
{
#if 0
	uint32_t uDataAddr = 0, uBmpSize = 0;
	uint16_t uBmpData = 0;

	BmpBuffer32BitRead(&uBmpSize, ptrBitmap + 2);
	BmpBuffer32BitRead(&uDataAddr, ptrBitmap + 10);

	if (LCD_Direction == _0_degree) {
		GL_SetDisplayWindow(Xpos_Init, Ypos_Init, Height, Width);
		/* Set GRAM write direction and BGR = 1 */
		/* I/D=00 (Horizontal : decrement, Vertical : decrement) */
		/* AM=1 (address is updated in vertical writing direction) */
		LCD_WriteReg(R3, MAKE_R3_CMD(0x1008));
	}
	else if (LCD_Direction == _90_degree) {
		GL_SetDisplayWindow(Xpos_Init + Width - 1, Ypos_Init, Width, Height);
		/* Set GRAM write direction and BGR = 1 */
		/* I/D=01 (Horizontal : increment, Vertical : decrement) */
		/* AM=0 (address is updated in horizontal writing direction) */
		LCD_WriteReg(R3, MAKE_R3_CMD(0x1010));
	}
	else if (LCD_Direction == _180_degree) {
		GL_SetDisplayWindow(Xpos_Init + Height - 1, Ypos_Init + Width - 1, Height, Width);
		/* Set GRAM write direction and BGR = 1 */
		/* I/D=11 (Horizontal : increment, Vertical : increment) */
		/* AM=1 (address is updated in vertical writing direction) */
		LCD_WriteReg(R3, MAKE_R3_CMD(0x1038));
	}
	else if (LCD_Direction == _270_degree) {
		GL_SetDisplayWindow(Xpos_Init, Ypos_Init + Height - 1, Width, Height);
		/* Set GRAM write direction and BGR = 1 */
		/* I/D=10 (Horizontal : decrement, Vertical : increment) */
		/* AM=0 (address is updated in horizontal writing direction) */
		LCD_WriteReg(R3, MAKE_R3_CMD(0x1020));
	}

	LCD_SetCursor(Xpos_Init, Ypos_Init);

	if ((LCDType == LCD_ILI9320) || (LCDType == LCD_SPFD5408)) {
		/* Prepare to write GRAM */
		LCD_WriteRAM_Prepare();
	}

	/* Read bitmap data and write them to LCD */
	for (; uDataAddr < uBmpSize; uDataAddr += 2) {
		uBmpData = (uint16_t) (*(ptrBitmap + uDataAddr)) + (uint16_t) ((*(ptrBitmap + uDataAddr + 1)) << 8);
		LCD_WriteRAM(uBmpData);
	}
	if ((LCDType == LCD_ILI9320) || (LCDType == LCD_SPFD5408)) {
		if (pLcdHwParam.LCD_Connection_Mode == GL_SPI) {
			GL_LCD_CtrlLinesWrite(pLcdHwParam.LCD_Ctrl_Port_NCS, pLcdHwParam.LCD_Ctrl_Pin_NCS, GL_HIGH);
		}
	}
	LCD_Change_Direction(LCD_Direction);
	GL_SetDisplayWindow(LCD_Width - 1, LCD_Height - 1, LCD_Height, LCD_Width);
#endif
}




//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//		From TFT_Display.c
//
//
//////////////////////////////////////////////////////////////////////////////////////////////////////


#define DEBUG_REGMISMATCH 0
#if DEBUG_REGMISMATCH == 1
volatile int threadWatch_LCDREG = 0;
volatile int LCDRegMismatchCount_A1 = 0;
volatile int LCDRegMismatchCount_A2 = 0;
volatile int LCDRegMismatchCount_A3 = 0;
volatile int LCDRegMismatchCount_A4 = 0;
volatile uint8_t LCDRegMismatch_BadReg = 0;
volatile int LCDRegMismatchCount_B1 = 0;
volatile int LCDRegMismatchCount_B2 = 0;
volatile int LCDRegMismatchCount_B3 = 0;
volatile int LCDRegMismatchCount_B4 = 0;
#endif

// ***********************************************************************************
// Register access routines
// ***********************************************************************************
void LCD_WriteReg(uint8_t selectedReg, uint16_t regValue)
{
	// Catch threadsafe problem.
	static uint8_t threadSafeCheck = 0;
	assert(threadSafeCheck == 0);
	threadSafeCheck++;
	assert(threadSafeCheck == 1);

	// Loop until the value is set correctly
	// Note: This is a bit of a hack! But it works...
	uint8_t count = 0;
	do {
		LCD_REG = selectedReg;
		if (count++ > 100) {
			assert(0);
			break;
		}
	} while (LCD_REG != selectedReg);

	LCD_RAM = regValue;

	// Catch threadsafe problem.
	assert(threadSafeCheck == 1);
	threadSafeCheck--;
	assert(threadSafeCheck == 0);
}
void LCD_WriteRAM_Prepare(void)
{
	// Catch threadsafe problem.
	static uint8_t threadSafeCheck = 0;
	assert(threadSafeCheck == 0);
	threadSafeCheck++;
	assert(threadSafeCheck == 1);

#if DEBUG_REGMISMATCH == 1
	LCD_REG = 0x22;
	if (LCD_REG != 0x22) {
		LCDRegMismatchCount_B1++;
		if (LCD_REG != 0x22) {
			LCDRegMismatchCount_B2++;
			if (LCD_REG != 0x22) {
				LCDRegMismatchCount_B3++;
				LCD_REG = 0x22;
				if (LCD_REG != 0x22) {
					LCDRegMismatchCount_B4++;
				}
			}
		}
	}
#else
	// Loop until the value is set correctly
	uint8_t count = 0;
	do {
		LCD_REG = 0x22;
		if (count++ > 100) {
			assert(0);
			return;
		}
	} while (LCD_REG != 0x22);
#endif

	// Catch threadsafe problem.
	assert(threadSafeCheck == 1);
	threadSafeCheck--;
	assert(threadSafeCheck == 0);
}

void LCD_WriteRAM_PrepareDir(LCD_WriteRAM_Direction direction)
{
	uint16_t r3Cmd = 0;

	switch (direction) {
	case LCD_WriteRAMDir_Right:
		r3Cmd = BUILD_R3_CMD(R3_INCRCEMENT, R3_INCRCEMENT, R3_CHANGEADDR_X);
		break;
	case LCD_WriteRAMDir_Left:
		r3Cmd = BUILD_R3_CMD(R3_DECREMENT, R3_INCRCEMENT, R3_CHANGEADDR_X);
		break;
	case LCD_WriteRAMDir_Up:
		r3Cmd = BUILD_R3_CMD(R3_INCRCEMENT, R3_DECREMENT, R3_CHANGEADDR_Y);
		break;
	case LCD_WriteRAMDir_Down:
		r3Cmd = BUILD_R3_CMD(R3_INCRCEMENT, R3_INCRCEMENT, R3_CHANGEADDR_Y);
		break;
	default:
		assert(0);
	}
	LCD_WriteReg(R3, r3Cmd);

	LCD_WriteRAM_Prepare();
}
void LCD_WriteRAM(uint16_t RGB_Code)
{
	LCD_RAM = RGB_Code;

	// TODO: Why is this delay needed? Without it, nothing appears on LCD!??
	//	TFT_Delay(50);
	TFT_Delay(10);
}



uint16_t LCD_ReadReg(uint8_t LCD_Reg)
{
	// TODO: Not sure if this works; copied from a different file (stm32100e_eval_lcd.c).
	/* Write 16-bit Index (then Read Reg) */
	LCD_REG = LCD_Reg;
	/* Read 16-bit Reg */
	uint16_t regValue = LCD_RAM;
	regValue = LCD_RAM;
	regValue = LCD_RAM;
	return regValue;
}
uint16_t LCD_ReadRAM(void)
{
    return LCD_ReadReg(R34);
}

// ***********************************************************************************
// Register access routines
// ***********************************************************************************
/**
 * @brief  Sets a display window used for RAM writes. When calling this function to restrict
 *         the write window from within a function, that function should also re-call this
 *         function to reset the write-window back to the full screen.
 * @param  x: specifies the X top left corner.
 * @param  y: specifies the Y top left corner.
 * @param  height: display window height.
 * @param  width: display window width.
 * @retval None
 */
void LCD_SetDisplayWindow(uint16_t x, uint16_t y, uint16_t height, uint16_t width)
{
	uint16_t rotatedY1 = LCD_ROTATE_GETY(x, y);
	uint16_t rotatedY2 = LCD_ROTATE_GETY(x + width - 1, y + height - 1);
	uint16_t rotatedX1 = LCD_ROTATE_GETX(x, y);
	uint16_t rotatedX2 = LCD_ROTATE_GETX(x + width - 1, y + height - 1);

	#define MAX(a, b) ((a)>(b)?(a):(b))
	#define MIN(a, b) ((a)<(b)?(a):(b))

	uint16_t xStart = MIN(rotatedX1, rotatedX2);
	uint16_t xEnd   = MAX(rotatedX1, rotatedX2);
	uint16_t yStart = MIN(rotatedY1, rotatedY2);
	uint16_t yEnd   = MAX(rotatedY1, rotatedY2);

	xStart = MAX(0, xStart);
	xEnd   = MIN(LCD_ACTUAL_SCREEN_WIDTH-1, xEnd);
	yStart = MAX(0, yStart);
	yEnd   = MIN(LCD_ACTUAL_SCREEN_HEIGHT-1, yEnd);

	LCD_WriteReg(0x0050, xStart); // Window X start address
	LCD_WriteReg(0x0051, xEnd); // Window X end address
	LCD_WriteReg(0x0052, yStart); // Window Y start address
	LCD_WriteReg(0x0053, yEnd); // Window Y end address

	LCD_SetCursor(x, y);
}
void LCD_ResetDisplayWindow(void)
{
	LCD_SetDisplayWindow(0, 0, LCD_HEIGHT, LCD_WIDTH);
}
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
{
	LCD_WriteReg(0x20, LCD_ROTATE_GETX(Xpos, Ypos));
	LCD_WriteReg(0x21, LCD_ROTATE_GETY(Xpos, Ypos));
}

void LCD_DisplayOn(void)
{
	LCD_WriteReg(0x07, 0x0173);
}
void LCD_DisplayOff(void)
{
	LCD_WriteReg(0x07, 0x0000);
}



void LCD_PutPixel(uint16_t Xpos, uint16_t Ypos, uint16_t Color)
{
	LCD_SetCursor(Xpos, Ypos);
	LCD_WriteRAM_Prepare();
	LCD_WriteRAM(Color);
}

uint16_t LCD_GetPixel(uint16_t Xpos, uint16_t Ypos)
{
	// TODO: Unable to have GetPixel working.
	// - Data sheet suggests have to read ram twice, but does not seem to help.
	// - Perhaps TTY debugging would help be able to display values quickly?
#if 0
	uint16_t tmpColor = 0, tmpRed = 0, tmpBlue = 0;

	/*Read the color of given pixel*/
	LCD_WriteRAM_PrepareDir(LCD_WriteRAMDir_Right);
	LCD_SetCursor(Xpos, Ypos);
	tmpColor = LCD_ReadRAM();

	/*Swap the R and B color channels*/
	tmpRed = (tmpColor & Blue) << 11;
	tmpBlue = (tmpColor & Red) >> 11;
	tmpColor &= ~(Red | Blue);
	tmpColor |= (tmpRed | tmpBlue);

	return tmpColor;
#else
	return 0;
#endif
}


// ***********************************************************************************
// Drawing Routines
// ***********************************************************************************
/*
 * Draw a 16-bit bitmap to the screen.
 *  Image Generation Tip:
 *  	Use Gimp, load image, File --> Export, type C source code,
 *  	"Save as RGB454 (16-bit)"
 */
void LCD_DrawBMP16Bit(int x, int y, int height, int width, const uint16_t* pBitmap, _Bool revByteOrder)
{
	int numPixels = width * height;

	LCD_SetDisplayWindow(x, y, height, width);
	LCD_WriteRAM_PrepareDir(LCD_WriteRAMDir_Right);

	// Push bitmap data to screen.
	for (int wordNum = 0; wordNum < numPixels; wordNum++) {
		uint16_t color = (*(pBitmap + wordNum));
		if (revByteOrder) {
			color = ((color & 0x00FF) << 8) | ((color & 0xFF00) >> 8);
		}
		LCD_WriteRAM( color );
	}
	LCD_ResetDisplayWindow();
}


// Draw a 1-bit bitmap (monochrome) to the screen.
// Assume each row of bits starts on a new word (i.e., pad last word per row with 0 bits)
void LCD_DrawBMP1Bit(int x, int y, int height, int width, const uint16_t* pBitmap, _Bool revBitOrder)
{
	LCD_SetDisplayWindow(x, y, height, width);
	LCD_WriteRAM_PrepareDir(LCD_WriteRAMDir_Right);

	// Push bitmap data to screen.
	int word = 0;
	for (int row = 0; row < height; row++) {
		for (int col = 0; col < width; col++) {

			// On screen?
			if (x < 0 || x + col >= LCD_WIDTH || y < 0 || y + row >= LCD_HEIGHT) {
				continue;
			}

			// Setup the word:
			if (col % 16 == 0 && col > 0) {
				word++;
			}

			int bit = col % 16;
			if (revBitOrder) {
				bit = 15 - bit;
			}

			uint16_t turnOn = pBitmap[word] & (1 << bit);
			if (turnOn) {
				LCD_WriteRAM( LCD_textColor );
			} else {
				LCD_WriteRAM( LCD_backColor );
			}
		}
		// Assume next row starts on new byte:
		word++;
	}
	LCD_ResetDisplayWindow();
}

// Draw a 1-bit bitmap transparent (monochrome) to the screen.
// Assume each row of bits starts on a new word (i.e., pad last word per row with 0 bits)
// TODO: If this is fast enough, use the pixel-by-pixel approach for transparent and non (reduce duplication)
void LCD_DrawBMP1BitTransparent(int x, int y, int height, int width, const uint16_t* pBitmap, _Bool revBitOrder)
{
	// Push bitmap data to screen.
	int word = 0;
	for (int row = 0; row < height; row++) {
		for (int col = 0; col < width; col++) {

			// On screen?
			if (x < 0 || x + col >= LCD_WIDTH || y < 0 || y + row >= LCD_HEIGHT) {
				continue;
			}

			// Setup the word:
			if (col % 16 == 0 && col > 0) {
				word++;
			}

			int bit = col % 16;
			if (revBitOrder) {
				bit = 15 - bit;
			}

			uint16_t turnOn = pBitmap[word] & (1 << bit);
			if (turnOn) {
				LCD_PutPixel(x + col, y + row, LCD_textColor);
			} else {
				// Nothing; it's transparent!
			}
		}
		// Assume next row starts on new byte:
		word++;
	}
}

void LCD_SetTextColor(__IO uint16_t Color)
{
	LCD_textColor = Color;
}

void LCD_SetBackColor(__IO uint16_t Color)
{
	LCD_backColor = Color;
}

void LCD_Clear(uint16_t Color)
{
	LCD_ResetDisplayWindow();
	LCD_WriteRAM_PrepareDir(LCD_WriteRAMDir_Right);

	for (uint16_t y = 0; y < LCD_HEIGHT; y++) {
		for (uint16_t x = 0; x < LCD_WIDTH; x++) {
			LCD_WriteRAM(Color);
		}
	}
}

void LCD_DrawLine(uint16_t xStart, uint16_t yStart, uint16_t length, LCD_WriteRAM_Direction direction)
{
	LCD_SetCursor(xStart, yStart);
	LCD_WriteRAM_PrepareDir(direction);
	for (uint16_t i = 0; i < length; i++) {
		LCD_WriteRAM(LCD_textColor);
	}
}

void LCD_DrawRect(uint16_t Xpos, uint16_t Ypos, uint8_t Height, uint16_t Width)
{
	LCD_DrawLine(Xpos, Ypos, Width, LCD_WriteRAMDir_Right);
	LCD_DrawLine(Xpos, Ypos + Height - 1, Width, LCD_WriteRAMDir_Right);
	LCD_DrawLine(Xpos, Ypos, Height, LCD_WriteRAMDir_Down);
	LCD_DrawLine(Xpos + Width - 1, Ypos, Height, LCD_WriteRAMDir_Down);
}

void LCD_DrawFilledRect(int x, int y, int height, int width)
{
	int numPixels = width * height;

	LCD_SetDisplayWindow(x, y, height, width);
	LCD_WriteRAM_PrepareDir(LCD_WriteRAMDir_Right);

	// Push pixels to the screen
	for (int wordNum = 0; wordNum < numPixels; wordNum++) {
		LCD_WriteRAM(LCD_textColor);
	}
	LCD_ResetDisplayWindow();
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//		Test functions
//////////////////////////////////////////////////////////////////////////////////////////////////////
void LCD_TestDisplayScreen(void)
{
	// Test clear screen
	LCD_Clear(LCD_COLOR_BLUE);
	LCD_Clear(LCD_COLOR_RED);
	LCD_Clear(LCD_COLOR_GREEN);
	LCD_Clear(LCD_COLOR_YELLOW);
	LCD_Clear(LCD_COLOR_BLACK);

	// Test horizontal and vertical lines
	// Lines from origin (top-left)
	LCD_SetTextColor(LCD_COLOR_WHITE);
	LCD_DrawLine(0,0, 100, LCD_WriteRAMDir_Right);
	LCD_DrawLine(0,10, 100, LCD_WriteRAMDir_Right);
	LCD_SetTextColor(LCD_COLOR_YELLOW);
	LCD_DrawLine(0,0, 100, LCD_DIR_VERTICAL);

	// Lines into bottom right corner.
	LCD_SetTextColor(LCD_COLOR_RED);
	LCD_DrawLine(LCD_WIDTH-200-1,LCD_HEIGHT-1, 200, LCD_WriteRAMDir_Right);
	LCD_SetTextColor(LCD_COLOR_GREEN);
	LCD_DrawLine(LCD_WIDTH-1,LCD_HEIGHT-50-1, 50, LCD_WriteRAMDir_Down);

	// Draw + in middle of screen
	int midX = LCD_WIDTH / 2;
	int midY = LCD_HEIGHT / 2;
	int crossLen = 50;
	LCD_SetTextColor(LCD_COLOR_RED);
	LCD_DrawLine(midX, midY, crossLen, LCD_WriteRAMDir_Right);
	LCD_SetTextColor(LCD_COLOR_GREEN);
	LCD_DrawLine(midX, midY, crossLen, LCD_WriteRAMDir_Down);
	LCD_SetTextColor(LCD_COLOR_BLUE);
	LCD_DrawLine(midX, midY, crossLen, LCD_WriteRAMDir_Left);
	LCD_SetTextColor(LCD_COLOR_YELLOW);
	LCD_DrawLine(midX, midY, crossLen, LCD_WriteRAMDir_Up);

	LCD_SetBackColor(LCD_COLOR_BLUE);
	uint16_t data[] = {
			0xFF, 0x00, 0xCC, 0x33, 0xAA, 0x55,
			0xFF, 0x00, 0xCC, 0x33, 0xAA, 0x55,
			0xFF, 0x00, 0xCC, 0x33, 0xAA, 0x55,
			0xFF, 0x00, 0xCC, 0x33, 0xAA, 0x55,
			0xFF, 0x00, 0xCC, 0x33, 0xAA, 0x55,
	};
	LCD_SetTextColor(LCD_COLOR_WHITE);
	LCD_DrawBMP1Bit(midX+75, midY+75, 5, 48, data, 0);


	// Test DrawRect
	LCD_SetTextColor(LCD_COLOR_RED);
	LCD_DrawRect(10, 50, 150, 50);
	LCD_SetTextColor(LCD_COLOR_GREEN);
	LCD_DrawRect(60, 50, 150, 50);
	LCD_SetTextColor(LCD_COLOR_BLUE);
	LCD_DrawRect(110, 50, 150, 50);

	// Test Display Off/On
	LCD_DisplayOff();
	LCD_DisplayOn();
	LCD_BackLight(50);
	LCD_BackLight(0);
	LCD_BackLight(100);

	// Test drawing a bitmap
	LCD_DrawBMP16Bit(0,0, gimp_image.height, gimp_image.width, (uint16_t*) gimp_image.pixel_data, 0);

	#if 0
	// Test put/get pixel
	LCD_PutPixel(10, 10, 0x1234);
	assert(LCD_GetPixel(10, 10) == 0x1234);
	LCD_PutPixel(10, 10, Red);
	assert(LCD_GetPixel(10, 10) == LCD_COLOR_RED);
	LCD_PutPixel(10, 10, Green);
	assert(LCD_GetPixel(10, 10) == LCD_COLOR_GREEN);
	LCD_PutPixel(10, 10, Blue);
	assert(LCD_GetPixel(10, 10) == LCD_COLOR_BLUE);
	#endif

	volatile int a = 0;
	while (1)
		a ++;
}
