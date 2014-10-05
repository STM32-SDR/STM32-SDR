/*
 * Code for frequency manager and selection screen
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
#include <assert.h>
#include "FrequencyManager.h"
#include "Text_Enter.h"
#include "ModeSelect.h"
#include "xprintf.h"

// Used in this file to refer to the correct screen (helps to keep code copy-paste friendly.
static GL_Page_TypeDef *s_pThisScreen = &g_screenFrequencies;

// Store reference to lables to change them.
GL_PageControls_TypeDef* lbl1;
GL_PageControls_TypeDef* lbl2;
GL_PageControls_TypeDef* lbl3;
GL_PageControls_TypeDef* lbl4;

#define ID_FREQBTN_START     200
#define FIRST_BUTTON_Y       30
#define SPACE_PER_BUTTON_Y   30
#define LEFT_COL_X           0
#define COLUMN_WIDTH         110
#define BUTTONS_PER_COLUMN   5

#define TRUE 1
#define FALSE 0

static int editMode = FALSE;

extern void set_kybd_mode (int mode);

void displayPresetInstructions(void);
void displayEditInstructions(void);
// void displayBlank(void);

/**
 * Call-back prototypes
 */
static void freqButton_Click(GL_PageControls_TypeDef* pThis);
//static void store_Click(GL_PageControls_TypeDef* pThis);
//static void done_Click(GL_PageControls_TypeDef* pThis);
static void edit_Click(GL_PageControls_TypeDef* pThis);
static void mode_Click(GL_PageControls_TypeDef* pThis);

/**
 * Create the screen
 */
void ScreenFrequencies_Create(void)
{
	Create_PageObj(s_pThisScreen);

	/*
	 * Create the UI widgets
	 */
	GL_PageControls_TypeDef* lblTitle = Widget_NewLabel("Frequencies", LCD_COLOR_WHITE, LCD_COLOR_BLACK, 0, GL_FONTOPTION_12x12, 0);
	lbl1 = Widget_NewLabel("Tap a band to select.", LCD_COLOR_WHITE, LCD_COLOR_BLACK, 0, GL_FONTOPTION_8x12Bold, 0);
	lbl2 = Widget_NewLabel("", LCD_COLOR_WHITE, LCD_COLOR_BLACK, 0, GL_FONTOPTION_8x12Bold, 0);
	lbl3 = Widget_NewLabel("Press & turn frequency knob", LCD_COLOR_WHITE, LCD_COLOR_BLACK, 0, GL_FONTOPTION_8x12Bold, 0);
	lbl4 = Widget_NewLabel("to set frequency step size.", LCD_COLOR_WHITE, LCD_COLOR_BLACK, 0, GL_FONTOPTION_8x12Bold, 0);


	GL_PageControls_TypeDef* btnFreqBigButton = Widget_NewBigButtonFrequency();
//	GL_PageControls_TypeDef* btnStore = NewButton(0, " Store  ", store_Click);
	GL_PageControls_TypeDef* btnEdit = NewButton(0, " Edit ", edit_Click);
//	GL_PageControls_TypeDef* btnDone = NewButton(0, "  Done  ", done_Click);


	/*
	 * Place the widgets onto the screen
	 */
	// Title
	AddPageControlObj(  0,   0, lblTitle,        s_pThisScreen);
//	AddPageControlObj(200,   0, lblDirections1,  s_pThisScreen);
//	AddPageControlObj(200,  10, lblDirections2,  s_pThisScreen);

	// Populate the frequency buttons:
	for (int i = 0; i < FREQBAND_NUMBER_OF_BANDS; i++) {
		GL_PageControls_TypeDef* btnFreq = NewButton(ID_FREQBTN_START + i, FrequencyManager_DisplayBandName(i), freqButton_Click);

		int x = (i / BUTTONS_PER_COLUMN) * COLUMN_WIDTH;
		int y = (i % BUTTONS_PER_COLUMN) * SPACE_PER_BUTTON_Y + FIRST_BUTTON_Y;
		AddPageControlObj(x, y, btnFreq, s_pThisScreen);
	}

	// Button Row
	AddPageControlObj(150, 0, lbl1,  s_pThisScreen);
	AddPageControlObj(150, 10, lbl2,  s_pThisScreen);
	AddPageControlObj(0,   LCD_HEIGHT - 62, lbl3,  s_pThisScreen);
	AddPageControlObj(0,   LCD_HEIGHT - 46, lbl4,  s_pThisScreen);
//	AddPageControlObj(80,   LCD_HEIGHT - 30, btnStore, s_pThisScreen);
	AddPageControlObj(0,  LCD_HEIGHT - 30, btnEdit, s_pThisScreen);
//	AddPageControlObj(0, LCD_HEIGHT - 30, btnDone, s_pThisScreen);
	AddPageControlObj(
			LCD_WIDTH - ((GL_Custom_TypeDef*)(btnFreqBigButton->objPTR))->GetWidth(btnFreqBigButton),
			LCD_HEIGHT - ((GL_Custom_TypeDef*)(btnFreqBigButton->objPTR))->GetHeight(btnFreqBigButton),
			btnFreqBigButton, s_pThisScreen);

	// show preset instruction at bottom

}
/*
void showBuffers (char* buffer1, char* buffer2, char* buffer3, char* buffer4){
	debug(GUI, "showBuffers:\n");
	GL_SetFont(GL_FONTOPTION_8x16);
	GL_SetBackColor(LCD_COLOR_BLACK);
	GL_SetTextColor(LCD_COLOR_YELLOW);
	GL_PrintString(170, 0, buffer1, 0);
	GL_PrintString(170, 10, buffer2, 0);
	GL_PrintString(0, LCD_HEIGHT - 62, buffer3, 0);
	GL_PrintString(46, LCD_HEIGHT - 46, buffer4, 0);
}
*/

