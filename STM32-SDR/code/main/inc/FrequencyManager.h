// Control the selected frequency, its value, and how it changes.
#ifndef _FREQUENCYMANAGER_H_
#define _FREQUENCYMANAGER_H_

#include <stdint.h>

typedef enum {
	FREQBAND_20M_PSK = 0,
	FREQBAND_80M_PSK,
	FREQBAND_40M_PSKj,
	FREQBAND_40M_PSKe,
	FREQBAND_40M_PSKu,
	FREQBAND_10MHZ_MMV,
	FREQBAND_30M_PSK,
	FREQBAND_20M_SSB,
	FREQBAND_15MHZ_MMV,
	FREQBAND_17M_PSK,
	FREQBAND_15M_PSK,
	FREQBAND_12M_PSK,
	FREQBAND_10M_PSK,
	FREQBAND_6M_PSK,
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
