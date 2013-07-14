#include "options.h"
#include "Codec_Gains.h"
#include <assert.h>

static OptionNumber s_currentOptionNumber = OPTION_RX_AUDIO;

static OptionStruct s_optionsData[] = {
	{
		/*Name*/ " Rx Audio ",
		/*Init*/ 0,
		/*Min */ DAC_GAIN_MIN,
		/*Max */ DAC_GAIN_MAX,
		/*Rate*/ 1,
		/*Data*/ 0
	},
	{
		/*Name*/ "   Rx RF  ",
		/*Init*/ 20,
		/*Min */ 0,
		/*Max */ 80,
		/*Rate*/ 1,
		/*Data*/ 0,
	},
	{
		/*Name*/ " SSB Level",
		/*Init*/ 20,
		/*Min */ 0,
		/*Max */ 80,
		/*Rate*/ 1,
		/*Data*/ 0,
	},
	{
		/*Name*/ " CW Level ",
		/*Init*/ 0,
		/*Min */ -127,
		/*Max */ 29,
		/*Rate*/ 1,
		/*Data*/ 0,
	},
	{
		/*Name*/ " PSK Level",
		/*Init*/ 0,
		/*Min */ -127,
		/*Max */ 29,
		/*Rate*/ 1,
		/*Data*/ 0,
	},
	{
		/*Name*/ " ST Level ",
		/*Init*/ 0,
		/*Min */ 0,
		/*Max */ 29,
		/*Rate*/ 1,
		/*Data*/ 0,
	},
	{
		/*Name*/ "   Rx Amp ",
		/*Init*/ 5000,
		/*Min */ 1000,
		/*Max */ 10000,
		/*Rate*/ 10,
		/*Data*/ 0,
	},
	{
		/*Name*/ " Rx Phase ",
		/*Init*/ 0,
		/*Min */ -10000,
		/*Max */ 10000,
		/*Rate*/ 10,
		/*Data*/ 0,
	},
	{
		/*Name*/ "   Tx Amp ",
		/*Init*/ 5000,
		/*Min */ 1000,
		/*Max */ 10000,
		/*Rate*/ 10,
		/*Data*/ 0,
	},
	{
		/*Name*/ " Tx Phase ",
		/*Init*/ 0,
		/*Min */ -10000,
		/*Max */ 10000,
		/*Rate*/ 10,
		/*Data*/ 0,
	},
	{
		/*Name*/ " Mic Bias ",
		/*Init*/ 0,
		/*Min */ 0,
		/*Max */ 1,
		/*Rate*/ 1,
		/*Data*/ 0,
	},
	{0,0,0,0,0,0}	// Terminator with a null pointer description
};

// Prototypes EEPROM Access
static void writeToEEPROM(void);
static _Bool haveValidEEPROMData(void);
static void readFromEEPROM(void);

// Initialization
void Options_Initialize(void)
{
	// Load default values
	for (int i = 0; i < NUM_OPTIONS; i++) {
		Options_SetValue(i, s_optionsData[i].Initial);
	}

	// Load from EEPROM if valid:
	if (haveValidEEPROMData()) {
		readFromEEPROM();
	}
}

// Work with option data
const char* Options_GetName(int optionIdx)
{
	assert(optionIdx >= 0 && optionIdx < NUM_OPTIONS);
	return s_optionsData[optionIdx].Name;
}
uint16_t Options_GetValue(int optionIdx)
{
	assert(optionIdx >= 0 && optionIdx < NUM_OPTIONS);
	return s_optionsData[optionIdx].CurrentValue;
}
void Options_SetValue(int optionIdx, int16_t newValue)
{
	assert(optionIdx >= 0 && optionIdx < NUM_OPTIONS);
	s_optionsData[optionIdx].CurrentValue = newValue;
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
	assert(newOption >= 0 && newOption < NUM_OPTIONS);
	s_currentOptionNumber = newOption;
}


// EEPROM Access
static void writeToEEPROM(void)
{

}
static _Bool haveValidEEPROMData(void)
{
	return 0;
}
static void readFromEEPROM(void)
{

}
