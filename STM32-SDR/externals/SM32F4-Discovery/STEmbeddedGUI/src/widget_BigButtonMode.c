/**
 * Big Button for displaying/changing the mode
 */
#include "widgets.h"
#include "screen_All.h"
#include <assert.h>
#include "ModeSelect.h"
#include "DMA_IRQ_Handler.h"

#define TEXT_HEIGHT_PER_CHAR 12
#define TEXT_WIDTH_PER_CHAR  8

// Offsets for text:
#define OFFSETY_MODE  (0)
#define OFFSETY_BAND  (TEXT_HEIGHT_PER_CHAR + OFFSETY_MODE)

#define OFFSETX_TEXT  (0)

#define INSIDE_WIDTH_IN_CHARS  5
#define INSIDE_WIDTH    (TEXT_WIDTH_PER_CHAR * INSIDE_WIDTH_IN_CHARS)
#define INSIDE_HEIGHT   (2 * TEXT_HEIGHT_PER_CHAR)

// Prototypes:
static void insideEventHandler(GL_PageControls_TypeDef* pThis, int relX, int relY);
static void insideDrawHandler(GL_PageControls_TypeDef* pThis, _Bool force, int relX, int relY);

/*
 * Public Interface
 */

/*
 * Create a big button for the mode.
 */
GL_PageControls_TypeDef* Widget_NewBigButtonMode(void)
{
	GL_PageControls_TypeDef* newControl = Widget_NewBigButton(
			"MODE",
			INSIDE_WIDTH, INSIDE_HEIGHT,
			insideEventHandler,
			insideDrawHandler
		);

	return newControl;
}


/*
 * Private interface
 */
static void insideEventHandler(GL_PageControls_TypeDef* pThis, int relX, int relY)
{
	// TODO: Change mode button to display mode-select screen (when created)
	Screen_ShowScreen(&g_screenCalibrationTest);
}
static void insideDrawHandler(GL_PageControls_TypeDef* pThis, _Bool force, int relX, int relY)
{
	// Setup impossible values (at least for the idx)
	static uint16_t previousMode = -1;
	static float previousBand = -1;

	// TODO: Improve modular interface for band and mode.
	uint16_t curMode = Mode;
	float curBand = rgain;

	// Redraw only when needed:
	_Bool redrawMode = force || curMode != previousMode;
	_Bool redrawBand = force || curBand != previousBand;

	// Draw the mode
	if (redrawMode) {
		GL_SetFont(GL_FONTOPTION_8x12Bold);
		GL_SetTextColor(BIGBUTTON_COLOR_NORMAL_TEXT);
		GL_SetBackColor(BIGBUTTON_COLOR_NORMAL_BACK);

		int writeX = relX + OFFSETX_TEXT;
		int writeY = relY + OFFSETY_MODE;

		switch (Mode) {
		case MODE_SSB:  GL_PrintString(writeX, writeY, " SSB ", 0); break;
		case MODE_CW:   GL_PrintString(writeX, writeY, " CW  ", 0);  break;
		case MODE_PSK:  GL_PrintString(writeX, writeY, " PSK ", 0); break;
		default:        GL_PrintString(writeX, writeY, " ERR ", 0); break;
		}

		previousMode = curMode;
	}


	// Draw the band
	if (redrawBand) {
		GL_SetFont(GL_FONTOPTION_8x12Bold);
		GL_SetTextColor(BIGBUTTON_COLOR_NORMAL_TEXT);
		GL_SetBackColor(BIGBUTTON_COLOR_NORMAL_BACK);

		int writeX = relX + OFFSETX_TEXT;
		int writeY = relY + OFFSETY_BAND;

		if (rgain < 0) {
			GL_PrintString(writeX, writeY, " LSB ", 0);
		} else if (rgain > 0) {
			GL_PrintString(writeX, writeY, " USB ", 0);
		} else {
			GL_PrintString(writeX, writeY, " ERR ", 0);
		}

		previousBand = curBand;
	}
}

