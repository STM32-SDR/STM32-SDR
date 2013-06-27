/*
 * Low level driver for the ADS7843 touchscreen controller.
 * - Allows direct access to the touchscreen and its events.
 * - Should be used with TSHal routines for calibration.
 *   The client code (general application) should *NOT* use these functions.
 */
#include <stdint.h>


/*
 * Public functions
 */
void  TSDriver_Initialize(void);
_Bool TSDriver_HasNewTouchEvent(void);
void  TSDriver_GetRawTouchEventCoords(uint16_t *pX, uint16_t *pY);
void  TSDriver_ClearRawTouchEvent(void);

// Called from the ISR
void TSDriver_HandleTouchInterrupt(void);
