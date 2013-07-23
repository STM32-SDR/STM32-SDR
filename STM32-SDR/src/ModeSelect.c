/*
 * ModeSelect.c
 */
#include "ModeSelect.h"
#include <assert.h>
#include "DMA_IRQ_Handler.h"
#include "ChangeOver.h"

typedef struct
{
	const char*        Name;
	const char*        Description;
	const UserModeType UserMode;
	const ModeType     Mode;
	const SideBandType SideBand;
} ModeStruct;

static ModeStruct s_modeData[] = {
	{
		/*Name*/ " USB ",
		/*Desc*/ "Voice communication on USB.",
		/*Usr */ USERMODE_USB,
		/*Mode*/ MODE_SSB,
		/*Side*/ SIDEBAND_USB,
	},
	{
		/*Name*/ " LSB ",
		/*Desc*/ "Voice communication on LSB.",
		/*Usr */ USERMODE_LSB,
		/*Mode*/ MODE_SSB,
		/*Side*/ SIDEBAND_LSB,
	},
	{
		/*Name*/ " CW  ",
		/*Desc*/ "CW on the lower side band.",
		/*Usr */ USERMODE_CW,
		/*Mode*/ MODE_CW,
		/*Side*/ SIDEBAND_LSB,
	},
	{
		/*Name*/ " CWR ",
		/*Desc*/ "CW on the upper side band.",
		/*Usr */ USERMODE_CWR,
		/*Mode*/ MODE_CW,
		/*Side*/ SIDEBAND_USB,
	},
	{
		/*Name*/ "DIG-U",
		/*Desc*/ "Digital PSK31 on upper side band.",
		/*Usr */ USERMODE_DIGU,
		/*Mode*/ MODE_PSK,
		/*Side*/ SIDEBAND_USB,
	},
	{
		/*Name*/ "DIG-L",
		/*Desc*/ "Digital PSK31 on lower side band.",
		/*Usr */ USERMODE_DIGL,
		/*Mode*/ MODE_PSK,
		/*Side*/ SIDEBAND_LSB,
	},
};

static ModeStruct* s_pCurrentMode = &s_modeData[USERMODE_USB];


void Mode_Init(void)
{
	Mode_SetCurrentMode(USERMODE_USB);
}
void Mode_SetCurrentMode(UserModeType newMode)
{
	// Change to receive whenever we switch mode
	Receive_Sequence();

	// Store the change in mode
	assert(newMode >= 0 && newMode < USERMODE_NUM_MODES);
	s_pCurrentMode = &s_modeData[newMode];

	// No need to change the mode directly with other code now;
	// ISR queries this module each time mode is needed.

	// Change the operation band:
	if (s_pCurrentMode->SideBand == SIDEBAND_LSB) {
		rgain = -0.5;
	}
	else if (s_pCurrentMode->SideBand == SIDEBAND_USB) {
		rgain = 0.5;
	}
	else {
		assert(0);
	}
}

// Query current state:
const char* Mode_GetCurrentUserModeName(void)
{
	return s_pCurrentMode->Name;
}
const char* Mode_GetCurrentUserModeDescription(void)
{
	return s_pCurrentMode->Description;
}
UserModeType Mode_GetCurrentUserMode(void)
{
	return s_pCurrentMode->UserMode;
}
ModeType Mode_GetCurrentMode(void)
{
	return s_pCurrentMode->Mode;
}
SideBandType Mode_GetCurrentSideBand(void)
{
	return s_pCurrentMode->SideBand;
}

// Query based on a UserMode
const char* Mode_GetUserModeName(UserModeType userMode)
{
	return s_modeData[userMode].Name;
}
const char* Mode_GetUserModeDescription(UserModeType userMode)
{
	return s_modeData[userMode].Description;
}
