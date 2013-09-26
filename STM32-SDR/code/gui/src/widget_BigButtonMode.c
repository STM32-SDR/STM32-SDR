/*
 * Code for Big Button for displaying / changing the mode
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

#include "widgets.h"
#include "screen_All.h"
#include <assert.h>
#include "ModeSelect.h"
#include "DMA_IRQ_Handler.h"
#include "ChangeOver.h"

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
	int16_t curRxTx = RxTx_InTxMode();


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

		int writeX = relX + OFFSETX_TEXT;
		int writeY = relY + OFFSETY_RXTX;

		if (RxTx_InRxMode()) {
			GL_SetBackColor(LCD_COLOR_GREEN);
			GL_SetTextColor(LCD_COLOR_BLACK);
			GL_PrintString(writeX, writeY, " RX  ", 0);
		} else {
			GL_SetBackColor(LCD_COLOR_RED);
			GL_SetTextColor(LCD_COLOR_WHITE);
			GL_PrintString(writeX, writeY, " TX  ", 0);
		}
		previousRxTx = curRxTx;
	}
}

