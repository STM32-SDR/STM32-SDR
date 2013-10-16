/*
 * Code that manipulates the various options
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


#include "options.h"
#include "Codec_Gains.h"
#include <assert.h>
#include "ModeSelect.h"
#include "DMA_IRQ_Handler.h"
#include "eeprom.h"
#include "Si570.h"
#include "FrequencyManager.h"
#include "ChangeOver.h"
#include "AGC_Processing.h"
#include "DSP_Processing.h"

#define EEPROM_OFFSET 200

#define EEPROM_SENTINEL_LOC 50
//#define EEPROM_SENTINEL_VAL 5680
//#define EEPROM_SENTINEL_VAL 3333
//#define EEPROM_SENTINEL_VAL 2222
#define EEPROM_SENTINEL_VAL 1037

static OptionNumber s_currentOptionNumber = OPTION_RX_AUDIO;

typedef struct
{
	const char* Name;
	const int16_t Initial;
	const int16_t Minimum;
	const int16_t Maximum;
	const int16_t ChangeUnits;
	int16_t CurrentValue;
} OptionStruct;

// Order must match OptionNumber in options.h
static OptionStruct s_optionsData[] = {
	{
		/*Name*/ "Rx Audio ",
		/*Init*/ 0,
		/*Min */ DAC_GAIN_MIN,
		/*Max */ DAC_GAIN_MAX,
		/*Rate*/ 1,
		/*Data*/ 0
	},
	{
		/*Name*/ "  Rx RF  ",
		/*Init*/ 80,
		/*Min */ 0,
		/*Max */ 80,
		/*Rate*/ 1,
		/*Data*/ 0,
	},
	{
		/*Name*/ "Mic Gain ",
		/*Init*/ 20,
		/*Min */ 0,
		/*Max */ 80,
		/*Rate*/ 1,
		/*Data*/ 0,
	},
	{
		/*Name*/ "Tx Level ",
		/*Init*/ 29,
		/*Min */ 0,
		/*Max */ 29,
		/*Rate*/ 1,
		/*Data*/ 0,
	},

	{
		/*Name*/ "ST Level ",
		/*Init*/ 0,
		/*Min */ 0,
		/*Max */ 29,
		/*Rate*/ 1,
		/*Data*/ 0,
	},
	{
		/*Name*/ "  Rx Amp ",
		/*Init*/ 5000,
		/*Min */ 1000,
		/*Max */ 10000,
		/*Rate*/ 10,
		/*Data*/ 0,
	},
	{
		/*Name*/ "Rx Phase ",
		/*Init*/ 0,
		/*Min */ -10000,
		/*Max */ 10000,
		/*Rate*/ 10,
		/*Data*/ 0,
	},
	{
		/*Name*/ "  Tx Amp ",
		/*Init*/ 5000,
		/*Min */ 1000,
		/*Max */ 10000,
		/*Rate*/ 10,
		/*Data*/ 0,
	},
	{
		/*Name*/ "Tx Phase ",
		/*Init*/ 0,
		/*Min */ -10000,
		/*Max */ 10000,
		/*Rate*/ 10,
		/*Data*/ 0,
	},
	{
		/*Name*/ "Mic Bias ",
		/*Init*/ 0,
		/*Min */ 0,
		/*Max */ 1,
		/*Rate*/ 1,
		/*Data*/ 0,
	},
	{
		/*Name*/ "AGC Thr 1",
		/*Init*/ 400,
		/*Min */ 50,
		/*Max */ 1000,
		/*Rate*/ 10,
		/*Data*/ 0,
	},

	{
		/*Name*/ "AGC Thr 2",
		/*Init*/ 100,
		/*Min */ 50,
		/*Max */ 1000,
		/*Rate*/ 10,
		/*Data*/ 0,
	},

	{
		/*Name*/ "AGC Mode ",
		/*Init*/ 0,
		/*Min */ 0,
		/*Max */ 3,
		/*Rate*/ 1,
		/*Data*/ 0,
	},

	{
		/*Name*/ "SI570Mult",
		/*Init*/ 4,
		/*Min */ 2,
		/*Max */ 4,
		/*Rate*/ 2,
		/*Data*/ 0,
	},
};

// Initialization
void Options_Initialize(void)
{
	// TODO: Clear out this initialization from options to the correct places.
	rgain = 0.5;					//temp location, move to sequencer

	 //Load from EEPROM if valid:
	if (Options_HaveValidEEPROMData()) {
		Options_ReadFromEEPROM();
	}
	else {
		Options_ResetToDefaults();
		Options_WriteToEEPROM();
	}

	// Initialize the multiplier to the radio:
	assert(s_optionsData[OPTION_SI570_MULT].CurrentValue == 2 ||
			s_optionsData[OPTION_SI570_MULT].CurrentValue == 4);
	FrequencyManager_SetFreqMultiplier(s_optionsData[OPTION_SI570_MULT].CurrentValue);

}
void Options_ResetToDefaults(void)
{
	for (int i = 0; i < NUM_OPTIONS; i++) {
		Options_SetValue(i, s_optionsData[i].Initial);
	}
}

