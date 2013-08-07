// General code to allow screen management for the UI
#include "screen_All.h"
#include "LcdHal.h"
#include <assert.h>

static GL_Page_TypeDef *s_pCurrentScreen = 0;

/*
 * DIRECTIONS:
 * See screen_All.h
 */

// Global variable for each screen
// ## Add new global for each new screen.
GL_Page_TypeDef g_screenMain;
GL_Page_TypeDef g_screenCalibrate;
GL_Page_TypeDef g_screenCalibrationTest;
GL_Page_TypeDef g_screenMode;
GL_Page_TypeDef g_screenOptions;
GL_Page_TypeDef g_screenFrequencies;


// Create all screens
void Screen_CreateAllScreens(void)
{
	// Create (populate/allocate/...) each screen.
	// ## Add new screens to this list.
	ScreenMain_Create();
	ScreenCalibrate_Create();
	ScreenCalibrationTest_Create();
	ScreenMode_Create();
	ScreenOptions_Create();
	ScreenFrequencies_Create();
}

// Change to a new screen.
void Screen_ShowScreen(GL_Page_TypeDef *pNewScreen)
{
	assert(pNewScreen != 0);
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


