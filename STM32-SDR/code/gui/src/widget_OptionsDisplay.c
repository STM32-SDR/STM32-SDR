/*
 * Code for Options control widget
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
#include "options.h"
#include "xprintf.h"

#define TEXT_HEIGHT_PER_CHAR 12
#define TEXT_WIDTH_PER_CHAR  8

// Offsets for text:
#define OFFSETY_OPTION  (0)
#define OFFSETY_VALUE   (TEXT_HEIGHT_PER_CHAR + OFFSETY_OPTION)

#define OFFSETX_OPTION (0)
#define OFFSETX_VALUE  (0)

#define INSIDE_WIDTH_IN_CHARS  9
#define INSIDE_WIDTH    (TEXT_WIDTH_PER_CHAR * INSIDE_WIDTH_IN_CHARS)
#define INSIDE_HEIGHT   (2 * TEXT_HEIGHT_PER_CHAR)


// Prototypes:
static void insideEventHandler(GL_PageControls_TypeDef* pThis, int relX, int relY);
static void insideDrawHandler(GL_PageControls_TypeDef* pThis, _Bool force, int relX, int relY);



/*
 * Public Interface
 */
GL_PageControls_TypeDef* Widget_NewBigButtonOptions()
{
	GL_PageControls_TypeDef* newControl = Widget_NewBigButton(
			"Setting ",
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
	switch (Screen_GetScreenMode())
	{
		case MAIN :
			debug (GUI, "insideEventHandler: MAIN -> Screen_ShowScreen options\n");
			Screen_SetScreenMode(OPTIONS);
			Screen_ShowScreen(&g_screenOptions);
			break;
		case OPTIONS :
			debug (GUI, "insideEventHandler: OPTIONS -> Screen_Done\n");
			Screen_Done();
			break;
		case ADVANCED :
			debug (GUI, "insideEventHandler: ADVANCED -> Screen_Done\n");
			Screen_Done();
			break;
		default:
			assert(0);
	}

}
static void insideDrawHandler(GL_PageControls_TypeDef* pThis, _Bool force, int relX, int relY)
{
	// Setup impossible values (at least for the idx)
	static int lastOptIdx = -1;
	static int16_t lastOptValue = -30215; // Random-ish number

	int selOptIdx = Options_GetSelectedOption();
	int16_t value = Options_GetValue(selOptIdx);

	// Redraw only when needed:
	_Bool redrawName = force || selOptIdx != lastOptIdx;
	_Bool redrawValue = force || redrawName || lastOptValue != value;

	// Change name display
	if (redrawName) {
		GL_SetFont(GL_FONTOPTION_8x12Bold);
		GL_SetTextColor(BIGBUTTON_COLOR_NORMAL_TEXT);
		GL_SetBackColor(BIGBUTTON_COLOR_NORMAL_BACK);
		GL_PrintString(relX + OFFSETX_OPTION, relY + OFFSETY_OPTION, Options_GetName(selOptIdx), 0);
		lastOptIdx = selOptIdx;
	}

	// Change value display
	if (redrawValue) {
		#define MAX_LEN 10
		char numberStr[MAX_LEN];
		intToCommaString(value, numberStr, MAX_LEN);

		GL_SetFont(GL_FONTOPTION_8x12Bold);
		GL_SetTextColor(BIGBUTTON_COLOR_EDIT_TEXT);
		GL_SetBackColor(BIGBUTTON_COLOR_EDIT_BACK);
		GL_PrintString(relX + OFFSETX_VALUE, relY + OFFSETY_VALUE, numberStr, 0);

		lastOptValue = value;
	}
}
