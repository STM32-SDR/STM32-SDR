/*
 * Code that handles switching the various mode from TX to RX
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

#include "ChangeOver.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "Codec_Gains.h"
#include "Init_I2C.h"  //Referenced for Delay(n);
#include "PSKMod.h"
#include "CW_Mod.h"
#include "DMA_IRQ_Handler.h"
#include "options.h"
#include "ScrollingTextBox.h"
#include "AGC_Processing.h"
#include "ModeSelect.h"
#include "Init_Codec.h"
#include <assert.h>
#include <xprintf.h>


// Constants
//const int DEBOUNCE_COUNT_REQUIRED = 3;	// Called from IRQ
const int DEBOUNCE_COUNT_REQUIRED = 30;	// Called from main()

// Private state variables:
static _Bool s_isPttPressed = 0;
static _Bool s_inTxMode = 0;
static _Bool s_inTransition = 0;

// Private functions:
static void Receive_Sequence(void);
static void Xmit_SSB_Sequence(void);
static void Xmit_CW_Sequence(void);
static void Xmit_PSK_Sequence(void);
static void Init_PTT_IO(void);
void handlePttStateChange(void);

/****************************************
 * Public interface
 ****************************************/
// Initialize radio at startup.
void RxTx_Init(void)
{
	Init_PTT_IO();
}

// Control switching between receive and transmit mode.
void RxTx_SetReceive(void)
{
	Receive_Sequence();
}
void RxTx_SetTransmit(void)
{
	switch(Mode_GetCurrentMode()) {
	case MODE_SSB:
		Xmit_SSB_Sequence();
		break;
	case MODE_CW:
		Xmit_CW_Sequence();
		break;
	case MODE_PSK:
		Xmit_PSK_Sequence();
		break;
	default:
		assert(0);
	}
}

// Query current mode (transmit or receive).
_Bool RxTx_InRxMode(void)
{
	return !s_inTxMode;
}
_Bool RxTx_InTxMode(void)
{
	return s_inTxMode;
}


// Handle Push To Talk
void RxTx_CheckAndHandlePTT(void)
{
	// Debounce PTT signal changes
	static int debounceCount = 0;

	/* Note on PTT pins:
	 * Connector "Key" has:
	 *   Pin 1: PTT_ --> Port D, Pin 2
	 *   Pin 5: Key1 --> Port C, Pin 9
	 *
	 * Pin C9 is the "normal" PTT pin, D2 allows room for future expansion
	 * of the PTT approach and support unique input pins for dot vs dash generation.
	 */
	_Bool isKeyDown = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_9) == 0;

	// Are we trying to change?
	if (isKeyDown != s_isPttPressed) {
		// Debounce:
		if (debounceCount < DEBOUNCE_COUNT_REQUIRED) {
			// Count up:
			debounceCount++;
//			xprintf("#%d\n", debounceCount);

			// Debounce done?
			if (debounceCount == DEBOUNCE_COUNT_REQUIRED) {
				s_isPttPressed = isKeyDown;
				debounceCount = 0;
				//xprintf("PTT Changed to %d\n", s_isPttPressed);
				handlePttStateChange();
			}
		}
	} else {
		// Not currently trying to change:
		debounceCount = 0;
	}


	// Special case to handle CW:
	if (Mode_GetCurrentMode() == MODE_CW) {
		if (CW_DesiresTransmitMode() && !RxTx_InTxMode()) {
			Connect_Sidetone_Input();  //  Route the CW Sidetone to Headphones
			RxTx_SetTransmit();
			xprintf("To CW Tx\n");
		}
		if (!CW_DesiresTransmitMode() && RxTx_InTxMode()) {
			s_inTransition = 1;
			Disconnect_Sidetone_Input();
			Delay(900000);
			RxTx_SetReceive();
			xprintf("To CW Rx\n");
		}
	}
}

