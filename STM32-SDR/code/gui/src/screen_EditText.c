/*
 * Code for edit text in a screen
 * used in setting parameters
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
#include "Text_Enter.h"
#include "Keyboard_Input.h"
//#include <assert.h>

// Used in this file to refer to the correct screen (helps to keep code copy-paste friendly.
static GL_Page_TypeDef *s_pThisScreen = &g_screenEditText;

static GL_PageControls_TypeDef* s_lblStatus;


/**
 * Call-back prototypes
 */
static void textButton_Click(GL_PageControls_TypeDef* pThis);
static void store_Click(GL_PageControls_TypeDef* pThis);
static void done_Click(GL_PageControls_TypeDef* pThis);
static void edit_Click(GL_PageControls_TypeDef* pThis);
static void clearLabel(void);
static void resetLabel(void);

#define ID_TEXT_START      200
#define FIRST_BUTTON_Y       0
#define SPACE_PER_BUTTON_Y   30
#define LEFT_COL_X           0
#define COLUMN_WIDTH         110
#define BUTTONS_PER_COLUMN   5

/**
 * Create the screen
 */
void ScreenEditText_Create(void)
{
	Create_PageObj(s_pThisScreen);

	/*
	 * Create the UI widgets
	 */

	s_lblStatus = Widget_NewLabel("Tap Item then Edit.", LCD_COLOR_WHITE, LCD_COLOR_BLACK, 0, GL_FONTOPTION_8x12Bold, 0);
	GL_PageControls_TypeDef* btnStore = NewButton(0, " Store ", store_Click);
	GL_PageControls_TypeDef* btnEdit = NewButton(0, " Edit ", edit_Click);
	GL_PageControls_TypeDef* btnDone = NewButton(0, "  Done  ", done_Click);


	// Populate the options buttons:
	for (int i = 0; i < Text_Items; i++) {
		GL_PageControls_TypeDef* btnText = NewButton(ID_TEXT_START + i, Text_GetName(i), textButton_Click);

		int x = (i / BUTTONS_PER_COLUMN) * COLUMN_WIDTH;
		int y = (i % BUTTONS_PER_COLUMN) * SPACE_PER_BUTTON_Y + FIRST_BUTTON_Y;
		AddPageControlObj(x, y, btnText, s_pThisScreen);
	}

	// Button Row

	AddPageControlObj(90, LCD_HEIGHT - 50, s_lblStatus,  s_pThisScreen);
	AddPageControlObj(120 , LCD_HEIGHT - 30, btnStore, s_pThisScreen);
	AddPageControlObj(0, LCD_HEIGHT - 30, btnEdit, s_pThisScreen);
	AddPageControlObj(LCD_WIDTH - 75, LCD_HEIGHT - 30, btnDone, s_pThisScreen);
}

static void resetLabel(void)
{
	Widget_ChangeLabelColour(s_lblStatus, LCD_COLOR_WHITE);
	Widget_ChangeLabelText(s_lblStatus, "Tap Item then Edit.");
}


static void clearLabel(void)
{
	Widget_ChangeLabelColour(s_lblStatus, LCD_COLOR_WHITE);
	Widget_ChangeLabelText(s_lblStatus, "                            ");
}

static void displayStoreFeedback(void)
{
	clearLabel();
	Widget_ChangeLabelColour(s_lblStatus, LCD_COLOR_YELLOW);
	Widget_ChangeLabelText(s_lblStatus, "Values stored to EEPROM.");
}

/**
 * Callbacks
 */
static void textButton_Click(GL_PageControls_TypeDef* pThis)
{
	uint16_t id = pThis->ID - ID_TEXT_START;
	//assert(id >= 0 && id <= Text_Items);
	Text_SetSelectedText(id);
	Item_Display(id);
	Text_Wipe();
	Text_Display(id);
}

static void store_Click(GL_PageControls_TypeDef* pThis)
{
	Count_WriteToEEPROM();
	Text_WriteToEEPROM();
	displayStoreFeedback();
}

static void edit_Click(GL_PageControls_TypeDef* pThis)
{
	Text_Clear(s_currentTextNumber);
	Text_Display(s_currentTextNumber);
	set_kybd_mode(3);
	text_cnt = 0;
}

static void done_Click(GL_PageControls_TypeDef* pThis) {
	// Change text back to normal for next time.
	clearLabel();
	resetLabel();
	set_kybd_mode(0);
	Screen_ShowScreen(&g_screenMainPSK);
}




