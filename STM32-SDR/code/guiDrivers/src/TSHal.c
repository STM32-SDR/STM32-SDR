/*
 * Code for Touch screen HAL
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


#include "TSHal.h"
#include "TSDriver_ADS7843.h"
#include <assert.h>
#include "eeprom.h"
#include "Encoders.h"
#include "xprintf.h"


// Calibration points:
#define TOUCHPOINT_TOP           (LCD_HEIGHT / 5)
#define TOUCHPOINT_BOTTOM        (4 * (LCD_HEIGHT / 5))
#define TOUCHPOINT_LEFT          (LCD_WIDTH / 5)
#define TOUCHPOINT_RIGHT         (4 * (LCD_WIDTH / 5))

#define TOUCHPOINT_CENTERX       (LCD_WIDTH / 2)
#define TOUCHPOINT_CENTERY       (LCD_HEIGHT / 2)

// Note: order is specific to calibration calculation below.
static const CalibrationPoint s_touchTargets[] = {
		{ TOUCHPOINT_CENTERX, TOUCHPOINT_CENTERY},
		{ TOUCHPOINT_LEFT,    TOUCHPOINT_TOP},
		{ TOUCHPOINT_RIGHT,   TOUCHPOINT_TOP},
		{ TOUCHPOINT_RIGHT,   TOUCHPOINT_BOTTOM},
		{ TOUCHPOINT_LEFT,    TOUCHPOINT_BOTTOM},
};


// Static variables
static _Bool s_isCalibrated = 0;

// Calibration Constants
#define RESCALE_FACTOR 1000000
static int32_t A2 = 0, B2 = 0, C2 = 0, D2 = 0, E2 = 0, F2 = 0;


// Local functions:
static void TS_CalculateCalibration(CalibrationPoint touchedPoints[TS_NUM_CALIBRATION_POINTS]);
static uint16_t getDisplayCoordinateY(uint16_t x_touch, uint16_t y_touch);
static uint16_t getDisplayCoordinateX(uint16_t x_touch, uint16_t y_touch);

/*
 * Initialization
 */
void TS_Initialize(void)
{
	TSDriver_Initialize();

	// Load from EEPROM if valid.
	// If both encoders are pressed in at startup, don't load so that
	// we force a Touch Screen calibration.
	xprintf("Ecoders pressed? Option=%d, Frequency=%d. (0=no, 1=yes).\n",
			Encoders_IsOptionsEncoderPressed(),
			Encoders_IsFrequencyEncoderPressed());

	if (TS_HaveValidEEPROMData() && !Encoders_AreBothEncodersPressed()) {
		TS_ReadCalibrationFromEEPROM();
	}
}


/*
 * Normal usage functions
 */
_Bool TS_HasNewTouchEvent(void)
{
	return TSDriver_HasNewTouchEvent();
}
void  TS_GetTouchEventCoords(uint16_t *pX, uint16_t *pY)
{
	uint16_t rawX, rawY;
	TSDriver_GetRawTouchEventCoords(&rawX, &rawY);

	// Apply calibration:
	*pX = getDisplayCoordinateX(rawX, rawY);
	*pY = getDisplayCoordinateY(rawX, rawY);
}

void  TS_ClearTouchEvent(void)
{
	TSDriver_ClearRawTouchEvent();
}




/* ********************************************************
 *
 *               Calibration Functions
 *
 * ********************************************************/
_Bool TS_IsCalibrated(void)
{
	return s_isCalibrated;
}

void  TS_GetUncalibratedTouchEvent(uint16_t *pX, uint16_t *pY)
{
	TSDriver_GetRawTouchEventCoords(pX, pY);
}

void  TS_GetCalibrationTarget(int pointNumber, uint16_t *pXTarg, uint16_t *pYTarg)
{
	assert(pointNumber >= 0 && pointNumber < TS_NUM_CALIBRATION_POINTS);
	*pXTarg = s_touchTargets[pointNumber].X;
	*pYTarg = s_touchTargets[pointNumber].Y;
}

void  TS_SetCalibrationData(CalibrationPoint touchedPoints[TS_NUM_CALIBRATION_POINTS])
{
	s_isCalibrated = 1;
	TS_CalculateCalibration(touchedPoints);
	TS_WriteCalibrationToEEPROM();
}