_Bool RxTx_IsPttPressed(void)
{
	return s_isPttPressed;
}
_Bool RxTx_InTransion(void)
{
	return s_inTransition;
}

/****************************************
 * Private Functions
 ****************************************/
void Receive_Sequence(void)
{
	//Codec_Init();

	Mute_HP();
	Mute_LO();
	s_inTxMode = 0;
	if (AGC_Mode != 3)
	{AGC_On =1;
	Init_AGC();
	}
	else
	{
	AGC_On =0;  //This forces the manual AGC mode
	Init_AGC();  //added JDG
	}
	GPIO_WriteBit(GPIOD, GPIO_Pin_3, Bit_SET);	//Make PTT_Out High, Remember FET Inversion
	Delay(1000);
	//Disconnect_Sidetone_Input();
	//Delay(900000);
	Disconnect_PGA();

	Connect_IQ_Inputs();
	Set_DAC_DVC(Options_GetValue(OPTION_RX_AUDIO));
	Set_ADC_DVC(-10);  //was -20 using Milt's AGC scheme
	Set_HP_Gain(6);
	s_inTransition = 0;
	//Init_Waterfall();
}

void Xmit_SSB_Sequence(void)
{
	Mute_HP();
	Mute_LO();
	s_inTxMode = 1;
	AGC_On = 0;  //Turn off AGC so that DAC is held constant during transmit
	Init_AGC();  //added JDG
	Disconnect_PGA();
	Set_DAC_DVC(15); //SSB Xmit DAC Gain
	Set_ADC_DVC(0);
	Connect_Microphone_Input();
	Set_PGA_Gain(Options_GetValue(OPTION_Mic_Gain));
	GPIO_WriteBit(GPIOD, GPIO_Pin_3, Bit_RESET);  //Make PTT_Out Low,Remember FET Inversion
	Delay(1000);
	//Set_LO_Gain(24);
	Set_LO_Gain(Options_GetValue(OPTION_Tx_LEVEL));
}

void Xmit_CW_Sequence(void)
{
	//Mute_HP();
	Mute_LO();
	s_inTxMode = 1;
	AGC_On = 0;
	Init_AGC();  //added JDG
	Set_DAC_DVC(-33); //CW Xmit DAC Gain
	GPIO_WriteBit(GPIOD, GPIO_Pin_3, Bit_RESET);  //Make PTT_Out Low,Remember FET Inversion
	Delay(1000);
	//Set_LO_Gain(20);
	Set_LO_Gain(Options_GetValue(OPTION_Tx_LEVEL));

}

void Xmit_PSK_Sequence(void)
{
	Mute_HP();
	Mute_LO();
	s_inTxMode = 1;
	AGC_On = 0;  //Turn off AGC so that DAC is held constant during transmit
	Init_AGC();  //added JDG
	Disconnect_PGA();
	Set_DAC_DVC(12); //PSK Xmit DAC Gain
	GPIO_WriteBit(GPIOD, GPIO_Pin_3, Bit_RESET);  //Make PTT_Out Low,Remember FET Inversion
	Delay(1000);
	//Set_LO_Gain(20);
	Set_LO_Gain(Options_GetValue(OPTION_Tx_LEVEL));
	Set_HP_Gain(Options_GetValue(OPTION_ST_LEVEL));
}

void Init_PTT_IO(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2; // Pin 2 is PTT_In
	GPIO_Init(GPIOD, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3; //  Pin 3 controls the FET-Gate for PTT_Out
	GPIO_Init(GPIOD, &GPIO_InitStruct);

}

// Called when Push To Talk state has changed (after debounce)
void handlePttStateChange(void)
{
	switch (Mode_GetCurrentMode()) {
	case MODE_SSB:
		if (s_isPttPressed) {
			RxTx_SetTransmit();
		} else {
			RxTx_SetReceive();
		}
		break;
	case MODE_CW:
		break;
	case MODE_PSK:
		break;
	default:
		assert(0);
	}
}