// Work with option data
const char* Options_GetName(int optionIdx)
{
	assert(optionIdx >= 0 && optionIdx < NUM_OPTIONS);
	return s_optionsData[optionIdx].Name;
}
int16_t Options_GetValue(int optionIdx)
{
	assert(optionIdx >= 0 && optionIdx < NUM_OPTIONS);
	return s_optionsData[optionIdx].CurrentValue;
}
void Options_SetValue(int optionIdx, int16_t newValue)
{
	assert(optionIdx >= 0 && optionIdx < NUM_OPTIONS);
	s_optionsData[optionIdx].CurrentValue = newValue;

	/*
	 * Process each option individually as needed.
	 */
	switch (optionIdx) {
	case OPTION_RX_AUDIO:
		if (RxTx_InRxMode())
			dac_gain = newValue;
			Old_dac_gain = -200;  //Set to an outrageous value for change testing in AGC_Processsing.c
		break;

	case OPTION_RX_RF:
		if (RxTx_InRxMode())
			PGAGAIN0 = newValue;
			Old_PGAGAIN0 = -200; //Set to an outrageous value for change testing in AGC_Processsing.c
			break;

	case OPTION_Mic_Gain:
		if (RxTx_InTxMode() && (Mode_GetCurrentMode() == MODE_SSB))
			Set_PGA_Gain(newValue);
		break;

	case OPTION_Tx_LEVEL:
		if (RxTx_InTxMode())
			Set_LO_Gain(newValue);
		break;

	case OPTION_ST_LEVEL:  //Side Tone Level
		if (RxTx_InTxMode())
			Set_HP_Gain(newValue);
		break;

	case OPTION_RX_AMP:
		R_lgain = ((float) newValue) / 10000.0;
		break;

	case OPTION_RX_PHASE:
		R_xgain = ((float) newValue) / 10000.0;
		break;

	case OPTION_TX_AMP:
		T_lgain = ((float) newValue) / 10000.0;
		break;

	case OPTION_TX_PHASE:
		T_xgain = ((float) newValue) / 10000.0;
		break;

	case OPTION_MIC_BIAS:
		if (newValue == 0) {
			Turn_Off_Bias();
		} else {
			Turn_On_Bias();
		}
		break;

	case OPTION_AGC_THRSH1:
		if (RxTx_InRxMode())
			Init_AGC();
		break;

	case OPTION_AGC_THRSH2:
		if (RxTx_InRxMode())
			Init_AGC();
		break;

	case OPTION_AGC_Mode:
		AGC_Mode = newValue;
		Init_AGC();
		break;

	case OPTION_SI570_MULT:
		FrequencyManager_SetFreqMultiplier(newValue);
		break;

	default:
		// Handle any programming errors where we forget to update this switch statement.
		assert(0);
	}

}
uint16_t Options_GetMinimum(int optionIdx)
{
	assert(optionIdx >= 0 && optionIdx < NUM_OPTIONS);
	return s_optionsData[optionIdx].Minimum;
}
uint16_t Options_GetMaximum(int optionIdx)
{
	assert(optionIdx >= 0 && optionIdx < NUM_OPTIONS);
	return s_optionsData[optionIdx].Maximum;
}
uint16_t Options_GetChangeRate(int optionIdx)
{
	assert(optionIdx >= 0 && optionIdx < NUM_OPTIONS);
	return s_optionsData[optionIdx].ChangeUnits;
}

// Option selection
OptionNumber Options_GetSelectedOption(void)
{
	return s_currentOptionNumber;
}
void Options_SetSelectedOption(OptionNumber newOption)
{
//	assert(newOption >= 0 && newOption < NUM_OPTIONS);
	if (newOption >= 0 && newOption < NUM_OPTIONS) {
		s_currentOptionNumber = newOption;
	}
}


// EEPROM Access
void Options_WriteToEEPROM(void)
{
	for (int i = 0; i < NUM_OPTIONS; i++) {
		Write_Int_EEProm(EEPROM_OFFSET + i * 2, Options_GetValue(i));
	}

	// Add Sentinel
	Write_Int_EEProm(EEPROM_SENTINEL_LOC, EEPROM_SENTINEL_VAL);
}
_Bool Options_HaveValidEEPROMData(void)
{
	//Read the 50 address to see if IQ data has been stored
	int16_t sentinel = Read_Int_EEProm(EEPROM_SENTINEL_LOC);

	return (sentinel == EEPROM_SENTINEL_VAL);
}
void Options_ReadFromEEPROM(void)
{
	for (int i = 0; i < NUM_OPTIONS; i++) {
		int16_t newValue = Read_Int_EEProm(EEPROM_OFFSET + i * 2);
		Options_SetValue(i, newValue);
	}
}
