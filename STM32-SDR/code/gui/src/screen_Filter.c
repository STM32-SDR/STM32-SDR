/*
 * code for Filter Options screen
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

#include "screen_All.h"
#include "widgets.h"
#include "options.h"
#include <assert.h>
#include "FrequencyManager.h"
#include "xprintf.h"
#include "Encoders.h"
#include <string.h>

// Used in this file to refer to the correct screen (helps to keep code copy-paste friendly.
static GL_Page_TypeDef *s_pThisScreen = &g_screenFilter;

static GL_PageControls_TypeDef* lblBandTableCode[8];
static GL_PageControls_TypeDef* lblBandTableFreq[9];

//used to store frequency to be restored when done
#define MIN_BAND 0
#define MAX_BAND 7

static uint32_t saveFrequency;
static int filterCodeSelected = MIN_BAND;
static int filterFreqSelected = MIN_BAND;

/**
 * Call-back prototypes
 */
static void optionsCodeUp_Click(GL_PageControls_TypeDef* pThis);
static void optionsCodeDown_Click(GL_PageControls_TypeDef* pThis);
static void optionsFreqUp_Click(GL_PageControls_TypeDef* pThis);
static void optionsFreqDown_Click(GL_PageControls_TypeDef* pThis);
static void doneFilter_Click(GL_PageControls_TypeDef* pThis);
static void defaultsFilter_Click(GL_PageControls_TypeDef* pThis);
static _Bool FilterDisplayUpdateCodeHandler(GL_PageControls_TypeDef* pThis, _Bool forceRedisplay);
static _Bool FilterDisplayUpdateFreqHandler(GL_PageControls_TypeDef* pThis, _Bool forceRedisplay);

#define ID_FILTER_OPTION_START      160
#define FIRST_BUTTON_Y       40
#define SPACE_PER_BUTTON_Y   35
#define LEFT_COL_X           0
#define COLUMN_WIDTH         90
#define BUTTONS_PER_COLUMN   3
#define BUTTONS_PER_ADV_COLUMN   4

/**
 * Create the screen
 */

