/*
 * Code for edit tags in a screen
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
static GL_Page_TypeDef *s_pThisScreen = &g_screenEditTagText;

static GL_PageControls_TypeDef* s_lblStatus1;
static GL_PageControls_TypeDef* s_lblStatus2;

#define TRUE 1
#define FALSE 0

static int editMode = FALSE;

/**
 * Call-back prototypes
 */
static void tagButton_Click(GL_PageControls_TypeDef* pThis);
//static void store_Click(GL_PageControls_TypeDef* pThis);
static void done_Click(GL_PageControls_TypeDef* pThis);
//static void edit_Click(GL_PageControls_TypeDef* pThis);
static void clearTagLabel(void);
static void resetTagLabel(void);

#define ID_TEXT_START      250
#define FIRST_BUTTON_Y       0
#define SPACE_PER_BUTTON_Y   35
#define LEFT_COL_X           0
#define COLUMN_WIDTH         60
#define BUTTONS_PER_COLUMN   4

/**
 * Create the screen
 */
void ScreenEditTagText_Create(void)
{
	debug(GUI, "ScreenEditTagText_Create:\n");
	Create_PageObj(s_pThisScreen);

	/*
	 * Create the UI widgets
	 */
	debug(GUI, "ScreenEditTagText_Create: declare fixed buttons\n");
	s_lblStatus1 = Widget_NewLabel("Press to Edit.      ", LCD_COLOR_WHITE, LCD_COLOR_BLACK, 0, GL_FONTOPTION_8x12Bold, 0);
	s_lblStatus2 = Widget_NewLabel("Tag Editing", LCD_COLOR_WHITE, LCD_COLOR_BLACK, 0, GL_FONTOPTION_8x12Bold, 0);
	//	GL_PageControls_TypeDef* btnStore = NewButton(0, " Store  ", store_Click);
//	GL_PageControls_TypeDef* btnEdit = NewButton(0, "  Edit  ", edit_Click);
	GL_PageControls_TypeDef* btnDone = NewButton(0, "  Done  ", done_Click);

	debug(GUI, "ScreenEditTagText_Create: setup tag buttons\n");
	// Populate the options buttons:
	GL_PageControls_TypeDef* btnText;
	for (int i = 0; i <= Tag_10; i++) {
		btnText = NewButton(ID_TEXT_START + i, Tag_GetName(i), tagButton_Click);

		int x = (i / BUTTONS_PER_COLUMN) * COLUMN_WIDTH;
		int y = (i % BUTTONS_PER_COLUMN) * SPACE_PER_BUTTON_Y + FIRST_BUTTON_Y;
		AddPageControlObj(x, y, btnText, s_pThisScreen);
	}

	// Button Row

	debug(GUI, "ScreenEditTagText_Create: layout fixed buttons\n");

	AddPageControlObj(200, 85, s_lblStatus1,  s_pThisScreen);
	AddPageControlObj(180, 0, s_lblStatus2,  s_pThisScreen);
//	AddPageControlObj(80 , LCD_HEIGHT - 30, btnStore, s_pThisScreen);
//	AddPageControlObj(160, LCD_HEIGHT - 30, btnEdit, s_pThisScreen);
	AddPageControlObj(0, LCD_HEIGHT - 30, btnDone, s_pThisScreen);

	debug(GUI, "ScreenEditTagText_Create: Done\n");
}

void Screen_ChangeButtonTagLabel(int i){
	debug(GUI, "Screen_ChangeButtonTagLabel:\n");
	ChangeButtonText(s_pThisScreen, ID_TEXT_START + i, Tag_GetName(i));
}

static void resetTagLabel(void)
{
	debug(GUI, "resetTagLabel:\n");
	Widget_ChangeLabelColour(s_lblStatus1, LCD_COLOR_WHITE);
	Widget_ChangeLabelText(s_lblStatus1, "Press to Edit.      ");
}


static void clearTagLabel(void)
{
	debug(GUI, "clearTagLabel:\n");
	Widget_ChangeLabelColour(s_lblStatus1, LCD_COLOR_WHITE);
	Widget_ChangeLabelText(s_lblStatus1, "                    ");
}
/*
static void displayStoreFeedback(void)
{
	debug(GUI, "displayStoreFeedback:\n");
	clearTagLabel();
	Widget_ChangeLabelColour(s_lblStatus1, LCD_COLOR_YELLOW);
	Widget_ChangeLabelText(s_lblStatus1, "Values stored       ");
}
*/
/**
 * Callbacks
 */
static void tagButton_Click(GL_PageControls_TypeDef* pThis)
{
	debug(GUI, "tagButton_Click:\n");
	Screen_ButtonAnimate(pThis);
	uint16_t id = pThis->ID - ID_TEXT_START;
	editMode=TRUE;
	if (editMode){
		//assert(id >= 0 && id <= Text_Items);
		Tag_SetSelectedText(id);
		clearTagLabel();
		Text_Blank();
		TagItem_Display(id);
//		TagText_Wipe();
		TagText_Display(id, tagTextPointer);
		set_kybd_mode(3);
	}
}
/*
static void store_Click(GL_PageControls_TypeDef* pThis)
{
	debug(GUI, "store_Click:\n");
	Count_WriteToEEPROM();
	Text_WriteToEEPROM();
	displayStoreFeedback();
}

static void edit_Click(GL_PageControls_TypeDef* pThis)
{
	debug (GUI, "edit_Click\n");
	if (editMode) {
		editMode = FALSE;
		clearTagLabel();
		resetTagLabel();
		Tag_Blank();
		set_kybd_mode(0);
	}
	else {
		editMode = TRUE;
		clearTagLabel();
		Widget_ChangeLabelColour(s_lblStatus1, LCD_COLOR_YELLOW);
		Widget_ChangeLabelText(s_lblStatus1, "Tap item to Edit.   ");
	}

}
*/
static void done_Click(GL_PageControls_TypeDef* pThis) {
	// Change text back to normal for next time.
	debug (GUI, "done_Click\n");
	Screen_ChangeButtonTagLabel(s_currentTagNumber);
	Screen_TagDone();
}

void Screen_TagDone() {
	debug (GUI, "Screen_TagDone\n");
	Count_WriteToEEPROM();
	Text_WriteToEEPROM();
	clearTagLabel();
	resetTagLabel();
	set_kybd_mode(0);
	Screen_SetScreenMode(OPTIONS);
	Screen_ShowScreen(&g_screenOptions);
}



