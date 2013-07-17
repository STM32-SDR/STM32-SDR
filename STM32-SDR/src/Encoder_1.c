/*
 * Encoder_1.c
 *
 *  Created on: Feb 25, 2013
 *      Author: CharleyK
 */


#include	"stm32f4xx_rcc.h"
#include	"stm32f4xx_exti.h"
#include	"stm32f4xx_syscfg.h"
#include 	"stm32f4xx_gpio.h"
#include	"arm_math.h"
#include	"TFT_Display.h"
#include	"SI570.h"
#include	"Init_I2C.h"
#include	"Init_Codec.h"
#include	"Encoder_1.h"
#include	"FrequencyManager.h"

EXTI_InitTypeDef EXTI_InitStructure;


const int16_t ENC_Sens1 = 2;

void Encoder1_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

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

	/* Enable SYSCFG clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	/* Connect EXTI Line8 to PA8 pin */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource8 );

	/* Configure EXTI Line8 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line8;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Enable and set EXTI Line9-5 Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void init_encoder1(void)
{
	// TODO: Display the SI570 error in a better way.
	if (SI570_Chk == 3) {
		LCD_StringLine(234, 60, " SI570_?? ");
	}

	if (SI570_Chk == 3) {
	}
	else {
		Compute_FXTAL();
		process_SS1();
	}
}

void process_encoder1(void)
{
	// encoder motion has been detected--determine direction
	static uint8_t old = { 0 };
	static int8_t enc_states[] = { 0, 1, -1, 0, -1, 0, 0, 1, 1, 0, 0, -1, 0, -1, 1, 0 };
	int8_t tempDIR;
	static int8_t DIR0;
	static int8_t DIR1;

	Delay(100);
	old <<= 2;  //remember previous encoder state
	old |= (GPIO_ReadInputData(GPIOC ) >> 5 & 0x03);  //get the current state
	tempDIR = enc_states[(old & 0x0F)];
	DIR0 += tempDIR;
	if ((DIR0 < ENC_Sens1) && (DIR0 > -ENC_Sens1))
		DIR1 = 0;
	if (DIR0 >= ENC_Sens1) {
		DIR1 = 1;
		DIR0 = 0;
	}
	if (DIR0 <= -ENC_Sens1) {
		DIR1 = -1;
		DIR0 = 0;
	}

	if (DIR1 != 0) {
		if (DIR1 > 0) {
			FrequencyManager_StepFrequencyUp();
		} else {
			FrequencyManager_StepFrequencyDown();
		}
	}
}


int16_t Encoder1_GetPosition(void)
{
	return ~GPIO_ReadInputData(GPIOE ) >> 3 & 0x000F;
}

void process_SS1(void)
{
	FrequencyManager_SetSelectedBand(Encoder1_GetPosition());
}
