// General code to allow screen management for the UI
#include "screen_All.h"
#include "ModeSelect.h"
#include "LcdHal.h"
#include <assert.h>

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
GL_Page_TypeDef g_screenFrequencies;
GL_Page_TypeDef g_screenEditText;

// Create all screens
void Screen_CreateAllScreens(void)
{
	// Create (populate/allocate/...) each screen.
	// ## Add new screens to this list.
	ScreenMainPSK_Create();
	ScreenMainCW_Create();
	ScreenMainSSB_Create();
	ScreenCalibrate_Create();
	ScreenCalibrationTest_Create();
	ScreenMode_Create();
	ScreenOptions_Create();
	ScreenFrequencies_Create();
	ScreenEditText_Create();
}

// Change to a new screen.
void Screen_ShowScreen(GL_Page_TypeDef *pNewScreen)
{
	assert(pNewScreen != 0);

	// Break out if already on this screen.
	if (pNewScreen == s_pCurrentScreen) {
		return;
	}

	if (s_pCurrentScreen != 0) {
		s_pCurrentScreen->ShowPage(s_pCurrentScreen, GL_FALSE);
	}
	GL_Clear(LCD_COLOR_BLACK);
	GL_SetBackColor(LCD_COLOR_BLACK);
	GL_SetTextColor(LCD_COLOR_WHITE);

	// Second parameter as true forces the screen to redraw itself
	// as opposed to allowing it choose what to redraw based on need.
	pNewScreen->ShowPage(pNewScreen, GL_TRUE);

	s_pCurrentScreen = pNewScreen;
}

// Transition to the correct main screen, based on the mode.
void Screen_ShowMainScreen(void)
{
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