/**
 * Copyright from touchscreen.h:
  ******************************************************************************
  * @file    touchscreen.h
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    11-July-2011
  * @brief   Touchscreen Driver
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/*
 * Compute calibration constants
 */
static void TS_CalculateCalibration(CalibrationPoint touchedPoints[TS_NUM_CALIBRATION_POINTS])
{
	uint32_t coordinate_X1 = 0, coordinate_X2 = 0, coordinate_X3 = 0, coordinate_X4 = 0, coordinate_X5 = 0;
	uint32_t coordinate_Y1 = 0, coordinate_Y2 = 0, coordinate_Y3 = 0, coordinate_Y4 = 0, coordinate_Y5 = 0;
	uint16_t Xd1 = (LCD_WIDTH / 2), Xd2 = 1 * (LCD_WIDTH / 5), Xd3 = 4 * (LCD_WIDTH / 5), Xd4 = 4 * (LCD_WIDTH / 5), Xd5 = 1 * (LCD_WIDTH / 5);
	uint16_t Yd1 = (LCD_HEIGHT / 2), Yd2 = 1 * (LCD_HEIGHT / 5), Yd3 = 1 * (LCD_HEIGHT / 5), Yd4 = 4 * (LCD_HEIGHT / 5), Yd5 = 4 * (LCD_HEIGHT / 5);
	double A = 0.0, B = 0.0, C = 0.0, D = 0.0, E = 0.0, F = 0.0;
	double d = 0.0, dx1 = 0.0, dx2 = 0.0, dx3 = 0.0, dy1 = 0.0, dy2 = 0.0, dy3 = 0.0;
	uint32_t X2_1 = 0, X2_2 = 0, X2_3 = 0, X2_4 = 0, X2_5 = 0;
	uint32_t Y2_1 = 0, Y2_2 = 0, Y2_3 = 0, Y2_4 = 0, Y2_5 = 0;
	uint32_t XxY_1 = 0, XxY_2 = 0, XxY_3 = 0, XxY_4 = 0, XxY_5 = 0;
	uint32_t XxXd_1 = 0, XxXd_2 = 0, XxXd_3 = 0, XxXd_4 = 0, XxXd_5 = 0;
	uint32_t YxXd_1 = 0, YxXd_2 = 0, YxXd_3 = 0, YxXd_4 = 0, YxXd_5 = 0;
	uint32_t XxYd_1 = 0, XxYd_2 = 0, XxYd_3 = 0, XxYd_4 = 0, XxYd_5 = 0;
	uint32_t YxYd_1 = 0, YxYd_2 = 0, YxYd_3 = 0, YxYd_4 = 0, YxYd_5 = 0;
	uint32_t alfa = 0, beta = 0, chi = 0, Kx = 0, Ky = 0, Lx = 0, Ly = 0;
	uint16_t epsilon = 0, fi = 0, Mx = 0, My = 0;

	// Place raw values into variables:
	coordinate_X1 = touchedPoints[0].X;         coordinate_Y1 = touchedPoints[0].Y;
	coordinate_X2 = touchedPoints[1].X;         coordinate_Y2 = touchedPoints[1].Y;
	coordinate_X3 = touchedPoints[2].X;         coordinate_Y3 = touchedPoints[2].Y;
	coordinate_X4 = touchedPoints[3].X;         coordinate_Y4 = touchedPoints[3].Y;
	coordinate_X5 = touchedPoints[4].X;         coordinate_Y5 = touchedPoints[4].Y;

	// Compute the scaling function.
	X2_1 = (coordinate_X1 * coordinate_X1);
	X2_2 = (coordinate_X2 * coordinate_X2);
	X2_3 = (coordinate_X3 * coordinate_X3);
	X2_4 = (coordinate_X4 * coordinate_X4);
	X2_5 = (coordinate_X5 * coordinate_X5);

	Y2_1 = (coordinate_Y1 * coordinate_Y1);
	Y2_2 = (coordinate_Y2 * coordinate_Y2);
	Y2_3 = (coordinate_Y3 * coordinate_Y3);
	Y2_4 = (coordinate_Y4 * coordinate_Y4);
	Y2_5 = (coordinate_Y5 * coordinate_Y5);

	XxY_1 = (coordinate_X1 * coordinate_Y1);
	XxY_2 = (coordinate_X2 * coordinate_Y2);
	XxY_3 = (coordinate_X3 * coordinate_Y3);
	XxY_4 = (coordinate_X4 * coordinate_Y4);
	XxY_5 = (coordinate_X5 * coordinate_Y5);

	XxXd_1 = (coordinate_X1 * Xd1);
	XxXd_2 = (coordinate_X2 * Xd2);
	XxXd_3 = (coordinate_X3 * Xd3);
	XxXd_4 = (coordinate_X4 * Xd4);
	XxXd_5 = (coordinate_X5 * Xd5);

	YxXd_1 = (coordinate_Y1 * Xd1);
	YxXd_2 = (coordinate_Y2 * Xd2);
	YxXd_3 = (coordinate_Y3 * Xd3);
	YxXd_4 = (coordinate_Y4 * Xd4);
	YxXd_5 = (coordinate_Y5 * Xd5);

	XxYd_1 = (coordinate_X1 * Yd1);
	XxYd_2 = (coordinate_X2 * Yd2);
	XxYd_3 = (coordinate_X3 * Yd3);
	XxYd_4 = (coordinate_X4 * Yd4);
	XxYd_5 = (coordinate_X5 * Yd5);

	YxYd_1 = (coordinate_Y1 * Yd1);
	YxYd_2 = (coordinate_Y2 * Yd2);
	YxYd_3 = (coordinate_Y3 * Yd3);
	YxYd_4 = (coordinate_Y4 * Yd4);
	YxYd_5 = (coordinate_Y5 * Yd5);

	alfa = X2_1 + X2_2 + X2_3 + X2_4 + X2_5;
	beta = Y2_1 + Y2_2 + Y2_3 + Y2_4 + Y2_5;
	chi = XxY_1 + XxY_2 + XxY_3 + XxY_4 + XxY_5;
	epsilon = coordinate_X1 + coordinate_X2 + coordinate_X3 + coordinate_X4 + coordinate_X5;
	fi = coordinate_Y1 + coordinate_Y2 + coordinate_Y3 + coordinate_Y4 + coordinate_Y5;
	Kx = XxXd_1 + XxXd_2 + XxXd_3 + XxXd_4 + XxXd_5;
	Ky = XxYd_1 + XxYd_2 + XxYd_3 + XxYd_4 + XxYd_5;
	Lx = YxXd_1 + YxXd_2 + YxXd_3 + YxXd_4 + YxXd_5;
	Ly = YxYd_1 + YxYd_2 + YxYd_3 + YxYd_4 + YxYd_5;
	Mx = Xd1 + Xd2 + Xd3 + Xd4 + Xd5;
	My = Yd1 + Yd2 + Yd3 + Yd4 + Yd5;

	d = 5 * (((double) alfa * beta) - ((double) chi * chi)) + 2 * ((double) chi * epsilon * fi) - ((double) alfa * fi * fi) - ((double) beta * epsilon * epsilon);
	dx1 = 5 * (((double) Kx * beta) - ((double) Lx * chi)) + ((double) fi * (((double) Lx * epsilon) - ((double) Kx * fi))) + ((double) Mx * (((double) chi * fi) - ((double) beta * epsilon)));
	dx2 = 5 * (((double) Lx * alfa) - ((double) Kx * chi)) + ((double) epsilon * (((double) Kx * fi) - ((double) Lx * epsilon))) + ((double) Mx * (((double) chi * epsilon) - ((double) alfa * fi)));
	dx3 = ((double) Kx * (((double) chi * fi) - ((double) beta * epsilon))) + ((double) Lx * (((double) chi * epsilon) - ((double) alfa * fi)))
	        + ((double) Mx * (((double) alfa * beta) - ((double) chi * chi)));
	dy1 = 5 * (((double) Ky * beta) - ((double) Ly * chi)) + ((double) fi * (((double) Ly * epsilon) - ((double) Ky * fi))) + ((double) My * (((double) chi * fi) - ((double) beta * epsilon)));
	dy2 = 5 * (((double) Ly * alfa) - ((double) Ky * chi)) + ((double) epsilon * (((double) Ky * fi) - ((double) Ly * epsilon))) + ((double) My * (((double) chi * epsilon) - ((double) alfa * fi)));
	dy3 = ((double) Ky * (((double) chi * fi) - ((double) beta * epsilon))) + ((double) Ly * (((double) chi * epsilon) - ((double) alfa * fi)))
	        + ((double) My * (((double) alfa * beta) - ((double) chi * chi)));

	A = dx1 / d;
	B = dx2 / d;
	C = dx3 / d;
	D = dy1 / d;
	E = dy2 / d;
	F = dy3 / d;

	/* To avoid computation with "double" variables A, B, C, D, E, F, we use the s32 variables
	 A2, B2, C2, D2, E2, F2, multiplied for a Scale Factor equal to 100000 to retain the precision*/
	A2 = (int32_t) (A * RESCALE_FACTOR);
	B2 = (int32_t) (B * RESCALE_FACTOR);
	C2 = (int32_t) (C * RESCALE_FACTOR);
	D2 = (int32_t) (D * RESCALE_FACTOR);
	E2 = (int32_t) (E * RESCALE_FACTOR);
	F2 = (int32_t) (F * RESCALE_FACTOR);
}


