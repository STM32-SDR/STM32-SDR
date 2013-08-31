/*
 * header for Touchscreen Hardware Abstraction Interface
 * Supports:
 * - Generalized interface for reporting (calibrated) touch events to system.
 * - Back-end for touchscreen calibration.
 *
 * Requires underlying driver for accessing touchscreen hardware and reporting uncalibrated events.
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


#include <stdint.h>
#include "LcdHal.h"

#define TS_NUM_CALIBRATION_POINTS 5
typedef struct
{
	int16_t X;
	int16_t Y;
} CalibrationPoint;


/*
 * Initialization
 */
void TS_Initialize(void);

/*
 * Normal usage functions
 *
 * 1. Call TS_HasNewTouchEvent() to see if there is an event pending.
 * 2. Call TS_GetTouchEventCords() to get the (calibrated) X,Y location where the user touched.
 * 3. Call TS_ClearTouchEvent() to clear (mark as handled) the current touch event.
 */
_Bool TS_HasNewTouchEvent(void);
void  TS_GetTouchEventCoords(uint16_t *pX, uint16_t *pY);
void  TS_ClearTouchEvent(void);


/*
 * Calibration Functions
 */
_Bool TS_IsCalibrated(void);
void  TS_GetUncalibratedTouchEvent(uint16_t *pX, uint16_t *pY);
void  TS_GetCalibrationTarget(int pointNumber, uint16_t *pXTarg, uint16_t *pYTarg);
void  TS_SetCalibrationData(CalibrationPoint touchedPoints[TS_NUM_CALIBRATION_POINTS]);

/*
 * EEPROM Functions
 */
_Bool TS_HaveValidEEPROMData(void);
void TS_ReadCalibrationFromEEPROM(void);
void TS_WriteCalibrationToEEPROM(void);
