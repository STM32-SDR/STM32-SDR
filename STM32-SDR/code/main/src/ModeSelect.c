/*
 * Code that controls the selection of the different modes (LSB,USB,CW, CER, PSKU, PSKL)
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

#include "ModeSelect.h"
#include <assert.h>
#include "DMA_IRQ_Handler.h"
#include "ChangeOver.h"
#include "widgets.h"
#include "STM32-SDR-Subroutines.h"

extern double NCO_Frequency;
extern int NCOTUNE;
extern int WF_Flag;
extern void Screen_PSK_SetTune(void);
extern int NCO_Point;

typedef struct
{
	const char*        Name;
	const char*        Description;
	const UserModeType UserMode;
	const ModeType     Mode;
	const SideBandType SideBand;
	const double	   DefaultNCO;
} ModeStruct;

static ModeStruct s_modeData[] = {
	{
		/*Name*/ " USB ",
		/*Desc*/ "SSB on Upper Side Band >10MHz.",
		/*Usr */ USERMODE_USB,
		/*Mode*/ MODE_SSB,
		/*Side*/ SIDEBAND_USB,
		/*NCO*/  1000.,
	},
	{
		/*Name*/ " LSB ",
		/*Desc*/ "SSB on Lower Side Band <10MHz.",
		/*Usr */ USERMODE_LSB,
		/*Mode*/ MODE_SSB,
		/*Side*/ SIDEBAND_LSB,
		/*NCO*/  1000.,
	},
	{
		/*Name*/ " CW  ",
		/*Desc*/ "CW Normal (LSB).",
		/*Usr */ USERMODE_CW,
		/*Mode*/ MODE_CW,
		/*Side*/ SIDEBAND_LSB,
		/*NCO*/  550.
	},
	{
		/*Name*/ " CWR ",
		/*Desc*/ "CW Reverse (USB).",
		/*Usr */ USERMODE_CWR,
		/*Mode*/ MODE_CW,
		/*Side*/ SIDEBAND_USB,
		/*NCO*/  550.
	},
	{
		/*Name*/ "PSK-U",
		/*Desc*/ "PSK31 (USB).",
		/*Usr */ USERMODE_DIGU,
		/*Mode*/ MODE_PSK,
		/*Side*/ SIDEBAND_USB,
		/*NCO*/  1500.
	},
	{
		/*Name*/ "PSK-L",
		/*Desc*/ "PSK31 (LSB).",
		/*Usr */ USERMODE_DIGL,
		/*Mode*/ MODE_PSK,
		/*Side*/ SIDEBAND_LSB,
		/*NCO*/  1500.
	},
	{
		/*Name*/ "Tune ",
		/*Desc*/ "Carrier of 1kHz USB tone",
		/*Usr */ USERMODE_TUNE,
		/*Mode*/ MODE_PSK,
		/*Side*/ SIDEBAND_USB,
		/*NCO*/  1000.
	},
};

static ModeStruct* s_pCurrentMode = &s_modeData[USERMODE_USB];


void Mode_Init(void)
{
	//Mode_SetCurrentMode(USERMODE_USB);
	Mode_SetCurrentMode(USERMODE_DIGU);
}
void Mode_SetCurrentMode(UserModeType newMode)
{
	// Change to receive whenever we switch mode
	RxTx_SetReceive();

	// Store the change in mode
	assert(newMode >= 0 && newMode < USERMODE_NUM_MODES);
	s_pCurrentMode = &s_modeData[newMode];

	// No need to change the mode directly with other code now;
	// ISR queries this module each time mode is needed.

	// Change the operation band:
	if (s_pCurrentMode->SideBand == SIDEBAND_LSB) {
		rgain = -0.5;
		NCOTUNE = 0;
	}
	else if (s_pCurrentMode->SideBand == SIDEBAND_USB) {
		rgain = 0.5;
		NCOTUNE = 0;
	}
	else {
		assert(0);
	}

	NCO_Frequency = s_pCurrentMode->DefaultNCO;

	switch (newMode) {
		case USERMODE_LSB:
		case USERMODE_USB:
		case USERMODE_CW:
		case USERMODE_CWR:
			RxTx_SetReceive();
			WF_Flag = 0;
			No_Filt (); //default to AF filter off
			break;
		case USERMODE_DIGU:
		case USERMODE_DIGL:
			RxTx_SetReceive();
			WF_Flag = 1;
			Screen_PSK_SetTune(); //move to up arrow on tune button
			break;
		case USERMODE_TUNE:
			WF_Flag = 0;
			Screen_PSK_SetTune(); //move to up arrow on tune button
			NCO_Point = 55; //Position for 1kHz
			RxTx_SetTransmit();
			break;
		default:
			break;
	}
	Init_Waterfall();
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
