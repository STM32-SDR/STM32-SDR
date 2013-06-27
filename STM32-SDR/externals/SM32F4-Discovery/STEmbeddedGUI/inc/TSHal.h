/**
 * Touchscreen Hardware Abstraction Interface
 * Supports:
 * - Generalized interface for reporting (calibrated) touch events to system.
 * - Back-end for touchscreen calibration.
 *
 * Requires underlying driver for accessing touchscreen hardware and reporting uncalibrated events.
 */

#include <stdint.h>
#include "LcdHal.h"

#define TS_NUM_CALIBRATION_TARGETS     5

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
// TODO: Implement this (code in touchscreen.c)
_Bool TS_IsCalibrated(void);
void  TS_GetUncalibratedTouchEvent(uint16_t *pX, uint16_t *pY);
// What should we pass in?!?
void  TS_SetCalibrationData(void);
void  TS_GetCalibrationTarget(int pointNumber, uint16_t *pXTarg, uint16_t *pYTarg);
