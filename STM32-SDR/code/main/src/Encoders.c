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
#include	"stm32f4xx_tim.h"
#include	"arm_math.h"
#include	"SI570.h"
#include	"Init_I2C.h"
#include	"Init_Codec.h"
#include	"Encoders.h"
#include	"FrequencyManager.h"
#include	"options.h"
#include	"LcdHal.h"
#include	"ChangeOver.h"
#include	"xprintf.h"
#include	"screen_All.h"

extern int 	NCOTUNE;

typedef struct
{
	uint16_t old;
	int8_t direction;
	GPIO_TypeDef *pPort;
	uint8_t lowPinNumber;
} EncoderStruct_t;
static EncoderStruct_t s_encoderStruct1 = {0, 0, GPIOC, 5};
static EncoderStruct_t s_encoderStruct2 = {0, 0, GPIOB, 4};

// Encoder used for options =0 or filter code selection = 1

// Prototypes
static void initEncoderTimer(void);
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
	init_encoder2();  //this may be required to introduce a delay for start up
	FrequencyManager_StepFrequencyDown();
	FrequencyManager_StepFrequencyUp();
	initEncoderTimer();
}

static void initEncoderTimer(void)
{
	debug(GUI, "initEncoderTimer:\n");

	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable the TIM8 global Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* TIM7 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);

	/* Time base configuration */
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = 256;
	TIM_TimeBaseStructure.TIM_Prescaler = 256;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure);

	/* TIM IT enable */
	TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);

	/* TIM4 enable counter */
	TIM_Cmd(TIM7, ENABLE);
}


void TIM7_IRQHandler(void)
{
	static int8_t change;
	static int count;
	static int old_count;
	static int loop;

	if (TIM_GetITStatus(TIM7, TIM_IT_Update ) != RESET){
		count++;
		change=calculateEncoderChange(&s_encoderStruct1);
		if (change != 0){
			loop++;
//			xprintf ("%d, %d\n", loop, (count - old_count));
			if ((count - old_count) < 20){
				s_encoderStruct1.direction += change;
				loop=0;
			} else if (loop >= 4){
				s_encoderStruct1.direction += change;
				loop=0;
			}
			old_count=count;
		}

		change=calculateEncoderChange(&s_encoderStruct2);
		applyEncoderChange2(change);
		TIM_ClearITPendingBit(TIM7, TIM_IT_Update );
	}
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


// Return if one (or both) encoders pressed.
_Bool Encoders_IsOptionsEncoderPressed(void)
{
	return !GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7);
}
_Bool Encoders_IsFrequencyEncoderPressed(void)
{
	return !GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8);
}
_Bool Encoders_AreBothEncodersPressed(void)
{
	return Encoders_IsOptionsEncoderPressed()
			&& Encoders_IsFrequencyEncoderPressed();
}

/*
 * Process Encoder Changes
 */
void Encoders_CalculateAndProcessChanges(void){
	if (s_encoderStruct1.direction > 0){
		applyEncoderChange1(1);
		s_encoderStruct1.direction--;
	}
	if (s_encoderStruct1.direction < 0){
		applyEncoderChange1(-1);
		s_encoderStruct1.direction++;
	}
	if (s_encoderStruct2.direction > 0){
		applyEncoderChange2(1);
		s_encoderStruct2.direction--;
	}
	if (s_encoderStruct2.direction < 0){
		applyEncoderChange2(-1);
		s_encoderStruct2.direction++;
	}
}

