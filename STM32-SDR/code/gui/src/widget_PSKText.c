/*
 * Code for PSK Text display widget
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
#include "ScrollingTextBox.h"
#include <assert.h>

#include "PSKDet.h"
#include "PSKMod.h"
#include "Keyboard_Input.h"
#include "Text_Enter.h"

// Colours:

// Offsets for text:
#define FONT_TITLE GL_FONTOPTION_8x16
#define FONT_DATA  GL_FONTOPTION_8x16
#define TEXT_LINE_HEIGHT    (17)

#define TITLE_COLOUR     LCD_COLOR_YELLOW
#define TITLE_BACKGROUND LCD_COLOR_BLACK
#define DATA_COLOUR      LCD_COLOR_BLACK
#define DATA_BACKGROUND  LCD_COLOR_WHITE

#define OFFSETX_TITLE    0
#define OFFSETX_ONAIR    0
#define OFFSETX_TX       0
#define OFFSETX_KEYBOARD 0
#define OFFSETY_TITLE    (0 * TEXT_LINE_HEIGHT)
#define OFFSETY_ONAIR    (1 * TEXT_LINE_HEIGHT)
#define OFFSETY_TX       (2 * TEXT_LINE_HEIGHT)
#define OFFSETY_KEYBOARD (3 * TEXT_LINE_HEIGHT)

#define TOTAL_WIDTH    (LCD_WIDTH)
#define TOTAL_HEIGHT   (5*TEXT_LINE_HEIGHT)


// Prototypes:
static uint16_t getWidth(GL_PageControls_TypeDef* obj);
static uint16_t getHeight(GL_PageControls_TypeDef* obj);
static void eventHandler(GL_PageControls_TypeDef* obj);
static void drawHandler(GL_PageControls_TypeDef* obj, _Bool force);

static uint32_t calculateStringHash(char* str);

/*
 * Public Interface
 */
GL_PageControls_TypeDef* Widget_NewPSKTextDisplay(void)
{
	GL_PageControls_TypeDef* newControl = NewCustomWidget(
				0,
				getWidth,
				getHeight,
				eventHandler,
				drawHandler,
				0);
	return newControl;
}

/*
 * Private interface
 */
static uint16_t getWidth(GL_PageControls_TypeDef* pThis)
{
	return TOTAL_WIDTH;
}
static uint16_t getHeight(GL_PageControls_TypeDef* pThis)
{
	return TOTAL_HEIGHT;
}
static void eventHandler(GL_PageControls_TypeDef* pThis)
{

}
static void drawHandler(GL_PageControls_TypeDef* pThis, _Bool force)
{
	// Setup unlikely values to start
	static uint32_t lastTxHash = 0;
	static uint32_t lastKeyboardHash = 0;
	static uint32_t lastCallHash = 0;
	static uint32_t lastNameHash = 0;
	extern unsigned char NewChar;



	uint32_t curTxHash = calculateStringHash(XmitBuffer);
	uint32_t curKeyboardHash = calculateStringHash((char*) kybd_string);
	uint32_t curCallHash = calculateStringHash(Get_Contact(0));
	uint32_t curNameHash = calculateStringHash(Get_Contact(1));

	// Redraw only when needed:
	//_Bool redrawTitle = force;
	_Bool redrawOnAirBuffer = force || NewChar != 0;
	_Bool redrawTxBuffer = force || lastTxHash != curTxHash;
	_Bool redrawKeyboardBuffer = force || lastKeyboardHash != curKeyboardHash;
	_Bool redrawCallBuffer = force || lastCallHash != curCallHash;
	_Bool redrawNameBuffer = force || lastNameHash != curNameHash;


	// Display Call
	if (redrawCallBuffer){
		GL_SetFont(GL_FONTOPTION_8x16);
		GL_SetBackColor(LCD_COLOR_BLACK);
		GL_SetTextColor(LCD_COLOR_WHITE);
		GL_PrintString(42, 175,Get_Contact(0), 0);
		lastCallHash = curCallHash;
	}

	//Display Name
	if (redrawNameBuffer) {
		GL_SetFont(GL_FONTOPTION_8x16);
		GL_SetBackColor(LCD_COLOR_BLACK);
		GL_SetTextColor(LCD_COLOR_WHITE);
		GL_PrintString(170, 175,Get_Contact(1), 0);
		lastNameHash = curNameHash;
	}

	// Display the on-air buffer
	GL_SetFont(FONT_DATA);
	GL_SetTextColor(DATA_COLOUR);
	GL_SetBackColor(DATA_BACKGROUND);
	if (redrawOnAirBuffer) {
		DisplayText (NewChar);
		NewChar = 0;
	}

	// Display the Queue
	if (redrawTxBuffer) {
		//GL_PrintString(x + OFFSETX_TX, y + OFFSETY_TX, XmitBuffer, 0);
		//lastTxHash = curTxHash;
	}

	// Display the keyboard buffer
	if (redrawKeyboardBuffer) {
		//GL_PrintString(x + OFFSETX_KEYBOARD, y + OFFSETY_KEYBOARD, (char*) kybd_string, 0);
		//lastKeyboardHash = curKeyboardHash;
	}
}


// Compute a hash function for a string.
// Note: There are more 7-digit strings than can be enumerated in 32-bits; therefore
//       it is not possible to make a function which generates unique 32-bit numbers for any given string.
static uint32_t calculateStringHash(char* str)
{
	uint32_t hash = 5371;
	for (int idx = 0; str[idx] != 0; idx++) {
		// Hash = hash * 33 + character
		hash = ((hash << 5) + hash) + str[idx];
	}

	return hash;
}