// Convert the raw touch data into locations (x, y) on the screen:
static uint16_t getDisplayCoordinateX(uint16_t x_touch, uint16_t y_touch)
{
	float temp = (A2 * x_touch + B2 * y_touch + C2) / RESCALE_FACTOR;
	uint16_t Xd = (uint16_t) (temp);

	// Avoid too far off screen (over/underflow).
	// Note that touch screen sensor may extend beyond LCD.
	if (Xd >= LCD_WIDTH*2) {
		Xd = LCD_WIDTH - 1;
	}
	return Xd;
}
static uint16_t getDisplayCoordinateY(uint16_t x_touch, uint16_t y_touch)
{
	float temp = (D2 * x_touch + E2 * y_touch + F2) / RESCALE_FACTOR;
	uint16_t Yd = (uint16_t) (temp);

	// Avoid too far off screen (over/underflow).
	// Note that touch screen sensor may extend beyond LCD.
	if (Yd >= LCD_HEIGHT*2) {
		Yd = LCD_HEIGHT - 1;
	}
	return Yd;
}


/* ********************************************************
 *
 *               EEPROM Functions
 *
 * ********************************************************/
// options start at 200; choose something quite a bit higher.
#define EEPROM_OFFSET 1000
#define EEPROM_SENTINEL_LOC (EEPROM_OFFSET - 2) // 2 byte sentinel
#define EEPROM_SENTINEL_VAL 11413