static int8_t calculateEncoderChange(EncoderStruct_t *pEncoder)
{
	// Encoder motion has been detected--determine direction
	int8_t direction=0;

	uint16_t code = (GPIO_ReadInputData(pEncoder->pPort) >> pEncoder->lowPinNumber & 0x03);

	// encoder sequence is 0, 1, 3, 2 clockwise
	// or 2, 3, 1, 0 anti-clockwise
	// uses a 8 state machine to detect rotation to eliminate noise
	// requires 3 sequences before determining direction

	//new encoder bits are prepended to the MSB's
	// pEncoder->old bits 0/1 and 2/3 are old values and bits 4/5 are new values
	code = 0x100 * code + pEncoder->old;

	// when a change is detected shift right 4 bits and store the sequence
	// for use next cycle
	if (((code & 0xF00 )>> 8) != ((code & 0x0F0 )>>4))
		pEncoder->old = code / 0x10; //save the new encoder bits

	switch (code){
	case 0x310:
	case 0x231:
	case 0x023:
	case 0x102:
		// clockwise
		direction = +1;
		break;
	case 0x132:
	case 0x013:
	case 0x201:
	case 0x320:
		// anti-clockwise
		direction = -1;
		break;

		//may be changing direction from clockwise to anti-clockwise
	case 0x202:
		pEncoder->old = 0x20;
		break;
	case 0x010:
		pEncoder->old = 0x01;
		break;
	case 0x131:
		pEncoder->old = 0x13;
		break;
	case 0x323:
		pEncoder->old = 0x32;
		break;

		//may be changing direction from anti-clockwise to clockwise
	case 0x020:
		pEncoder->old = 0x02;
		break;
	case 0x232:
		pEncoder->old = 0x23;
		break;
	case 0x313:
		pEncoder->old = 0x31;
		break;
	case 0x101:
		pEncoder->old = 0x10;
		break;

		// ignore any other sequences
	default:
		break;
	}

	return direction;
}

static void applyEncoderChange1(int8_t changeDirection)
{
	// Check for no change
	if (changeDirection == 0) {
		return;
	}

	// Are we pressed in?
	if (Encoders_IsFrequencyEncoderPressed()) {
		if (changeDirection > 0) {
			FrequencyManager_DecreaseFreqStepSize();
		} else {
			FrequencyManager_IncreaseFreqStepSize();
		}
	}
	else {

		if (NCOTUNE){
			if (changeDirection > 0) {
				Tune_NCO_Up( );
			} else {
				Tune_NCO_Down();
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

	// Filter screen requires an update band frequency
	if (Screen_GetScreenMode() == FILTER){
		int band = Screen_GetFilterFreqID();
		uint32_t currentFrequency = FrequencyManager_GetCurrentFrequency();
		FrequencyManager_SetBandFreqFilter (band, currentFrequency);
	}

}
static void applyEncoderChange2(int8_t changeDirection)
{
	int curOptIdx;
	int16_t currentValue;
	int16_t newValue;
	int16_t minValue;
	int16_t maxValue;

	// Check for no change
	if (changeDirection == 0) {
		return;
	}
	// Are we setting general options or the band filter code values
	// Options uses an array of min/max values
	// Filter sets a fixed 3-bit binary code
	if (Screen_GetScreenMode() != FILTER){
		/*
		 * Check the limits
		 */
		curOptIdx = Options_GetSelectedOption();
		currentValue = Options_GetValue(curOptIdx);
		newValue = currentValue + Options_GetChangeRate(curOptIdx) * changeDirection;
		minValue = Options_GetMinimum(curOptIdx);
		maxValue = Options_GetMaximum(curOptIdx);
		if (newValue < minValue) {
			newValue = minValue;
		}
		if (newValue > maxValue) {
			newValue = maxValue;
		}
		// Set the value & Display it

		Options_SetValue(curOptIdx, newValue);
	} else {
		int band = Screen_GetFilterCodeID();
		currentValue = FrequencyManager_GetFilterCode (band);
		int changeRate = BAND_FILTER_CHANGE_RATE;
		newValue = currentValue + changeRate * changeDirection;
		minValue = BAND_FILTER_MINIMUM;
		maxValue = BAND_FILTER_MAXIMUM;
		if (newValue < minValue) {
			newValue = minValue;
		}
		if (newValue > maxValue) {
			newValue = maxValue;
		}

		// Set the value & Display it
		FrequencyManager_SetBandCodeFilter (band, newValue);
	}
}


