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
#include "xprintf.h"
//#include <assert.h>

// Used in this file to refer to the correct screen (helps to keep code copy-paste friendly.
static GL_Page_TypeDef *s_pThisScreen = &g_screenEditText;

static GL_PageControls_TypeDef* s_lblStatus1;
static GL_PageControls_TypeDef* s_lblStatus2;

#define TRUE 1
#define FALSE 0

static int editMode = FALSE;


/**
 * Call-back prototypes
 */
static void textButton_Click(GL_PageControls_TypeDef* pThis);
static void tagEditButton_Click(GL_PageControls_TypeDef* pThis);
//static void store_Click(GL_PageControls_TypeDef* pThis);
static void done_Click(GL_PageControls_TypeDef* pThis);
//static void edit_Click(GL_PageControls_TypeDef* pThis);
static void clearLabel(void);
static void resetLabel(void);

#define ID_TEXT_START      200
#define FIRST_BUTTON_Y       0
#define SPACE_PER_BUTTON_Y   35
#define LEFT_COL_X           0
#define COLUMN_WIDTH         60
#define BUTTONS_PER_COLUMN   4

/**
 * Create the screen
 */
void ScreenEditText_Create(void)
{
	debug(GUI, "ScreenEditText_Create:\n");
	Create_PageObj(s_pThisScreen);

	/*
	 * Create the UI widgets
	 */

	s_lblStatus1 = Widget_NewLabel("Press to Edit.", LCD_COLOR_WHITE, LCD_COLOR_BLACK, 0, GL_FONTOPTION_8x12Bold, 0);
	s_lblStatus2 = Widget_NewLabel("Keyboard Functions", LCD_COLOR_WHITE, LCD_COLOR_BLACK, 0, GL_FONTOPTION_8x12Bold, 0);
//	GL_PageControls_TypeDef* btnStore = NewButton(0, " Store  ", store_Click);
//	GL_PageControls_TypeDef* btnEdit = NewButton(0, "  Edit  ", edit_Click);
	GL_PageControls_TypeDef* btnDone = NewButton(0, "  Done  ", done_Click);
	GL_PageControls_TypeDef* btnTagEdit = NewButton(0, "  Tags  ", tagEditButton_Click);


	// Populate the options buttons:

	for (int i = 0; i <= Text_F12; i++) {
		static GL_PageControls_TypeDef* btnText;
		btnText = NewButton(ID_TEXT_START + i, Text_GetName(i), textButton_Click);

		int x = (i / BUTTONS_PER_COLUMN) * COLUMN_WIDTH;
		int y = (i % BUTTONS_PER_COLUMN) * SPACE_PER_BUTTON_Y + FIRST_BUTTON_Y;
		AddPageControlObj(x, y, btnText, s_pThisScreen);
	}

	// Button Row

	AddPageControlObj(200, 85, s_lblStatus1,  s_pThisScreen);
	AddPageControlObj(180, 0, s_lblStatus2,  s_pThisScreen);
//	AddPageControlObj(80 , LCD_HEIGHT - 30, btnStore, s_pThisScreen);
//	AddPageControlObj(160, LCD_HEIGHT - 30, btnEdit, s_pThisScreen);
	AddPageControlObj(0, LCD_HEIGHT - 30, btnDone, s_pThisScreen);
	AddPageControlObj(240, LCD_HEIGHT - 30, btnTagEdit, s_pThisScreen);
}

void Screen_ChangeButtonTextLabel(int i){
	debug(GUI, "screen_ChangeButtonText: i = \n", i);
	ChangeButtonText(s_pThisScreen, ID_TEXT_START + i, Text_GetName(i));
}

static void resetLabel(void)
{
	debug(GUI, "resetLabel:\n");
	Widget_ChangeLabelColour(s_lblStatus1, LCD_COLOR_WHITE);
//	Widget_ChangeLabelColour(s_lblStatus2, LCD_COLOR_WHITE);
	Widget_ChangeLabelText(s_lblStatus1, "Press to Edit.      ");
}


static void clearLabel(void)
{
	debug(GUI, "clearLabel:\n");
	Widget_ChangeLabelColour(s_lblStatus1, LCD_COLOR_WHITE);
	Widget_ChangeLabelText(s_lblStatus1, "                    ");
//	Widget_ChangeLabelColour(s_lblStatus2, LCD_COLOR_WHITE);
//	Widget_ChangeLabelText(s_lblStatus2, "                    ");
}

static void displayStoreFeedback(void)
{
	debug(GUI, "displayStoreFeedback:\n");
	clearLabel();
	Widget_ChangeLabelColour(s_lblStatus1, LCD_COLOR_YELLOW);
//	Widget_ChangeLabelColour(s_lblStatus2, LCD_COLOR_YELLOW);
	Widget_ChangeLabelText(s_lblStatus1, "Writing data        ");
//	Widget_ChangeLabelText(s_lblStatus2, "to EEPROM.          ");
}

/**
 * Callbacks
 */
static void textButton_Click(GL_PageControls_TypeDef* pThis)
{
	debug(GUI, "textButton_Click:\n");
	Screen_ButtonAnimate(pThis);
	uint16_t id = pThis->ID - ID_TEXT_START;
	editMode=TRUE;
	if (editMode){
		//assert(id >= 0 && id <= Text_Items);
		Text_SetSelectedText(id);
		clearLabel();
		Item_Display(id);
		Text_Wipe();
		Text_Display(id, textPointer);
		set_kybd_mode(3);
	}
}

/*static void store_Click(GL_PageControls_TypeDef* pThis)
{
	Count_WriteToEEPROM();
	Text_WriteToEEPROM();
	displayStoreFeedback();
}

static void edit_Click(GL_PageControls_TypeDef* pThis)
{
	debug (GUI, "edit_Click\n");
	if (editMode) {
		editMode = FALSE;
		clearLabel();
		resetLabel();
		Text_Blank();
		set_kybd_mode(0);
	}
	else {
		editMode = TRUE;
		clearLabel();
		Widget_ChangeLabelColour(s_lblStatus1, LCD_COLOR_YELLOW);
		Widget_ChangeLabelText(s_lblStatus1, "Tap item to Edit.   ");
	}
}
*/
static void done_Click(GL_PageControls_TypeDef* pThis) {
	// Change text back to normal for next time.
	debug (GUI, "done_Click\n");
	Screen_TextDone();
}

void Screen_TextDone(){
	debug (GUI, "Screen_TextDone\n");
	Screen_ChangeButtonTextLabel(s_currentTextNumber);
	displayStoreFeedback();
	Count_WriteToEEPROM();
	Text_WriteToEEPROM();
	clearLabel();
	resetLabel();
	set_kybd_mode(0);
	Screen_SetScreenMode(OPTIONS);
	Screen_ShowScreen(&g_screenOptions);

}

static void tagEditButton_Click(GL_PageControls_TypeDef* pThis) {
	// Change text back to normal for next time.
	debug (GUI, "tagEditButton_Click\n");
	clearLabel();
	resetLabel();
	set_kybd_mode(0);
	Screen_SetScreenMode(FUNCTION);
	Screen_ShowScreen(&g_screenEditTagText);
}


