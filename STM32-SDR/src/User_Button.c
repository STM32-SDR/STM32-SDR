/*
 User_Button.c/
 *
 *  Created on: Sep 9, 2012
 *
 *      Author: John
 */
//===============================================================================
#include	"stm32f4xx.h"
#include	"stm32f4xx_rcc.h"
#include	"stm32f4xx_exti.h"
#include	"stm32f4xx_syscfg.h"
#include 	"stm32f4xx_gpio.h"
#include	"misc.h"
#include	"User_Button.h"
#include	"PSKDet.h"
#include	"Init_Codec.h"  // for access to Delay( )
#include	"touch_pad.h"
#include 	"uart.h"
#include 	"TFT_Display.h"
#include 	"uart.h"
#include 	"xprintf.h"
#include	"arm_math.h"

EXTI_InitTypeDef EXTI_InitStructure;
uint8_t BT_Flag;

uint8_t PB_State;

//===============================================================================

void User_Button_Config(void)
{
	PB_State = 0;
	GPIO_InitTypeDef GPIO_InitStructure;
	//NVIC_InitTypeDef   NVIC_InitStructure;

	/* Enable GPIOA clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	/* Enable SYSCFG clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	/* Configure PA0 pin as input floating */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Connect EXTI Line0 to PA0 pin */
	//SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);
	/* Configure EXTI Line0 */
	// EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	// EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	// EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	// EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	// EXTI_Init(&EXTI_InitStructure);
	/* Enable and set EXTI Line0 Interrupt to the lowest priority */
	// NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	// NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
	// NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
	// NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	// NVIC_Init(&NVIC_InitStructure);
}

void BT_Flag_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable GPIOB clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	/* Enable SYSCFG clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	/* Configure PB10 & 11 pin as input floating */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void Check_BT_Flag(void)
{
	BT_Flag = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10);
}

//===============================================================================

void EXTI0_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line0 ) != RESET) {

		if (PB_State == 0)
			PB_State++;
		else
			PB_State--;

		//if (PB_State ==1)
		//LCD_StringLine(150,0,"PB_State On");
		//else
		//LCD_StringLine(150,0,"PB_State Off");

		EXTI_ClearITPendingBit(EXTI_Line0 );

	}

}

//===============================================================================