_Bool TS_HaveValidEEPROMData(void)
{
	int16_t sentinel = Read_Int_EEProm(EEPROM_SENTINEL_LOC);
	return (sentinel == EEPROM_SENTINEL_VAL);
}
void TS_ReadCalibrationFromEEPROM(void)
{
	uint16_t addr = EEPROM_OFFSET;
	A2 = Read_Long_EEProm(addr);
	addr += sizeof(A2);
	B2 = Read_Long_EEProm(addr);
	addr += sizeof(A2);
	C2 = Read_Long_EEProm(addr);
	addr += sizeof(A2);
	D2 = Read_Long_EEProm(addr);
	addr += sizeof(A2);
	E2 = Read_Long_EEProm(addr);
	addr += sizeof(A2);
	F2 = Read_Long_EEProm(addr);

	s_isCalibrated = 1;
}
void TS_WriteCalibrationToEEPROM(void)
{
	uint16_t addr = EEPROM_OFFSET;
	Write_Long_EEProm(addr, A2);
	addr += sizeof(A2);
	Write_Long_EEProm(addr, B2);
	addr += sizeof(A2);
	Write_Long_EEProm(addr, C2);
	addr += sizeof(A2);
	Write_Long_EEProm(addr, D2);
	addr += sizeof(A2);
	Write_Long_EEProm(addr, E2);
	addr += sizeof(A2);
	Write_Long_EEProm(addr, F2);

	// Add Sentinel
	Write_Int_EEProm(EEPROM_SENTINEL_LOC, EEPROM_SENTINEL_VAL);
}
