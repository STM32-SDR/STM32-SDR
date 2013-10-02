/*
 * Header file for frequency manager which controls the frequency, its value and how it changes
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


#ifndef _FREQUENCYMANAGER_H_
#define _FREQUENCYMANAGER_H_

#include <stdint.h>

typedef enum {
	FREQBAND_20M_PSK = 0,
	FREQBAND_40M_PSKu,
	FREQBAND_10MHZ_WWV,
	FREQBAND_15MHZ_WWV,
	FREQBAND_40M_QRP,
	FREQBAND_20M_QRP,
	FREQBAND_10M,
	FREQBAND_15M,
	FREQBAND_4M,
	FREQBAND_M1,
	FREQBAND_M2,
	FREQBAND_M3,
	FREQBAND_M4,
	FREQBAND_M5,
	FREQBAND_SI570_F0,

	// Count of number of bands
	// Automatically updated when more bands added above.
	FREQBAND_NUMBER_OF_BANDS
} BandPreset;


void FrequencyManager_Initialize(void);
void FrequencyManager_ResetBandsToDefault(void);

// Manage current selection
void FrequencyManager_SetSelectedBand(BandPreset newBand);
BandPreset FrequencyManager_GetSelectedBand(void);

// Work with preset bands:
const char* FrequencyManager_GetBandName(BandPreset band);
uint32_t FrequencyManager_GetBandValue(BandPreset band);

// Get/Set curent frequency to radio (in Hz)
void FrequencyManager_SetCurrentFrequency(uint32_t newFrequency);
uint32_t FrequencyManager_GetCurrentFrequency(void);
void FrequencyManager_SetFreqMultiplier(int16_t newFreqMult);

// Stepping the frequency (for example, using the adjustment knob)
void FrequencyManager_StepFrequencyUp(void);
void FrequencyManager_StepFrequencyDown(void);
void FrequencyManager_IncreaseFreqStepSize(void);
void FrequencyManager_DecreaseFreqStepSize(void);
uint32_t FrequencyManager_GetFrequencyStepSize(void);

// EEPROM Routines:
void FrequencyManager_WriteBandsToEeprom(void);
void FrequencyManager_ReadBandsFromEeprom(void);

#endif