void displayPresetInstructions(void){
	debug(GUI, "displayPresetInstructions:\n");
//	char buf1[] = "Tap band to select";
//	char buf2[] = "";
//	char buf3[] = "Press & turn frequency knob to set";
//	char buf4[] = "frequency step size.";
	Widget_ChangeLabelText (lbl1, "Tap a band to select.");
	Widget_ChangeLabelText (lbl2, "");
	Widget_ChangeLabelText (lbl3, "Press & turn frequency knob to set");
	Widget_ChangeLabelText (lbl4, "the frequency step size.");
}

void displayEditInstructions(void){
	debug(GUI, "displayEditInstructions:\n");
//	char buf1[] = "Tap band to edit";
//	char buf2[] = "";
//	char buf3[] = "";
//	char buf4[] = "";
	Widget_ChangeLabelText (lbl1, "Tap band to edit.    ");
	Widget_ChangeLabelText (lbl2, "");
	Widget_ChangeLabelText (lbl3, "                                  ");
	Widget_ChangeLabelText (lbl4, "                            ");
}
/*
void displayBlank (void){
	debug(GUI, "displayBlank:\n");
	char buf1[] = "                           ";
	char buf2[] = "                           ";
	char buf3[] = "                           ";
	char buf4[] = "                       ";
}
*/
/*
static void displayNormalFeedback(void)
{
	debug(GUI, "freqButton_Click:\n");
//	Widget_ChangeLabelColour(lblStatus1, LCD_COLOR_WHITE);
//	Widget_ChangeLabelColour(lblStatus2, LCD_COLOR_WHITE);
	Widget_ChangeLabelText(lblStatus1, "Press & turn frequency knob ");
	Widget_ChangeLabelText(lblStatus2, "to set frequency step size. ");
}
static void displayStoreFeedback(void)
{
	Widget_ChangeLabelColour(lblStatus1, LCD_COLOR_YELLOW);
	Widget_ChangeLabelColour(lblStatus2, LCD_COLOR_YELLOW);
	Widget_ChangeLabelText(lblStatus1, "Frequencies for each band   ");
	Widget_ChangeLabelText(lblStatus2, "now saved in EEPROM.        ");
}

static void displayDefaultsFeedback(void)
{
	Widget_ChangeLabelColour(lblStatus1, LCD_COLOR_YELLOW);
	Widget_ChangeLabelColour(lblStatus2, LCD_COLOR_YELLOW);
	Widget_ChangeLabelText(lblStatus1, "Frequencies for each band   ");
	Widget_ChangeLabelText(lblStatus2, "now reset to default.       ");
}
*/

static void freqEditButton_Click(GL_PageControls_TypeDef* pThis)
{
	debug(GUI, "freqEditButton_Click:\n");
	uint16_t id = pThis->ID - ID_FREQBTN_START;
	editMode=TRUE;
	//assert(id >= 0 && id <= Text_Items);
	Freq_SetSelectedText(id);
//	clearFreqLabel();
//	displayBlank ();
	displayEditInstructions ();
	FreqName_Display();
	set_kybd_mode(3);
}

