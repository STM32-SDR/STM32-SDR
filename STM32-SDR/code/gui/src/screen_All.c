/*
 * General code to allow screen management for the UI
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
#include "ModeSelect.h"
#include "LcdHal.h"
#include <assert.h>
#include "xprintf.h"

static GL_Page_TypeDef *s_pCurrentScreen = 0;

/*
 * DIRECTIONS:
 * See screen_All.h
 */

// Global variable for each screen
// ## Add new global for each new screen.
GL_Page_TypeDef g_screenMainPSK;
GL_Page_TypeDef g_screenMainCW;
GL_Page_TypeDef g_screenMainSSB;
GL_Page_TypeDef g_screenCalibrate;
GL_Page_TypeDef g_screenCalibrationTest;
GL_Page_TypeDef g_screenMode;
GL_Page_TypeDef g_screenOptions;
GL_Page_TypeDef g_screenAdvanced;
GL_Page_TypeDef g_screenFilter;
GL_Page_TypeDef g_screenEditProgText;
GL_Page_TypeDef g_screenFrequencies;
GL_Page_TypeDef g_screenEditText;
GL_Page_TypeDef g_screenEditTagText;

typedef struct{
_Bool isPressed;
int counter;
GL_PageControls_TypeDef* pointer;
} buttonMemory;

static buttonMemory button = {0, 0, 0};

// Create all screens
void Screen_CreateAllScreens(void)
{
	debug(GUI, "Screen_CreateAllScreens:\n");
	// Create (populate/allocate/...) each screen.
	// ## Add new screens to this list.
	ScreenMainPSK_Create();
	ScreenMainCW_Create();
	ScreenMainSSB_Create();
	ScreenCalibrate_Create();
	ScreenCalibrationTest_Create();
	ScreenMode_Create();
	ScreenOptions_Create();
	ScreenAdvanced_Create();
	ScreenFilter_Create();
	ScreenFrequencies_Create();
	ScreenEditText_Create();
	ScreenEditTagText_Create();
	ScreenEditProgText_Create();
	debug(GUI, "Screen_CreateAllScreens: Done\n");
}

// Change to a new screen.
void Screen_ShowScreen(GL_Page_TypeDef *pNewScreen)
{
	debug(GUI, "Screen_ShowScreen:1\n");
	assert(pNewScreen != 0);
	// Break out if already on this screen.
	debug(GUI, "Screen_ShowScreen:2\n");
	if (pNewScreen == s_pCurrentScreen) {
		debug(GUI, "Screen_ShowScreen:3\n");
		return;
	}
	debug(GUI, "Screen_ShowScreen:4\n");
	if (s_pCurrentScreen != 0) {
		debug(GUI, "Screen_ShowScreen:5\n");
		s_pCurrentScreen->ShowPage(s_pCurrentScreen, GL_FALSE);
		debug(GUI, "Screen_ShowScreen:6\n");
	}
	GL_Clear(LCD_COLOR_BLACK);
	GL_SetBackColor(LCD_COLOR_BLACK);
	GL_SetTextColor(LCD_COLOR_WHITE);

	// Second parameter as true forces the screen to redraw itself
	// as opposed to allowing it choose what to redraw based on need.
	pNewScreen->ShowPage(pNewScreen, GL_TRUE);
	debug(GUI, "Screen_ShowScreen:7\n");
	s_pCurrentScreen = pNewScreen;
	debug(GUI, "Screen_ShowScreen:8\n");

	// set default waterfall/spectrum display

}

// Transition to the correct main screen, based on the mode.
void Screen_ShowMainScreen(void)
{
	debug(GUI, "Screen_ShowMainScreen:\n");
	switch(Mode_GetCurrentMode()) {
	case MODE_PSK:
		Screen_ShowScreen(&g_screenMainPSK);
		break;
	case MODE_CW:
		Screen_ShowScreen(&g_screenMainCW);
		break;
	case MODE_SSB:
		Screen_ShowScreen(&g_screenMainSSB);
		break;
	default:
		assert(0);
	}
}

void Screen_ButtonAnimate(GL_PageControls_TypeDef* pThis)
{
	GL_Button_TypeDef* pThat = (GL_Button_TypeDef*) (pThis->objPTR);
	pThat->isObjectTouched = GL_TRUE;
	pThis->SetObjVisible(pThis, pThis->objCoordinates);
	button.pointer = pThis;
	button.counter=0;
	button.isPressed = 1;
}

void Screen_Update (void){
	if (button.isPressed) {
		button.counter++;
		if (button.counter>20000){
			GL_Button_TypeDef* pThat = (GL_Button_TypeDef*) (button.pointer->objPTR);
			if (pThat->isObjectTouched == GL_TRUE){
				pThat->isObjectTouched = GL_FALSE;
				button.pointer->SetObjVisible(button.pointer, button.pointer->objCoordinates);
			}
			button.isPressed = 0;
			button.counter = 0;
		}
	}
}
