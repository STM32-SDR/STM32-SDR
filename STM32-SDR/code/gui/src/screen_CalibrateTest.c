/*
 *
 * Touchscreen Display
 * Display where the user is touching to demonstrate the calibration.
 * User taps same location 3 times to exit.
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
#include <assert.h>
#include <stdio.h>		// For sprintf()
#include <stdlib.h>		// For abs()
#include "TSHal.h"

// Text location
#define TITLE_TOP                (0)
#define DIRECTIONS_TOP           (25)
#define DISPLAY_TOP              (150)

// How wide a cross to make
#define TOUCH_POINT_RADIUS       (20)

// Tap same location repeatedly to quit.
#define SAME_LOCATION_THRESHOLD  (15)
#define MAX_SAME_LOCATION_COUNT  (2)		// For 3 the same, first one auto-counts.
static int s_sameLocationCounter = 0;

// Used in this file to refer to the correct screen (helps to keep code copy-paste friendly.
static GL_Page_TypeDef *s_pThisScreen = &g_screenCalibrationTest;;

// Track where (if?) the last touch was.
_Bool s_haveTouch = 0;
uint16_t s_lastTouchX = 0;
uint16_t s_lastTouchY = 0;

/**
 * Call-back prototypes
 */
static uint16_t showTouches_GetWidth(GL_PageControls_TypeDef* pThis);
static uint16_t showTouches_GetHeight(GL_PageControls_TypeDef* pThis);
static void showTouches_Click(GL_PageControls_TypeDef* pThis);
static void showTouches_Draw(GL_PageControls_TypeDef* pThis, _Bool force);


/**
 * Create the screen
 */
void ScreenCalibrationTest_Create(void)
{
	Create_PageObj(s_pThisScreen);

	GL_PageControls_TypeDef* displayWidget = NewCustomWidget(1, showTouches_GetWidth, showTouches_GetHeight, showTouches_Click, showTouches_Draw, 0);
	AddPageControlObj(0, 0, displayWidget, s_pThisScreen);
}


/**
 * UI Callbacks
 */
// Return max u-int because we want the touchscreen code to find *any* point inside this widget.
static uint16_t showTouches_GetWidth(GL_PageControls_TypeDef* pThis)
{
	return UINT16_MAX;
}
static uint16_t showTouches_GetHeight(GL_PageControls_TypeDef* pThis)
{
	return UINT16_MAX;
}

static void showTouches_Click(GL_PageControls_TypeDef* pThis)
{
	// Get calibrated touched location
	uint16_t x, y;
	TS_GetTouchEventCoords(&x, &y);
	s_haveTouch = 1;

	// Count if same location
	if ((abs(s_lastTouchX - x) < SAME_LOCATION_THRESHOLD)
		&& (abs(s_lastTouchY - y) < SAME_LOCATION_THRESHOLD)) {
		s_sameLocationCounter++;
	} else {
		s_sameLocationCounter = 0;
	}
	if (s_sameLocationCounter >= MAX_SAME_LOCATION_COUNT) {
		s_sameLocationCounter = 0;
		s_haveTouch = 0;
		Screen_ShowMainScreen();
		return;
	}

	// Copy over values:
	s_lastTouchX = x;
	s_lastTouchY = y;

	// Draw it
	showTouches_Draw(pThis, 1);
}

static void showTouches_Draw(GL_PageControls_TypeDef* pThis, _Bool force)
{
	if (!force) {
		return;
	}

	// Clear screen
	GL_Clear(LCD_COLOR_YELLOW);

	// Draw title & directions
	GL_SetFont(GL_FONTOPTION_16x24);
	GL_SetTextColor(LCD_COLOR_NAVY);
	GL_PrintString(10, TITLE_TOP, "Check Calibration", 1);
	GL_SetTextColor(LCD_COLOR_BLACK);
	GL_SetFont(GL_FONTOPTION_8x12Bold);
	GL_PrintString(10, DIRECTIONS_TOP,      "Tap screen to test touch-screen", 1);
	GL_PrintString(10, DIRECTIONS_TOP + 12, "calibration and display marker.", 1);
	GL_SetTextColor(LCD_COLOR_BLUE);
	GL_PrintString(10, DIRECTIONS_TOP + 30, "Tap same location 3 times to quit.", 1);

	// Do we have a value to display
	if (s_haveTouch) {
		GL_PrintString(30, DISPLAY_TOP, "Coordinates read:", 1);
		char buf[15];
		sprintf(buf, "X Coord: %3d", s_lastTouchX);
		GL_PrintString(30, DISPLAY_TOP + 12, buf, 1);
		sprintf(buf, "Y Coord: %3d", s_lastTouchY);
		GL_PrintString(30, DISPLAY_TOP + 24, buf, 1);

		// Draw cross hairs
		GL_SetTextColor(LCD_COLOR_NAVY);
		GL_DrawLine(s_lastTouchX - TOUCH_POINT_RADIUS, s_lastTouchY, TOUCH_POINT_RADIUS * 2, LCD_WriteRAMDir_Right);
		GL_DrawLine(s_lastTouchX, s_lastTouchY - TOUCH_POINT_RADIUS, TOUCH_POINT_RADIUS * 2, LCD_WriteRAMDir_Down);
	}
}

