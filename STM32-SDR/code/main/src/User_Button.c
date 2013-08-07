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
#include 	"uart.h"
#include 	"xprintf.h"
#include	"arm_math.h"

#include	"PSKMod.h"
#include	"LcdHal.h"

#include "FrequencyManager.h"
#include "TSDriver_ADS7843.h"
#include "options.h"


EXTI_InitTypeDef EXTI_InitStructure;

//===============================================================================

void User_Button_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable GPIOA clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	/* Enable SYSCFG clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	/* Configure PA0 pin as input floating */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
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


_Bool IsBTConnected(void)
{
	return GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10);
}

//===============================================================================

void EXTI0_IRQHandler(void)
{
	static uint8_t PB_State = 0;
	if (EXTI_GetITStatus(EXTI_Line0 ) != RESET) {

		if (PB_State == 0)
			PB_State++;
		else
			PB_State--;

		// Do something with the PB_State?

		EXTI_ClearITPendingBit(EXTI_Line0 );
	}
}

//===============================================================================


/*
 * Delayed Event Handling
 */
static uint32_t s_delayEventsTriggered = 0;

// Called from main.
// TODO: Remove delayed processing here once all functionality present on UI's.
void DelayEvent_ProcessDelayedEvents(void)
{
	// Handle the marked event.
	if (s_delayEventsTriggered & DelayEvent_DisplayStoreIQ) {
		LCD_StringLine(0, 40, "Store   IQ");
	}
	if (s_delayEventsTriggered & DelayEvent_DisplayStoreFreq) {
		LCD_StringLine(234, 40, "Store Freq");
	}

	// Clear all the marked events.
	s_delayEventsTriggered = 0;
}

// Called from ISR.
void DelayEvent_TriggerEvent(DelayEvent_Event event)
{
	s_delayEventsTriggered |= event;
}






/* ****************************************************************
 *   ISR
 * ****************************************************************/
static void DelayUS(vu32 cnt)
{
	uint32_t i;
	for (i = 0; i < cnt; i++) {
		uint8_t us = 12;
		while (us--) {
			;
		}
	}
}

void EXTI9_5_IRQHandler(void)
{
	//Handle Touch Screen Interrupts
	if (EXTI_GetITStatus(EXTI_Line6) != RESET) {
		TSDriver_HandleTouchInterrupt();
		EXTI_ClearITPendingBit(EXTI_Line6 );
	}

	//Handle Encoder #2 PB interrupt
	if (EXTI_GetITStatus(EXTI_Line7) != RESET) {
		// LCD code is not thread safe, so trigger a delayed event to display the "Store IQ" message.
		DelayEvent_TriggerEvent(DelayEvent_DisplayStoreIQ);

		DelayUS(10);
		Options_WriteToEEPROM();
		DelayUS(10);
		EXTI_ClearITPendingBit(EXTI_Line7 );
	}

	//Handle Encoder #1 PB interrupt
	if (EXTI_GetITStatus(EXTI_Line8) != RESET) {
		// LCD code is not thread safe, so trigger a delayed event to display the "Store Freq" message.
		DelayEvent_TriggerEvent(DelayEvent_DisplayStoreFreq);

		DelayUS(10);
		FrequencyManager_WriteBandsToEeprom();
		DelayUS(10);
		EXTI_ClearITPendingBit(EXTI_Line8 );
	}
}
