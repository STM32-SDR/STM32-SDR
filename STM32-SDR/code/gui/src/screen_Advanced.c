/*
 * code for  Advanced Options screen
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
static GL_Page_TypeDef *s_pThisScreen = &g_screenAdvanced;

static GL_PageControls_TypeDef* s_lblStatus;

//used to store frequency to be restored when done

/**
 * Call-back prototypes
 */
static void optionButton_Click(GL_PageControls_TypeDef* pThis);
static void defaults_Click(GL_PageControls_TypeDef* pThis);
static void calibrate_Click(GL_PageControls_TypeDef* pThis);

#define ID_OPTION_START      100
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

void ScreenAdvanced_Create(void)
{
	Create_PageObj(s_pThisScreen);

	/*
	 * Create the UI widgets
	 */
	GL_PageControls_TypeDef* lblTitle = Widget_NewLabel("Advanced", LCD_COLOR_WHITE, LCD_COLOR_BLACK, 0, GL_FONTOPTION_16x24, 0);
	GL_PageControls_TypeDef* lblDirections1 = Widget_NewLabel("Tap to select", LCD_COLOR_WHITE, LCD_COLOR_BLACK, 0, GL_FONTOPTION_8x12Bold, 0);
	GL_PageControls_TypeDef* lblDirections2 = Widget_NewLabel("setting changes value", LCD_COLOR_WHITE, LCD_COLOR_BLACK, 0, GL_FONTOPTION_8x12Bold, 0);
	s_lblStatus = Widget_NewLabel("Tap setting to exit", LCD_COLOR_WHITE, LCD_COLOR_BLACK, 0, GL_FONTOPTION_8x12Bold, 0);


	GL_PageControls_TypeDef* btnOptionsBigButton = Widget_NewBigButtonOptions();
	GL_PageControls_TypeDef* btnDefaults = NewButton(0, "Defaults", defaults_Click);
	GL_PageControls_TypeDef* btnCalibrate = NewButton(13, "Cal. Screen", calibrate_Click);

	/*
	 * Place the widgets onto the screen
	 */
	// Title
	AddPageControlObj(  0,   0, lblTitle,        s_pThisScreen);
	AddPageControlObj(115,   0, lblDirections1,  s_pThisScreen);
	AddPageControlObj(115,  10, lblDirections2,  s_pThisScreen);

	// Populate the options buttons:
	for (int i = START_ADVANCED_OPTIONS; i < END_ADVANCED_OPTIONS; i++) {
		GL_PageControls_TypeDef* btnOption = NewButton(ID_OPTION_START + i, Options_GetName(i), optionButton_Click);

		int x = ((i - START_ADVANCED_OPTIONS) / BUTTONS_PER_ADV_COLUMN) * COLUMN_WIDTH;
		int y = ((i - START_ADVANCED_OPTIONS) % BUTTONS_PER_ADV_COLUMN) * SPACE_PER_BUTTON_Y + FIRST_BUTTON_Y;
		AddPageControlObj(x, y, btnOption, s_pThisScreen);
	}

	// Button Row
	AddPageControlObj(0, LCD_HEIGHT - 42, btnOptionsBigButton, s_pThisScreen);
	AddPageControlObj(0, LCD_HEIGHT - 60, s_lblStatus,  s_pThisScreen);

	AddPageControlObj(204, FIRST_BUTTON_Y, btnCalibrate, s_pThisScreen);
	AddPageControlObj(85, LCD_HEIGHT - 30, btnDefaults, s_pThisScreen);
}

static void displayDefaultsFeedback(void)
{
	Widget_ChangeLabelColour(s_lblStatus, LCD_COLOR_YELLOW);
	Widget_ChangeLabelText(s_lblStatus, "All values reset to default.");
}

/**
 * Callbacks
 */
static void optionButton_Click(GL_PageControls_TypeDef* pThis)
{
	Screen_ButtonAnimate(pThis);
	uint16_t id = pThis->ID - ID_OPTION_START;
	assert(id >= 0 && id <= NUM_OPTIONS);
	Options_SetSelectedOption(id);
}

static void defaults_Click(GL_PageControls_TypeDef* pThis)
{
	displayDefaultsFeedback();
	Options_ResetToDefaults();
}

static void calibrate_Click(GL_PageControls_TypeDef* pThis) {
	Screen_ButtonAnimate(pThis);
	Screen_ShowScreen(&g_screenCalibrate);
}
