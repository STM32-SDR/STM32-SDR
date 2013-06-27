/*
 * Touchscreen integration with application not using graphics framework.
 * New files:
 * - TSDriver_ADS7843.h/.c: Low level driver for reading raw values from touchscreen
 * - TSHal.h/.c: Abstract interface for handling calibration of the touchscreen readings
 */

void Old_HandleTouchEvent(void);

// Note: Also has ISR in .c file.