void ScreenFilter_Create(void)
{
	//pointers to filter manager strings
	char *text;
	char str[16];
	Create_PageObj(s_pThisScreen);

	/*
	 * Create the UI widgets
	 */
	GL_PageControls_TypeDef* lblTitle = Widget_NewLabel("Filter", LCD_COLOR_WHITE, LCD_COLOR_BLACK, 0, GL_FONTOPTION_16x24, 0);
	GL_PageControls_TypeDef* lblDirections2 = Widget_NewLabel("Set code & freqency", LCD_COLOR_WHITE, LCD_COLOR_BLACK, 0, GL_FONTOPTION_8x12Bold, 0);
	GL_PageControls_TypeDef* lblBandHeading1 = Widget_NewLabel("bits", LCD_COLOR_WHITE, LCD_COLOR_BLACK, 0, GL_FONTOPTION_8x16, 0);
	GL_PageControls_TypeDef* lblBandHeading2 = Widget_NewLabel("      kHz", LCD_COLOR_WHITE, LCD_COLOR_BLACK, 0, GL_FONTOPTION_8x16, 0);
	GL_PageControls_TypeDef* btnFreq = Widget_NewBigButtonFrequency();
	GL_PageControls_TypeDef* btnDefaults = NewButton(0, "Defaults", defaultsFilter_Click);
	GL_PageControls_TypeDef* btnDone = NewButton(0, "  Done  ", doneFilter_Click);

	/*
	 * Place the widgets onto the screen
	 */
	// Title
	AddPageControlObj(  0,   0, lblTitle,        s_pThisScreen);
	AddPageControlObj(130,  0, lblDirections2,  s_pThisScreen);
	AddPageControlObj(96, 16, lblBandHeading1,  s_pThisScreen);
	AddPageControlObj(136, 16, lblBandHeading2,  s_pThisScreen);

	// Show up/down buttons

	GL_PageControls_TypeDef* btnOptionCodeUp = NewButton(ID_FILTER_OPTION_START +0 , "  Up  ", optionsCodeUp_Click);
	AddPageControlObj(0, 43, btnOptionCodeUp, s_pThisScreen);
	GL_PageControls_TypeDef* btnOptionCodeDown = NewButton(ID_FILTER_OPTION_START +0 , " Down ", optionsCodeDown_Click);
	AddPageControlObj(0, 160, btnOptionCodeDown, s_pThisScreen);
	GL_PageControls_TypeDef* btnOptionFreqUp = NewButton(ID_FILTER_OPTION_START +0 , "  Up  ", optionsFreqUp_Click);
	AddPageControlObj(240, 43, btnOptionFreqUp, s_pThisScreen);
	GL_PageControls_TypeDef* btnOptionFreqDown = NewButton(ID_FILTER_OPTION_START +0 , " Down ", optionsFreqDown_Click);
	AddPageControlObj(240, 160, btnOptionFreqDown, s_pThisScreen);

	// Fetched band filter table

	for (int i = MIN_BAND; i <= MAX_BAND; i++)
	{

		//Define the set of filter band buttons
		//table of filter band settings

		lblBandTableCode[i] = Widget_NewLabel(" blank ", LCD_COLOR_WHITE, LCD_COLOR_BLACK, 0, GL_FONTOPTION_8x16, FilterDisplayUpdateCodeHandler);
		AddPageControlObj(80, 36 + 21 * i, lblBandTableCode[i],  s_pThisScreen);

		text = FrequencyManager_Code_ascii(i);
		strcpy(str, text);
		Widget_ChangeLabelText(lblBandTableCode[i], str);

		if (i != MAX_BAND){
		lblBandTableFreq[i] = Widget_NewLabel("- blank -", LCD_COLOR_WHITE, LCD_COLOR_BLACK, 0, GL_FONTOPTION_8x16, FilterDisplayUpdateFreqHandler);
		AddPageControlObj(136, 47 + 21 * i, lblBandTableFreq[i],  s_pThisScreen);

		text = FrequencyManager_Freq_ascii(i);
		strcpy(str, text);
		Widget_ChangeLabelText(lblBandTableFreq[i], str);
		}
	}

	// Button Row
	AddPageControlObj(
			LCD_WIDTH - ((GL_Custom_TypeDef*)(btnFreq->objPTR))->GetWidth(btnFreq),
			LCD_HEIGHT - ((GL_Custom_TypeDef*)(btnFreq->objPTR))->GetHeight(btnFreq),
			btnFreq, s_pThisScreen);
	AddPageControlObj(85, LCD_HEIGHT - 30, btnDefaults, s_pThisScreen);
	AddPageControlObj(0, LCD_HEIGHT - 30, btnDone, s_pThisScreen);

}

static _Bool FilterDisplayUpdateCodeHandler(GL_PageControls_TypeDef* pThis, _Bool forceRedisplay){

	char *text;
	char str[20];

	for (int i = 0; i <= MAX_BAND; i++)
	{
		text = FrequencyManager_Code_ascii(i);
		if (i == filterCodeSelected)
			strcpy(str, "-> ");
		else
			strcpy(str, "   ");
		strcat(str, text);
		Widget_ChangeLabelText(lblBandTableCode[i], str);
	}
	return 0;
}

static _Bool FilterDisplayUpdateFreqHandler(GL_PageControls_TypeDef* pThis, _Bool forceRedisplay){

	char *text;
	char str[20];

	for (int i = 0; i < MAX_BAND; i++)
	{
		text = FrequencyManager_Freq_ascii(i);
		if (i == filterFreqSelected && i != MAX_BAND)
			strcpy(str, "-> ");
		else
			strcpy(str, "   ");
		strcat(str, text);
		Widget_ChangeLabelText(lblBandTableFreq[i], str);
	}
	return 0;
}

int Screen_GetFilterCodeID (){
	return filterCodeSelected;
}

