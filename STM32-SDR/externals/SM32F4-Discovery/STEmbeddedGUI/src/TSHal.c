#include "TSHal.h"
#include "TSDriver_ADS7843.h"

/*
 * Initialization
 */
void TS_Initialize(void)
{
	TSDriver_Initialize();
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
	// TODO: Make calibration non-hard-coded
	*pX = (uint16_t) ((float) (3873 - rawX) / 11.11);
	*pY = (uint16_t) (((float) (rawY - 212) / 15.13));
}

void  TS_ClearTouchEvent(void)
{
	TSDriver_ClearRawTouchEvent();
}
