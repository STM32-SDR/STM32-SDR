/*
 * Code to handle the 2 encoder knobs
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


#include	"stm32f4xx_rcc.h"
#include	"stm32f4xx_exti.h"
#include	"stm32f4xx_syscfg.h"
#include 	"stm32f4xx_gpio.h"
#include	"arm_math.h"
#include	"SI570.h"
#include	"Init_I2C.h"
#include	"Init_Codec.h"
#include	"Encoders.h"
#include	"FrequencyManager.h"
#include	"options.h"
#include	"LcdHal.h"
#include	"ChangeOver.h"


// Data for the encoder state.
const int16_t ENC_SENS = 2;
const int8_t ENCODER_STATES[] = {
		0, 1, -1, 0,
		-1, 0, 0, 1,
		1, 0, 0, -1,
		0, -1, 1, 0 };

typedef struct
{
	uint8_t old;
	int8_t dir0;
	int8_t dir1;

	GPIO_TypeDef *pPort;
	uint8_t lowPinNumber;
} EncoderStruct_t;
static EncoderStruct_t s_encoderStruct1 = {0, 0, 0, GPIOC, 5};
static EncoderStruct_t s_encoderStruct2 = {0, 0, 0, GPIOB, 4};

// Prototypes
static void configureGPIOEncoder1(void);
static void configureGPIOEncoder2(void);
//static void configureEncoderInterrupt(void);
static void init_encoder1(void);
static void init_encoder2(void);
static int8_t calculateEncoderChange(EncoderStruct_t *pEncoder);
static void applyEncoderChange1(int8_t changeDirection);
static void applyEncoderChange2(int8_t changeDirection);


/* ----------------------
 * Initialization
 * ---------------------- */
void Encoders_Init(void)
{
	configureGPIOEncoder1();
	configureGPIOEncoder2();
	init_encoder1();
	init_encoder2();  //this may be requied to introduce a delay for start up
	FrequencyManager_StepFrequencyDown();
	FrequencyManager_StepFrequencyUp();
}

static void configureGPIOEncoder1(void)
{
	//EXTI_InitTypeDef EXTI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable GPIOC clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	/* Enable SYSCFG clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	/* Configure PC5,6&8 pin as input with Pull Up */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_8 );
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	/* Configure PE3,4,5&6 pin as input with Pull Up */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5
			| GPIO_Pin_6 );
	GPIO_Init(GPIOE, &GPIO_InitStructure);


}
static void configureGPIOEncoder2(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable GPIOC clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	/* Enable SYSCFG clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	/* Configure PC1,2,3&4 pin as input with Pull Up */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3	| GPIO_Pin_4 );
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	/* Configure PB3,4,5&7 pin as input with Pull Up */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7 );
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}



void init_encoder1(void)
{
	// TODO: Display the SI570 error in a better way.
	if (SI570_Chk == 3) {
		LCD_StringLine(234, 60, " SI570_?? ");
	}
	else {
		Compute_FXTAL();
	}
}

void init_encoder2(void)
{
	// TODO:- Is there a need to do this here?
	RxTx_SetReceive();
}



/*
 * Process Encoder Changes
 */

void Encoders_CalculateAndProcessChanges(void)
{
	// TODO:- Is delay needed for encoder? Should it be doubled (as would have been before encoder refactor)?
	Delay(100);

	int change = 0;
	change = calculateEncoderChange(&s_encoderStruct1);
	applyEncoderChange1(change);

	change = calculateEncoderChange(&s_encoderStruct2);
	applyEncoderChange2(change);
}

static int8_t calculateEncoderChange(EncoderStruct_t *pEncoder)
{
	// Encoder motion has been detected--determine direction
	Delay(100);

	// Remember previous encoder state and add current state (2 bits)
	pEncoder->old <<= 2;
	pEncoder->old |= (GPIO_ReadInputData(pEncoder->pPort) >> pEncoder->lowPinNumber & 0x03);

	int8_t tempDir = ENCODER_STATES[(pEncoder->old & 0x0F)];
	pEncoder->dir0 += tempDir;
	if ((pEncoder->dir0 < ENC_SENS) && (pEncoder->dir0 > -ENC_SENS))
		pEncoder->dir1 = 0;
	if (pEncoder->dir0 >= ENC_SENS) {
		pEncoder->dir1 = 1;
		pEncoder->dir0 = 0;
	}
	if (pEncoder->dir0 <= -ENC_SENS) {
		pEncoder->dir1 = -1;
		pEncoder->dir0 = 0;
	}

	return pEncoder->dir1;
}

static void applyEncoderChange1(int8_t changeDirection)
{
	// Check for no change
	if (changeDirection == 0) {
		return;
	}

	_Bool isEncoderPressedIn = !GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8);

	// Are we pressed in?
	if (isEncoderPressedIn) {
		if (changeDirection > 0) {
			FrequencyManager_DecreaseFreqStepSize();
		} else {
			FrequencyManager_IncreaseFreqStepSize();
		}
	}
	else {
		if (changeDirection > 0) {
			FrequencyManager_StepFrequencyUp();
		} else {
			FrequencyManager_StepFrequencyDown();
		}
	}
}
static void applyEncoderChange2(int8_t changeDirection)
{
	// Check for no change
	if (changeDirection == 0) {
		return;
	}

	/*
	 * Check the limits
	 */
	int curOptIdx = Options_GetSelectedOption();
	int16_t currentValue = Options_GetValue(curOptIdx);
	int16_t newValue = currentValue + Options_GetChangeRate(curOptIdx) * changeDirection;
	int16_t minValue = Options_GetMinimum(curOptIdx);
	int16_t maxValue = Options_GetMaximum(curOptIdx);
	if (newValue < minValue) {
		newValue = minValue;
	}
	if (newValue > maxValue) {
		newValue = maxValue;
	}

	// Set the value & Display it
	Options_SetValue(curOptIdx, newValue);
}