int Screen_GetFilterFreqID (){
	return filterFreqSelected;
}

/**
 * Callbacks
 */
static void optionsCodeUp_Click(GL_PageControls_TypeDef* pThis)
{
	Screen_ButtonAnimate(pThis);
	filterCodeSelected--;
	if (filterCodeSelected < MIN_BAND)
		filterCodeSelected = MAX_BAND;
	debug(GUI, "optionsCodeUp_Click: ID = %d\n", filterCodeSelected);
	int newCode = FrequencyManager_GetFilterCode (filterCodeSelected);
	FrequencyManager_Output_FilterCode(newCode);
}

static void optionsCodeDown_Click(GL_PageControls_TypeDef* pThis)
{
	Screen_ButtonAnimate(pThis);
	filterCodeSelected++;
	if (filterCodeSelected > MAX_BAND)
		filterCodeSelected = MIN_BAND;
	debug(GUI, "optionsCodeDown_Click: ID = %d\n", filterCodeSelected);
	int newCode = FrequencyManager_GetFilterCode (filterCodeSelected);
	FrequencyManager_Output_FilterCode(newCode);
}

static void optionsFreqUp_Click(GL_PageControls_TypeDef* pThis)
{
	Screen_ButtonAnimate(pThis);
	filterFreqSelected--;
	if (filterFreqSelected < MIN_BAND)
		filterFreqSelected = MAX_BAND-1;
	uint32_t setFreq = FrequencyManager_GetFilterFrequency(filterFreqSelected);
	FrequencyManager_SetCurrentFrequency(setFreq);
	debug(GUI, "optionsCodeUp_Click: ID = %d\n", filterFreqSelected);
}

static void optionsFreqDown_Click(GL_PageControls_TypeDef* pThis)
{
	Screen_ButtonAnimate(pThis);
	filterFreqSelected++;
	if (filterFreqSelected > MAX_BAND-1)
		filterFreqSelected = MIN_BAND;
	uint32_t setFreq = FrequencyManager_GetFilterFrequency(filterFreqSelected);
	FrequencyManager_SetCurrentFrequency(setFreq);
	debug(GUI, "optionsCodeDown_Click: ID = %d\n", filterFreqSelected);
}

static void defaultsFilter_Click(GL_PageControls_TypeDef* pThis)
{
	Screen_ButtonAnimate(pThis);
	debug (GUI, "defaultsFilter_Click\n");
	FrequencyManager_ResetFiltersToDefault();
}

static void doneFilter_Click(GL_PageControls_TypeDef* pThis) {

	debug (GUI, "doneFilter_Click\n");
	FrequencyManager_WriteFiltersToEeprom();
	FrequencyManager_SetCurrentFrequency(saveFrequency);

	FrequencyManager_SetFrequencyStepSize(100);
	Options_SetSelectedOption(OPTION_RX_AUDIO);

	Screen_SetScreenMode(MAIN);
	FrequencyManager_Check_FilterBand(saveFrequency); // Must be done in Main mode

	Screen_ShowMainScreen();
}

void Screen_filter_Click(GL_PageControls_TypeDef* pThis) {
	debug (GUI, "Screen_filter_Click:\n");

	// save the current synthesizer frequency to put back later and set new value
	saveFrequency = FrequencyManager_GetCurrentFrequency();
	uint32_t setFreq = FrequencyManager_GetFilterFrequency(filterFreqSelected);
	debug (GUI, "Set frequency to %d\n", setFreq);
	FrequencyManager_SetCurrentFrequency(setFreq);
	FrequencyManager_SetFrequencyStepSize(1000000);

	// output the selected code for the filter
	int newCode = FrequencyManager_GetFilterCode (filterCodeSelected);
	debug (GUI, "Set filter code to %d\n", newCode);
	FrequencyManager_Output_FilterCode(newCode);

	Screen_SetScreenMode(FILTER);
	Screen_PSK_SetTune(); //make sure we are in frequency tune mode
	Screen_ShowScreen(&g_screenFilter);
}

