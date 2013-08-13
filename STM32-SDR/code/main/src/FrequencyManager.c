#include "FrequencyManager.h"
#include <assert.h>
#include "eeprom.h"
#include "Si570.h"



// Band Values:
typedef struct
{
	const char* Name;
	const uint32_t Setpoint;
	uint32_t CurrentFrequency;
} BandsStruct;

static BandsStruct s_bandsData[] = {
		// Note: Does not initialize the CurrentFrequency; done in initialize function.
	{ " 20 M PSK ",  14070000},
	{ " 80 M PSK ",  3580000},
	{ " 40 M PSKj",  7028000},
	{ " 40 M PSKe",  7040000},
	{ " 40 M PSKu",  7070000},
	{ "10 MHz WWV",  10000000},
	{ " 30 M PSK ",  10138000},
	{ " 20 M SSB ",  14200000},
	{ "15 MHz WWV",  15000000},
	{ " 17 M PSK ",  18100000},
	{ " 15 M PSK ",  21070000},
	{ " 12 M PSK ",  24920000},
	{ " 10 M PSK ",  28120000},
	{ "  6 M PSK ",  50250000},
	{ " SI570 F0 ",  56320000}, 	// SI570 startup frequency
};
static BandPreset s_selectedBand = FREQBAND_20M_PSK;


// Stepping
#define MAX_STEP_SIZE 100000
#define MIN_STEP_SIZE 1
static uint32_t s_stepSize = 100;

#define FREQUENCY_MIN   2000000 //   2 MHz
#define FREQUENCY_MAX 500000000 // 500 MHz

#define EEPROM_FREQBAND_OFFSET 100
#define EEPROM_SENTINEL_LOC 0
#define EEPROM_SENTINEL_VAL 1235


static uint8_t s_frequencyMultiplier = 2;

void FrequencyManager_Initialize(void)
{

	uint32_t EEProm_Value1 = Read_Long_EEProm(EEPROM_SENTINEL_LOC); //Read the 0 address to see if SI570 data has been stored

	if (EEProm_Value1 != EEPROM_SENTINEL_VAL) {
		FrequencyManager_ResetBandsToDefault();
		FrequencyManager_WriteBandsToEeprom();
		Write_Long_EEProm(EEPROM_SENTINEL_LOC, EEPROM_SENTINEL_VAL);
	}
	else {
		FrequencyManager_ReadBandsFromEeprom();
	}

	// Initialize the F0 for the radio:
	F0 = (double) s_bandsData[FREQBAND_SI570_F0].CurrentFrequency;

	FrequencyManager_SetSelectedBand(FREQBAND_20M_PSK);
}

void FrequencyManager_ResetBandsToDefault(void)
{
	for (int i = 0; i < FREQBAND_NUMBER_OF_BANDS; i++) {
		s_bandsData[i].CurrentFrequency = s_bandsData[i].Setpoint;
	}
}

/*
 * Work with preset bands:
 */
void FrequencyManager_SetSelectedBand(BandPreset newBand)
{
	assert(newBand >= 0 && newBand < FREQBAND_NUMBER_OF_BANDS);
	s_selectedBand = newBand;
	FrequencyManager_SetCurrentFrequency(s_bandsData[newBand].CurrentFrequency);
}
BandPreset FrequencyManager_GetSelectedBand(void)
{
	return s_selectedBand;
}


const char* FrequencyManager_GetBandName(BandPreset band)
{
	assert(band >= 0 && band < FREQBAND_NUMBER_OF_BANDS);
	return s_bandsData[band].Name;
}
uint32_t FrequencyManager_GetBandValue(BandPreset band)
{
	assert(band >= 0 && band < FREQBAND_NUMBER_OF_BANDS);
	return s_bandsData[band].Setpoint;
}

/*
 * Get/Set current frequency to radio (in Hz)
 */
void FrequencyManager_SetCurrentFrequency(uint32_t newFrequency)
{
	// Change the radio frequency:
	if ((newFrequency < FREQUENCY_MIN) || (newFrequency > FREQUENCY_MAX)) {
		return;
	}

	// Handle a normal frequency or an option:
	// Set the startup frequency (F0)
	if (s_selectedBand == FREQBAND_SI570_F0) {
		F0 = (double) newFrequency;
		if (SI570_Chk != 3) {
			Compute_FXTAL();
		}
	}
	// Normal frequency: Set it.
	else {
		if (SI570_Chk != 3) {
			Output_Frequency(newFrequency * s_frequencyMultiplier);
		}
	}

	// If we made it through the above, record the value.
	s_bandsData[s_selectedBand].CurrentFrequency = newFrequency;
}
uint32_t FrequencyManager_GetCurrentFrequency(void)
{
	return s_bandsData[s_selectedBand].CurrentFrequency;
}

void FrequencyManager_SetFreqMultiplier(int16_t newFreqMult)
{
	s_frequencyMultiplier = newFreqMult;
}

/*
 * Manage stepping the frequency (for example, using the adjustment knob)
 */
void FrequencyManager_StepFrequencyUp(void)
{
	uint32_t newFreq = FrequencyManager_GetCurrentFrequency() + s_stepSize;
	FrequencyManager_SetCurrentFrequency(newFreq);
}
void FrequencyManager_StepFrequencyDown(void)
{
	uint32_t newFreq = FrequencyManager_GetCurrentFrequency() - s_stepSize;
	FrequencyManager_SetCurrentFrequency(newFreq);
}
void FrequencyManager_IncreaseFreqStepSize(void)
{
	if (s_stepSize < MAX_STEP_SIZE) {
		s_stepSize *= 10;
	}
}
void FrequencyManager_DecreaseFreqStepSize(void)
{
	if (s_stepSize > MIN_STEP_SIZE) {
		s_stepSize /= 10;
	}
}
uint32_t FrequencyManager_GetFrequencyStepSize(void)
{
	return s_stepSize;
}


/*
 * EEPROM Routines
 */
void FrequencyManager_WriteBandsToEeprom(void)
{
	for (int i = 0; i < FREQBAND_NUMBER_OF_BANDS; i++) {
		Write_Long_EEProm(EEPROM_FREQBAND_OFFSET + i * 4, s_bandsData[i].CurrentFrequency);
	}
}
void FrequencyManager_ReadBandsFromEeprom(void)
{
	for (int i = 0; i < FREQBAND_NUMBER_OF_BANDS; i++) {
		s_bandsData[i].CurrentFrequency = Read_Long_EEProm(EEPROM_FREQBAND_OFFSET+ i * 4);
	}
}
