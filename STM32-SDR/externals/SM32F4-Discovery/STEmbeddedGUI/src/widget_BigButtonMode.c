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
#define OFFSETY_RXTX  (TEXT_HEIGHT_PER_CHAR + OFFSETY_MODE)

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
// Create a big button for the mode.
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
	Screen_ShowScreen(&g_screenMode);
}
static void insideDrawHandler(GL_PageControls_TypeDef* pThis, _Bool force, int relX, int relY)
{
	// Setup impossible values (at least for the idx)
	static UserModeType previousUserMode = -1;
	static int16_t previousRxTx = -1;

	UserModeType curUserMode = Mode_GetCurrentUserMode();
	// TODO: Improve modular interface for tx/rx
	int16_t curRxTx = Tx_Flag;


	// Redraw only when needed:
	_Bool redrawUserMode = force || curUserMode != previousUserMode;
	_Bool redrawRxTx = force || curRxTx != previousRxTx;

	// Draw the user mode
	if (redrawUserMode) {
		GL_SetFont(GL_FONTOPTION_8x12Bold);
		GL_SetTextColor(BIGBUTTON_COLOR_NORMAL_TEXT);
		GL_SetBackColor(BIGBUTTON_COLOR_NORMAL_BACK);

		int writeX = relX + OFFSETX_TEXT;
		int writeY = relY + OFFSETY_MODE;

		GL_PrintString(writeX, writeY, Mode_GetCurrentUserModeName(), 0);

		previousUserMode = curUserMode;
	}


	// Draw Rx/Tx
	if (redrawRxTx) {
		GL_SetFont(GL_FONTOPTION_8x12Bold);
		GL_SetBackColor(BIGBUTTON_COLOR_NORMAL_BACK);

		int writeX = relX + OFFSETX_TEXT;
		int writeY = relY + OFFSETY_RXTX;

		if (Tx_Flag == 0) {
			GL_SetTextColor(LCD_COLOR_DGREEN);
			GL_PrintString(writeX, writeY, " Rx  ", 0);
		} else {
			GL_SetTextColor(LCD_COLOR_RED);
			GL_PrintString(writeX, writeY, " Tx  ", 0);
		}
		previousRxTx = curRxTx;
	}
}

