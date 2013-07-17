/*
 * Encoder_2.c
 *
 *  Created on: Mar 23, 2013
 *      Author: CharleyK
 */

#include	"stm32f4xx.h"
#include	"stm32f4xx_rcc.h"
#include	"stm32f4xx_exti.h"
#include	"stm32f4xx_syscfg.h"
#include 	"stm32f4xx_gpio.h"
#include	"misc.h"
#include	"arm_math.h"
#include	"TFT_Display.h"
#include	"SI570.h"
#include	"Init_I2C.h"
#include	"Init_Codec.h"
#include	"Encoder_2.h"
#include	"eeprom.h"
#include	"arm_math.h"
#include	"Codec_Gains.h"
#include	"ChangeOver.h"
#include	"Encoder_1.h"
#include	"assert.h"
#include	"options.h"
#include	"FrequencyManager.h"

EXTI_InitTypeDef EXTI_InitStructure;

int16_t ENC_Sens2;
int8_t DIR2;


void Encoder2_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//NVIC_InitTypeDef   NVIC_InitStructure;

	/* Enable GPIOC clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	/* Enable SYSCFG clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	/* Configure PC1,2,3&4 pin as input with Pull Up */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3
			| GPIO_Pin_4 );
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	/* Configure PE3,4,5&7 pin as input with Pull Up */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7 );
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Enable SYSCFG clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	/* Connect EXTI Line 7 to PA7 pin */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource7 );

	/* Configure EXTI Line7 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line7;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	// /* Enable and set EXTI Line9-5 Interrupt to the lowest priority */
	// NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	// NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
	// NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
	// NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	// NVIC_Init(&NVIC_InitStructure);

}


void init_encoder2(void)
{
	Receive_Sequence();

	ENC_Sens2 = 2;
	check_SS2();
}

void process_encoder2(void)
{
	// encoder motion has been detected--determine direction
	static uint8_t old = { 0 };
	static int8_t enc_states[] = { 0, 1, -1, 0, -1, 0, 0, 1, 1, 0, 0, -1, 0, -1, 1, 0 };
	int8_t tempDIR;
	static int8_t DIR0;

	Delay(100);
	old <<= 2;  //remember previous encoder state
	old |= (GPIO_ReadInputData(GPIOB ) >> 4 & 0x03);  //get the current state
	tempDIR = enc_states[(old & 0x0F)];
	DIR0 += tempDIR;
	if ((DIR0 < ENC_Sens2) && (DIR0 > -ENC_Sens2))
		DIR2 = 0;
	if (DIR0 >= ENC_Sens2) {
		DIR2 = 1;
		DIR0 = 0;
	}
	if (DIR0 <= -ENC_Sens2) {
		DIR2 = -1;
		DIR0 = 0;
	}

	// Do we have a change?
	if (DIR2 != 0) {
		check_SS2();

		/*
		 * Check the limits
		 */
		int curOptIdx = Options_GetSelectedOption();
		int16_t currentValue = Options_GetValue(curOptIdx);
		int16_t newValue = currentValue + Options_GetChangeRate(curOptIdx) * DIR2;
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
}

void check_SS2(void)
{
	//Read PC1,2,3,4 as four bit word
	int16_t read_SS2 = ~GPIO_ReadInputData(GPIOC ) >> 1 & 0x000F;
	Options_SetSelectedOption(read_SS2);
}

void Store_Defaults(void)
{
//	LCD_StringLine(234, 40, " Default  ");
	FrequencyManager_ResetBandsToDefault();
	FrequencyManager_WriteBandsToEeprom();
	process_SS1();

//	LCD_StringLine(0, 40, " Default  ");
	Options_ResetToDefaults();
	Options_WriteToEEPROM();
	check_SS2();
}

