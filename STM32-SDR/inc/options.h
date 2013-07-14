#include <stdint.h>

typedef struct
{
	const char* Name;
	const int16_t Initial;
	const int16_t Minimum;
	const int16_t Maximum;
	const int16_t ChangeUnits;
	int16_t CurrentValue;
} OptionStruct;

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
void         Options_Initialize(void);

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
