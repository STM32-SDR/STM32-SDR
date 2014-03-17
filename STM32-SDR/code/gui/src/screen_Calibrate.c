/*
 * Code for the calibration screen
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
#include "TSHal.h"
#include <assert.h>
#include "xprintf.h"

#define TOUCH_POINT_RADIUS       20
#define TITLE_TOP                (65)
#define DIRECTIONS_TOP           (130 + 20)
#define DONE_TOP                 (DIRECTIONS_TOP + 15)

// Used in this file to refer to the correct screen (helps to keep code copy-paste friendly.
static GL_Page_TypeDef *s_pThisScreen = &g_screenCalibrate;

// Calibration tracking variables for UI interaction
static int currentTouchPoint = 0;
static CalibrationPoint s_touchedReadings[TS_NUM_CALIBRATION_POINTS];


/**
 * Call-back prototypes
 */
static uint16_t calibration_GetWidth(GL_PageControls_TypeDef* pThis);
static uint16_t calibration_GetHeight(GL_PageControls_TypeDef* pThis);
static void calibration_Click(GL_PageControls_TypeDef* pThis);
static void calibration_Draw(GL_PageControls_TypeDef* pThis, _Bool force);


/**
 * Create the screen
 */
void ScreenCalibrate_Create(void)
{
	Create_PageObj(s_pThisScreen);

	GL_PageControls_TypeDef* calibrationWidget = NewCustomWidget(1, calibration_GetWidth, calibration_GetHeight, calibration_Click, calibration_Draw, 0);
	AddPageControlObj(0, 0, calibrationWidget, s_pThisScreen);
}


/**
 * UI Callbacks
 */
// Return max u-int because we want the touchscreen code to find *any* point inside this widget.
static uint16_t calibration_GetWidth(GL_PageControls_TypeDef* pThis)
{
	return UINT16_MAX;
}
static uint16_t calibration_GetHeight(GL_PageControls_TypeDef* pThis)
{
	return UINT16_MAX;
}

static void calibration_Click(GL_PageControls_TypeDef* pThis)
{
	// Done?
	if (currentTouchPoint >= TS_NUM_CALIBRATION_POINTS) {
		currentTouchPoint = 0;
		Screen_ShowScreen(&g_screenCalibrationTest);
		return;
	}

	// Get raw touched location
	uint16_t x, y;
	TS_GetUncalibratedTouchEvent(&x, &y);

	debug(CAL, "Touch screen calibration event: Uncalibrated touch at X=%6d, Y=%6d.\n", x, y);

	// Store touch:
	assert(currentTouchPoint >= 0);
	assert(currentTouchPoint < TS_NUM_CALIBRATION_POINTS);
	s_touchedReadings[currentTouchPoint].X = x;
	s_touchedReadings[currentTouchPoint].Y = y;

	// Move on:
	currentTouchPoint++;

	// Calibrate if that was the last point:
	if (currentTouchPoint >= TS_NUM_CALIBRATION_POINTS) {
		TS_SetCalibrationData(s_touchedReadings);

		// Advance to next screen immediately.
		// (Comment this out to show a "done" message after calibration).
		currentTouchPoint = 0;
		Screen_ShowScreen(&g_screenCalibrationTest);
		return;
	}

	calibration_Draw(pThis, 1);
}

static void calibration_Draw(GL_PageControls_TypeDef* pThis, _Bool force)
{
	if (!force) {
		return;
	}

	// Clear screen
	GL_Clear(LCD_COLOR_GREY);

	// Draw title & directions
	GL_SetFont(GL_FONTOPTION_16x24);
	GL_SetTextColor(LCD_COLOR_NAVY);
	GL_PrintString(10, TITLE_TOP, "Calibration Screen", 1);
	GL_SetFont(GL_FONTOPTION_8x12Bold);

	// Are we still getting user input?
	if (currentTouchPoint < TS_NUM_CALIBRATION_POINTS) {
		GL_PrintString(40, DIRECTIONS_TOP, "Tap the center of the + ...", 1);

		// Draw cross hairs
		uint16_t x, y;
		TS_GetCalibrationTarget(currentTouchPoint, &x, &y);
		GL_SetTextColor(LCD_COLOR_NAVY);
		GL_DrawLine(x - TOUCH_POINT_RADIUS, y, TOUCH_POINT_RADIUS * 2, LCD_WriteRAMDir_Right);
		GL_DrawLine(x, y - TOUCH_POINT_RADIUS, TOUCH_POINT_RADIUS * 2, LCD_WriteRAMDir_Down);
	}
	else {
		// Draw finished directions
		// (Unused if advancing to other screen immediately)
		GL_PrintString(40, DIRECTIONS_TOP, "Calibration complete.", 1);
		GL_PrintString(30, DONE_TOP, "Tap screen to continue", 1);
	}
}

