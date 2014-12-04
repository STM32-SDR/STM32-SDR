/*
 * code for Basic Options screen
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
#include "Init_I2C.h"  //Referenced for Delay(n);

// Used in this file to refer to the correct screen (helps to keep code copy-paste friendly.
static GL_Page_TypeDef *s_pThisScreen = &g_screenOptions;

static GL_PageControls_TypeDef* s_lblStatus;

static int screenMode = MAIN;

/**
 * Call-back prototypes
 */
static void optionButton_Click(GL_PageControls_TypeDef* pThis);
static void advanced_Click(GL_PageControls_TypeDef* pThis);
static void functionEditButton_Click(GL_PageControls_TypeDef* pThis);
static void programEditButton_Click(GL_PageControls_TypeDef* pThis);
static void tagEditButton_Click (GL_PageControls_TypeDef* pThis);

#define ID_OPTION_START      100
#define FIRST_BUTTON_Y       40
#define SPACE_PER_BUTTON_Y   35
#define LEFT_COL_X           0
#define COLUMN_WIDTH         90
#define BUTTONS_PER_COLUMN   3
#define BUTTONS_PER_ADV_COLUMN   4

/**
 * Create the screen
 */
void ScreenOptions_Create(void)
{
	Create_PageObj(s_pThisScreen);

	/*
	 * Create the UI widgets
	 */
	GL_PageControls_TypeDef* lblTitle = Widget_NewLabel("Settings", LCD_COLOR_WHITE, LCD_COLOR_BLACK, 0, GL_FONTOPTION_16x24, 0);
	GL_PageControls_TypeDef* lblDirections1 = Widget_NewLabel("Tap to select", LCD_COLOR_WHITE, LCD_COLOR_BLACK, 0, GL_FONTOPTION_8x12Bold, 0);
	GL_PageControls_TypeDef* lblDirections2 = Widget_NewLabel("setting changes value", LCD_COLOR_WHITE, LCD_COLOR_BLACK, 0, GL_FONTOPTION_8x12Bold, 0);
	s_lblStatus = Widget_NewLabel("Tap setting to exit", LCD_COLOR_WHITE, LCD_COLOR_BLACK, 0, GL_FONTOPTION_8x12Bold, 0);

	GL_PageControls_TypeDef* btnOptionsBigButton = Widget_NewBigButtonOptions();
	GL_PageControls_TypeDef* btnAdvanced = NewButton(15, " Advanced  ", advanced_Click);
	GL_PageControls_TypeDef* btnFunctions  = NewButton(17,  "Keyboard Fn", functionEditButton_Click);
	GL_PageControls_TypeDef* btnPrograms  = NewButton(18,  " Screen Fn ", programEditButton_Click);
	GL_PageControls_TypeDef* btnTags  = NewButton(19,  "  <Tags>   ", tagEditButton_Click);
	GL_PageControls_TypeDef* btnFilter = NewButton(20, "  Filter   ", Screen_filter_Click);
	/*
	 * Place the widgets onto the screen
	 */
	// Title
	AddPageControlObj(  0,   0, lblTitle,        s_pThisScreen);
	AddPageControlObj(115,   0, lblDirections1,  s_pThisScreen);
	AddPageControlObj(115,  10, lblDirections2,  s_pThisScreen);

	// Populate the options buttons:
	for (int i = START_BASIC_OPTIONS; i < END_BASIC_OPTIONS; i++) {
		GL_PageControls_TypeDef* btnOption = NewButton(ID_OPTION_START + i, Options_GetName(i), optionButton_Click);

		int x = (i / BUTTONS_PER_COLUMN) * COLUMN_WIDTH;
		int y = (i % BUTTONS_PER_COLUMN) * SPACE_PER_BUTTON_Y + FIRST_BUTTON_Y;
		AddPageControlObj(x, y, btnOption, s_pThisScreen);
	}

	// Button Row
	AddPageControlObj(0, LCD_HEIGHT - 42, btnOptionsBigButton, s_pThisScreen);
	AddPageControlObj(0, LCD_HEIGHT - 60, s_lblStatus,  s_pThisScreen);
	AddPageControlObj(220, FIRST_BUTTON_Y, btnAdvanced, s_pThisScreen);
	AddPageControlObj(220, FIRST_BUTTON_Y + SPACE_PER_BUTTON_Y, btnFunctions, s_pThisScreen);
	AddPageControlObj(220, FIRST_BUTTON_Y + 2 * SPACE_PER_BUTTON_Y, btnPrograms, s_pThisScreen);
	AddPageControlObj(220, FIRST_BUTTON_Y + 3 * SPACE_PER_BUTTON_Y, btnTags, s_pThisScreen);
	AddPageControlObj(220, FIRST_BUTTON_Y + 4 * SPACE_PER_BUTTON_Y, btnFilter, s_pThisScreen);
}

void Screen_SetScreenMode(int value) {
	screenMode = value;
	debug(GUI, "Screen_SetScreenMode\n", screenMode);
}

int Screen_GetScreenMode() {
	return screenMode;
}

/**
 * Callbacks
 */
static void optionButton_Click(GL_PageControls_TypeDef* pThis)
{
	uint16_t id = pThis->ID - ID_OPTION_START;
	assert(id >= 0 && id <= NUM_OPTIONS);
	Options_SetSelectedOption(id);
	if (id <= END_BASIC_OPTIONS)
		Screen_Done();
}

void Screen_Done() {

	Options_WriteToEEPROM();
	Screen_ShowMainScreen();
	Screen_SetScreenMode(MAIN);
}

void advanced_Click(GL_PageControls_TypeDef* pThis) {
	Screen_SetScreenMode(ADVANCED);
//	Options_SetSelectedOption(START_ADVANCED_OPTIONS);
	Screen_ShowScreen(&g_screenAdvanced);
}

static void functionEditButton_Click(GL_PageControls_TypeDef* pThis) {
	Screen_SetScreenMode(FUNCTION);
	Screen_ShowScreen(&g_screenEditText);
}

static void programEditButton_Click(GL_PageControls_TypeDef* pThis) {
	debug (GUI, "programEditButton_Click:1");
	Screen_SetScreenMode(FUNCTION);
	debug (GUI, "programEditButton_Click:2");
	Screen_ShowScreen(&g_screenEditProgText);
	debug (GUI, "programEditButton_Click: end");
}

static void tagEditButton_Click(GL_PageControls_TypeDef* pThis) {
	Screen_SetScreenMode(FUNCTION);
	Screen_ShowScreen(&g_screenEditTagText);

}
