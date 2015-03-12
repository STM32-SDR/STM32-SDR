/*
 * Code for the Frequency Manager functions
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

#include "FrequencyManager.h"
#include <assert.h>
#include <stdlib.h>
#include "eeprom.h"
#include "Si570.h"
#include "xprintf.h"
#include "screen_All.h"
#include "stm32f4xx_gpio.h"
#include "Band_Filter.h"
#include "ModeSelect.h"
#include "Init_I2C.h"
//#include "number_images.h"

int NCOTUNE;
int NCO_Point;
int NCOTuneCount;
int NCO_Flag;

// Band Values:

typedef struct
{
	char  Name[freqLabel_length+1];
	uint32_t Setpoint;
	uint32_t CurrentFrequency;
	UserModeType Mode;
} BandsStruct;

BandsStruct s_bandsData[] = {
		// Note: Does not initialize the CurrentFrequency; done in initialize function.
		{ " 20m PSK",  14070000, 0, USERMODE_DIGU},
		{ " 40m PSK",    7070000, 0, USERMODE_DIGL},
		{ "10 MHz WWV",  10000000, 0, USERMODE_USB},
		{ "15 MHz WWV",  15000000, 0, USERMODE_USB},
		{ " 40m QRP",   7285000, 0, USERMODE_LSB},
		{ " 20m QRP",  14285000, 0, USERMODE_USB},
		{ " 10m SSB",  28885000, 0, USERMODE_USB},
		{ " 15m SSB",  21385000, 0, USERMODE_USB},
		{ " 40m",  70200000, 0, USERMODE_USB},
		{ " 80m SSB",   3729000, 0, USERMODE_LSB},
		{ " 40m SSB",   7268500, 0, USERMODE_LSB},
		{ " 40m",   7090000, 0, USERMODE_LSB},
	    { " 6m SSB",  50885000, 0, USERMODE_USB},
	    { " 2m SSB", 144285000, 0, USERMODE_USB},
	    { "SI570 F0 ",  56320000, 0, USERMODE_USB},
	    { "",         0, 0, USERMODE_USB},
};

static BandPreset s_selectedBand = FREQBAND_20M_PSK;

// Stepping
#define MAX_STEP_SIZE 1000000
#define MIN_STEP_SIZE 1
static uint32_t s_stepSize = 100;

#define FREQUENCY_MIN   1800000 //   1.8 MHz
#define FREQUENCY_MAX 500000000 // 500 MHz

#define EEPROM_FREQBAND_OFFSET 400
#define EEPROM_SENTINEL_LOC 0
//#define EEPROM_SENTINEL_VAL 1235
#define EEPROM_SENTINEL_VAL 1247

// Filter bands, See also options.h and options.c Options_GetValue
#define EEPROM_FILTERBAND_OFFSET 300
#define FREQ_NUMBER_OF_FILTER_BANDS 8

static uint8_t s_frequencyMultiplier = 4;

//band filter selection table
typedef struct
{
	uint8_t Code; //binary code used to select band filter via GPIO
	uint32_t Frequency; //highest frequency for the band selected
} bandFilter;

const static bandFilter s_defaultBandsData[8] = {
		{0, 3400000},
		{1, 6900000},
		{2, 9900000},
		{3, 13900000},
		{4, 18000000},
		{5 ,20900000},
		{6, 27900000},
		{7, 500000000}};

static bandFilter s_bandTable[8];

static int oldCode = 8; //Detect band code changes, Initialized to invalid value

static uint32_t splitTxFrequency; // Used during split Tx/Rx
static uint32_t splitRxFrequency; // Used during split Tx/Rx
_Bool split=0;

void FrequencyManager_setSplit(_Bool setting){
	debug(GUI, "FrequencyManager_setSplit:\n");
	split = setting;
}

_Bool FrequencyManager_isSplit(void){
	debug(GUI, "FrequencyManager_isSplit:\n");
	return split;
}

void FrequencyManager_SaveTxFrequency(uint32_t savedFrequency){
	debug(GUI, "FrequencyManager_SaveTxFrequency:\n");
	splitTxFrequency = savedFrequency;
}

void FrequencyManager_SetTxFrequency(){
	debug(GUI, "FrequencyManager_SetTxFrequency:\n");
	splitRxFrequency = FrequencyManager_GetCurrentFrequency();
	FrequencyManager_SetCurrentFrequency(splitTxFrequency);
}

void FrequencyManager_SetRxFrequency(){
	debug(GUI, "FrequencyManager_SetRxFrequency:\n");
	FrequencyManager_SetCurrentFrequency(splitRxFrequency);
}

void FrequencyManager_Initialize(void)
{

	uint32_t EEProm_Value1 = Read_Long_EEProm(EEPROM_SENTINEL_LOC); //Read the 0 address to see if SI570 data has been stored

	if (EEProm_Value1 != EEPROM_SENTINEL_VAL) {
		FrequencyManager_ResetBandsToDefault();
		FrequencyManager_WriteBandsToEeprom();
		FrequencyManager_ResetFiltersToDefault();
		FrequencyManager_WriteFiltersToEeprom();
		Write_Long_EEProm(EEPROM_SENTINEL_LOC, EEPROM_SENTINEL_VAL);
	}
	else {
		FrequencyManager_ReadBandsFromEeprom();
		FrequencyManager_ReadFiltersFromEeprom();
	}

	// Initialize the F0 for the radio:
	F0 = (double) s_bandsData[FREQBAND_SI570_F0].CurrentFrequency;

	FrequencyManager_SetSelectedBand(FREQBAND_20M_PSK);

}

void FrequencyManager_ResetBandsToDefault(void)
{
	debug(GUI, "FrequencyManager_ResetBandsToDefault:\n");
	for (int i = 0; i < FREQBAND_NUMBER_OF_BANDS; i++) {
		s_bandsData[i].CurrentFrequency = s_bandsData[i].Setpoint;
	}
}

/*
 * Work with preset bands:
 */