void Screen_ChangeButtonFreqLabel(int i){
	debug(GUI, "Screen_ChangeButtonFreqLabel:\n");
	ChangeButtonText(s_pThisScreen, ID_FREQBTN_START + i, FrequencyManager_DisplayBandName(i));
	FrequencyManager_SaveCurrentFrequency();
}

/**
 * Callbacks
 */
static void freqButton_Click(GL_PageControls_TypeDef* pThis)
{
	debug(GUI, "freqButton_Click:\n");

	uint16_t userBand = pThis->ID - ID_FREQBTN_START;
	assert(userBand >= 0 && userBand <= FREQBAND_NUMBER_OF_BANDS);
	if (editMode==TRUE){
		FrequencyManager_SaveCurrentFrequency();
		FrequencyManager_SetSelectedBand(userBand);
		freqEditButton_Click(pThis);
	} else {
		FrequencyManager_SetSelectedBand(userBand);
		Screen_SetScreenMode(MAIN);
//		displayBlank ();
		displayPresetInstructions ();
		Screen_ShowMainScreen();

		// Reset message to clear any saved/defaults message.
//		displayNormalFeedback();
	}
}
/*
static void store_Click(GL_PageControls_TypeDef* pThis)
{
	debug(GUI, "store_Click:\n");
//	displayStoreFeedback();
	FrequencyManager_SaveCurrentFrequency();
	FrequencyManager_WriteBandsToEeprom();
	editMode=FALSE;
//	displayBlank ();
	displayPresetInstructions ();
	Screen_ShowMainScreen();
}
*/

/*
static void defaults_Click(GL_PageControls_TypeDef* pThis)
{
	displayDefaultsFeedback();
	FrequencyManager_ResetBandsToDefault();
}
*/

static void edit_Click(GL_PageControls_TypeDef* pThis)
{
	debug(GUI, "edit_Click:\n");

	if (Screen_GetScreenMode()==FREQUENCY){
		GL_PageControls_TypeDef* btnMode = NewButton(220, " Mode ", mode_Click);
		AddPageControlObj(160,   LCD_HEIGHT - 30, btnMode, s_pThisScreen);
		RefreshPageControl (s_pThisScreen, 220);
		Screen_SetScreenMode(FREQEDIT);
		displayEditInstructions ();
		editMode=TRUE;
	} else {
		FrequencyManager_SaveCurrentFrequency();
		Screen_SetScreenMode(FREQUENCY);
		editMode=FALSE;
		DestroyPageControl(s_pThisScreen, 220);
		RefreshPageControl (s_pThisScreen, 220);
		displayPresetInstructions ();
	}
}
/*
static void done_Click(GL_PageControls_TypeDef* pThis) {
	// Reset to defaults in preparation for next display.
//	displayNormalFeedback();
	debug(GUI, "done_Click:\n");
	editMode=FALSE;
//	displayBlank ();
	DestroyPageControl(s_pThisScreen, 220);
	DestroyPageControl(s_pThisScreen, 221);
	Screen_SetScreenMode(MAIN);
	displayPresetInstructions ();
	Screen_ShowMainScreen();
}
*/

static void mode_Click(GL_PageControls_TypeDef* pThis) {
	// Reset to defaults in preparation for next display.
//	displayNormalFeedback();
	debug(GUI, "mode_Click:\n");
	UserModeType userMode = Mode_GetCurrentUserMode();
	debug(GUI, "mode_Click:userMode = before %d\n", userMode);
	if (userMode<USERMODE_NUM_MODES-2)
		userMode++;
	else
		userMode = USERMODE_USB;
	debug(GUI, "mode_Click:userMode after = %d\n", userMode);
	Mode_SetCurrentMode(userMode);
	FreqName_Display();
}

void Screen_FreqDone(void) {
	// Reset to defaults in preparation for next display.
//	displayNormalFeedback();
	debug(GUI, "screen_FreqDone:\n");
	FrequencyManager_SaveCurrentFrequency();
	Screen_ChangeButtonFreqLabel(FrequencyManager_GetSelectedBand());
	editMode=FALSE;
//	displayBlank ();
	DestroyPageControl(s_pThisScreen, 220);
//	DestroyPageControl(s_pThisScreen, 221);
	Screen_SetScreenMode(MAIN);
	displayPresetInstructions ();
	Screen_ShowMainScreen();
}
