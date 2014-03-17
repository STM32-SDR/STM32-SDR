/*
 * Header file for Options screen code
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

#include <stdint.h>

// This sequence must be the same as options.c
typedef enum {
	OPTION_RX_AUDIO = 0,
	OPTION_RX_RF,
	OPTION_AGC_Mode,
	OPTION_Mic_Gain,
	OPTION_Tx_LEVEL,
	OPTION_ST_LEVEL,
	OPTION_RX_AMP,
	OPTION_RX_PHASE,
	OPTION_TX_AMP,
	OPTION_TX_PHASE,
	OPTION_MIC_BIAS,
	OPTION_AGC_THRSH,
	OPTION__RSL_CAL,
	OPTION_SI570_MULT,
	NUM_OPTIONS
} OptionNumber;

// Define the ranges for basic, advanced and filter options
#define START_BASIC_OPTIONS OPTION_RX_AUDIO
#define END_BASIC_OPTIONS OPTION_RX_AMP

#define START_ADVANCED_OPTIONS END_BASIC_OPTIONS
#define END_ADVANCED_OPTIONS NUM_OPTIONS

#define START_BAND_OPTIONS OPTION_BAND0
#define END_BAND_OPTIONS NUM_FILTER_OPTIONS

// Options selector limits for band external filters
#define BAND_FILTER_MINIMUM 0
#define BAND_FILTER_MAXIMUM 7
#define BAND_FILTER_CHANGE_RATE 1

// Initialization
void Options_Initialize(void);
void Options_ResetToDefaults(void);

// Work with option data
const char* Options_GetName(int optionIdx);
int16_t Options_GetValue(int optionIdx);
void     Options_SetValue(int optionIdx, int16_t newValue);
uint16_t Options_GetMinimum(int optionIdx);
uint16_t Options_GetMaximum(int optionIdx);
uint16_t Options_GetChangeRate(int optionIdx);

// Option selection
OptionNumber Options_GetSelectedOption(void);
void         Options_SetSelectedOption(OptionNumber newOption);

// EEPROM Access
void Options_WriteToEEPROM(void);
_Bool Options_HaveValidEEPROMData(void);
void Options_ReadFromEEPROM(void);