void FrequencyManager_SetSelectedBand(BandPreset newBand)
{
	debug(GUI, "FrequencyManager_SetSelectedBand:\n");
	assert(newBand >= 0 && newBand <= FREQBAND_NUMBER_OF_BANDS); //Includes blank band
	s_selectedBand = newBand;
	uint32_t newFreq = s_bandsData[newBand].Setpoint;
	UserModeType newMode = s_bandsData[newBand].Mode;
	FrequencyManager_SetCurrentFrequency(newFreq);
	Mode_SetCurrentMode(newMode);
	FrequencyManager_Check_FilterBand(newFreq);
}
BandPreset FrequencyManager_GetSelectedBand(void)
{
	if(Screen_GetScreenMode() != FILTER)
		return s_selectedBand;
	else
		return NULL_BAND;
}

char* FrequencyManager_GetBandName(BandPreset band)
{
	assert(band >= 0 && band <= FREQBAND_NUMBER_OF_BANDS);
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

void FrequencyManager_SaveCurrentFrequency(void){
	debug(GUI, "FrequencyManager_SaveCurrentFrequency:\n");
	s_bandsData[s_selectedBand].Setpoint =FrequencyManager_GetCurrentFrequency();
	UserModeType newMode = Mode_GetCurrentUserMode();
	s_bandsData[s_selectedBand].Mode = newMode;
	FrequencyManager_WriteBandsToEeprom();
}

uint32_t FrequencyManager_GetCurrentFrequency (void)
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
	FrequencyManager_Check_FilterBand(newFreq);
//	FrequencyManager_DisplayFrequency();
}
void FrequencyManager_StepFrequencyDown(void)
{
	uint32_t newFreq = FrequencyManager_GetCurrentFrequency() - s_stepSize;
	FrequencyManager_SetCurrentFrequency(newFreq);
	FrequencyManager_Check_FilterBand(newFreq);
//	FrequencyManager_DisplayFrequency();
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

void FrequencyManager_SetFrequencyStepSize(uint32_t step)
{
	s_stepSize = step;
}

//External band filter needs to be selected
void FrequencyManager_Check_FilterBand(uint32_t newFreq){

// check which band we are in by testing the frequency against each filter limit
	int newCode;
	for (int i = 0; i<8; i++){
		if (newFreq > s_bandTable[i].Frequency){
		}
		else {
			newCode = s_bandTable[i].Code;
			if (newCode != oldCode){
				debug(GUI, "FrequencyManager_Check_FilterBand: ");
				debug(GUI, "changing filter band, frequency = %d\n", newFreq);
				if(Screen_GetScreenMode() != FILTER) // In filter screen code set by buttons
					FrequencyManager_Output_FilterCode(newCode);

//				debug(GUI, "FrequencyManager_Check_FilterBand: ");
//				debug(GUI, "changing band, Freq = %d, code = %d\n", newFreq, newCode);
//				GPIO_SetFilter (newCode);
//				oldCode = newCode;
			}
			break; // stop after finding a match
		}
	}
}

void FrequencyManager_Output_FilterCode(int newCode){

	if (newCode != oldCode){ //only output to GPIO if there is a change
		debug(GUI, "FrequencyManager_Output_FilterBand: ");
		debug(GUI, "changing code, code = %d\n", newCode);
		GPIO_SetFilter (newCode);
		oldCode = newCode;
	}
}

char* FrequencyManager_DisplayBandName (BandPreset id){

	static char buf1[freqLabel_length+1] = {0}; // +1 to allow space for for null
	int length = strlen(s_bandsData[id].Name);
	for (int c=0; c<freqLabel_length; c++){
		if (c< length)
			buf1[c] = s_bandsData[id].Name[c];
		else
			buf1[c] = ' '; //pad remainder of line with spaces
	}
	debug(KEYBOARD, "FrequencyManager_DisplayBandName: buf1 = %s\n", buf1);
	return &buf1[0];
}

/*
 * EEPROM Routines
 */
void FrequencyManager_WriteBandsToEeprom(void)
{
	debug (GUI, "FrequencyManager_WriteBandsToEeprom:");
	uint32_t eepromFreq;
	uint16_t eepromMode;
	char text;

	for (int i = 0; i < FREQBAND_NUMBER_OF_BANDS; i++) {
		eepromFreq = Read_Long_EEProm(EEPROM_FREQBAND_OFFSET + i * 6);
		if (s_bandsData[i].Setpoint != eepromFreq)
			Write_Long_EEProm(EEPROM_FREQBAND_OFFSET + i *+6, s_bandsData[i].Setpoint);
		eepromMode = Read_Int_EEProm(EEPROM_FREQBAND_OFFSET + i * 6 + 4);
		if (s_bandsData[i].Mode != eepromMode)
			Write_Int_EEProm(EEPROM_FREQBAND_OFFSET + i * 6 + 4, s_bandsData[i].Mode);
	}

	for (int i = 0; i < FREQBAND_NUMBER_OF_BANDS; i++) {
//		buf1 = FrequencyManager_DisplayBandName (i);
		for (int c=0; c<freqLabel_length; c++){
			text = (char)I2C_ReadEEProm(EEPROM_FREQBAND_OFFSET + (FREQBAND_NUMBER_OF_BANDS*6) + i*(freqLabel_length+1) + c);
					if (s_bandsData[i].Name[c] != text)
						I2C_WriteEEProm(EEPROM_FREQBAND_OFFSET + (FREQBAND_NUMBER_OF_BANDS*6)+ i*(freqLabel_length+1) + c, s_bandsData[i].Name[c]);
		}
	}
}

void FrequencyManager_ReadBandsFromEeprom(void)
{
	debug (GUI, "FrequencyManager_ReadBandsFromEeprom:");
	for (int i = 0; i < FREQBAND_NUMBER_OF_BANDS; i++) {
		s_bandsData[i].CurrentFrequency = Read_Long_EEProm(EEPROM_FREQBAND_OFFSET+ i * 6);
		s_bandsData[i].Setpoint = Read_Long_EEProm(EEPROM_FREQBAND_OFFSET+ i * 6);
		s_bandsData[i].Mode = Read_Int_EEProm(EEPROM_FREQBAND_OFFSET+ i * 6 + 4);
	}
	for (int i = 0; i < FREQBAND_NUMBER_OF_BANDS; i++) {
		for (int c=0; c<freqLabel_length; c++){
			s_bandsData[i].Name[c] = (char)I2C_ReadEEProm(EEPROM_FREQBAND_OFFSET + (FREQBAND_NUMBER_OF_BANDS*6) + i*(freqLabel_length+1) + c);
		}
	}
}

void FrequencyManager_WriteFiltersToEeprom(void)
{
	uint16_t eepromCode;
	uint32_t eepromFreq;
	debug(GUI, "FrequencyManager_WriteFiltersToEeprom:\n");
	for (int i = 0; i < FREQ_NUMBER_OF_FILTER_BANDS; i++) {
		eepromCode = Read_Int_EEProm(EEPROM_FILTERBAND_OFFSET + i*6);
		if (s_bandTable[i].Code != eepromCode)
			Write_Int_EEProm(EEPROM_FILTERBAND_OFFSET + i*6, s_bandTable[i].Code);
		eepromFreq = Read_Long_EEProm(EEPROM_FILTERBAND_OFFSET+ i*6 + 2);
		if (s_bandTable[i].Frequency != eepromFreq)
			Write_Long_EEProm(EEPROM_FILTERBAND_OFFSET + i*6 + 2, s_bandTable[i].Frequency);
		debug(GUI, "i = %d, code = %d, frequency = %d \n", i/6, s_bandTable[i].Code, s_bandTable[i].Frequency );
	}
}
void FrequencyManager_ReadFiltersFromEeprom(void)
{
	debug(GUI, "FrequencyManager_ReadFiltersFromEeprom:\n");
	for (int i = 0; i < FREQ_NUMBER_OF_FILTER_BANDS; i++) {
		s_bandTable[i].Code = Read_Int_EEProm(EEPROM_FILTERBAND_OFFSET + i*6);
		s_bandTable[i].Frequency = Read_Long_EEProm(EEPROM_FILTERBAND_OFFSET+ i*6 + 2);
		debug(GUI, "i = %d, code = %d, frequency = %d \n", i, s_bandTable[i].Code, s_bandTable[i].Frequency );
	}
}

// Setting of Band Filter

void FrequencyManager_ResetFiltersToDefault(void)
{
	debug(GUI, "FrequencyManager_ResetFiltersToDefault:\n");
	for (int i = 0; i < FREQ_NUMBER_OF_FILTER_BANDS; i++){
	s_bandTable[i].Code = s_defaultBandsData[i].Code;
	s_bandTable[i].Frequency = s_defaultBandsData[i].Frequency;
	debug(GUI, "i = %d, code = %d, frequency = %d \n", i, s_bandTable[i].Code, s_bandTable[i].Frequency );

	}
}

void FrequencyManager_SetBandCodeFilter (uint8_t band, uint8_t value)
{
	debug(GUI, "FrequencyManager_SetBandCodeFilter:\n");
	s_bandTable[band].Code = value;
	debug(GUI, "band = %d, code = %d\n", band, value);
	FrequencyManager_Output_FilterCode(value);
}

void FrequencyManager_SetBandFreqFilter (uint8_t band, uint32_t frequency)
{
	debug(GUI, "FrequencyManager_SetBandFreqFilter:\n");
	s_bandTable[band].Frequency = frequency;
	debug(GUI, "band = %d, frequency = %d\n", band, frequency);
}
uint8_t FrequencyManager_GetFilterCode (int band)
{
	return s_bandTable[band].Code;
}

uint32_t FrequencyManager_GetFilterFrequency (int band)
{
	return s_bandTable[band].Frequency;
}

//Generate a string for band and code in format "n bbb "

char* FrequencyManager_Code_ascii(int band){

	int val = FrequencyManager_GetFilterCode(band);
	static char buf[5] = {0};
	int i = 3;

	for(; i ; --i, val /= 2)
		buf[i] = "01"[val % 2];

	return &buf[i+1];

}

char* FrequencyManager_Freq_ascii(int band){

	static char buf[8] = {0};
	int val = FrequencyManager_GetFilterFrequency(band)/1000;
	int i = 6;

	for(; i && val ; --i, val /= 10)
		buf[i] = "0123456789"[val % 10];

	//replace leading 0 with space
	for(; i ; --i, val /= 10)
			buf[i] = " "[0];
	return &buf[i+1];

}

void Tune_NCO_Up (void){
	NCO_Point += 1;
	if (NCO_Point > 240) NCO_Point = 240;
	NCOTuneCount=5;
	NCO_Flag = 1;
}

void Tune_NCO_Down (void)
{
	NCO_Point -= 1;
	if (NCO_Point < 0) NCO_Point = 0;
	NCOTuneCount=5;
	NCO_Flag = 1;
}
