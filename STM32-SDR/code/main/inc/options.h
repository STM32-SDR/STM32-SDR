#include <stdint.h>



typedef enum {
	OPTION_RX_AUDIO = 0,
	OPTION_RX_RF,
	OPTION_SSB_LEVEL,
	OPTION_CW_LEVEL,
	OPTION_PSK_LEVEL,
	OPTION_ST_LEVEL,
	OPTION_RX_AMP,
	OPTION_RX_PHASE,
	OPTION_TX_AMP,
	OPTION_TX_PHASE,
	OPTION_MIC_BIAS,

	NUM_OPTIONS
} OptionNumber;



// Initialization
void Options_Initialize(void);
void Options_ResetToDefaults(void);

// Work with option data
const char* Options_GetName(int optionIdx);
uint16_t Options_GetValue(int optionIdx);
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
