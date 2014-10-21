/*
 * Code for Big Button for displaying/changing frequency
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
#include "FrequencyManager.h"
#include "xprintf.h"

extern void Screen_FreqDone(void);

#define TEXT_HEIGHT_PER_CHAR 12
#define TEXT_WIDTH_PER_CHAR  8

// Offsets for text:
#define OFFSETY_BAND  (0)
#define OFFSETY_FREQ  (TEXT_HEIGHT_PER_CHAR + OFFSETY_BAND)

#define OFFSETX_TEXT  (0)

#define INSIDE_WIDTH_IN_CHARS  10
#define INSIDE_WIDTH    (TEXT_WIDTH_PER_CHAR * INSIDE_WIDTH_IN_CHARS)
#define INSIDE_HEIGHT   (2 * TEXT_HEIGHT_PER_CHAR)

// Prototypes:
static void insideEventHandler(GL_PageControls_TypeDef* pThis, int relX, int relY);
static void insideDrawHandler(GL_PageControls_TypeDef* pThis, _Bool force, int relX, int relY);
static void displayFrequency(uint32_t number, uint8_t x, uint8_t y, uint32_t changeRate);

/*
 * Public Interface
 */
GL_PageControls_TypeDef* Widget_NewBigButtonFrequency(void)
{
	GL_PageControls_TypeDef* newControl = Widget_NewBigButton(
			"Frequency",
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
	debug(GUI, "Widget_NewBigButtonFrequency:insideEventHandler:\n");
	_Bool touchLeftHalf = relX < INSIDE_WIDTH / 2;
	_Bool touchLeftQuarter = relX < INSIDE_WIDTH / 4;
	_Bool touchRightQuarter = relX > INSIDE_WIDTH * 3 / 4;
	switch (Screen_GetScreenMode()){
		case FILTER:
			if (touchLeftHalf) {
				FrequencyManager_IncreaseFreqStepSize();
			}
			else {
				FrequencyManager_DecreaseFreqStepSize();
			}
			break;

		case FREQUENCY:
		case FREQEDIT:
			if (touchLeftQuarter) {
				FrequencyManager_IncreaseFreqStepSize();
			} else if (touchRightQuarter){
				FrequencyManager_DecreaseFreqStepSize();
			} else {
				Screen_FreqDone();
				Screen_PSK_SetTune();
			}
			break;

		default:
			if (touchLeftQuarter) {
				FrequencyManager_IncreaseFreqStepSize();
			} else if (touchRightQuarter){
				FrequencyManager_DecreaseFreqStepSize();
			} else {
				Screen_SetScreenMode(FREQUENCY);
				Screen_ShowScreen(&g_screenFrequencies);
				Screen_PSK_SetTune(); // ensure we are in the correct tune mode
			}
			break;
	}
}

static void insideDrawHandler(GL_PageControls_TypeDef* pThis, _Bool force, int relX, int relY)
{
	// Setup impossible values (as much as possible
	static BandPreset previousBand = -1;
	static uint32_t previousFreq = -1;
	static uint32_t previousStep = -1;

	BandPreset curBand = FrequencyManager_GetSelectedBand();
	uint32_t curFreq = FrequencyManager_GetCurrentFrequency();
	uint32_t curStep = FrequencyManager_GetFrequencyStepSize();

	// Redraw only when needed:
	_Bool redrawBand = force || curBand != previousBand;
	_Bool redrawFreq = force || curFreq != previousFreq || curStep != previousStep;

	// Draw the band
	if (redrawBand) {
		GL_SetFont(GL_FONTOPTION_8x12Bold);
		GL_SetTextColor(BIGBUTTON_COLOR_NORMAL_TEXT);
		GL_SetBackColor(BIGBUTTON_COLOR_NORMAL_BACK);

		int writeX = relX + OFFSETX_TEXT;
		int writeY = relY + OFFSETY_BAND;

		GL_PrintString(writeX, writeY, FrequencyManager_DisplayBandName(FrequencyManager_GetSelectedBand()), 0);
		previousBand = curBand;
	}

	// Draw the Frequency
	if (redrawFreq) {
		GL_SetFont(GL_FONTOPTION_8x12Bold);
		GL_SetTextColor(BIGBUTTON_COLOR_EDIT_TEXT);
		GL_SetBackColor(BIGBUTTON_COLOR_EDIT_BACK);

		int writeX = relX + OFFSETX_TEXT;
		int writeY = relY + OFFSETY_FREQ;

		displayFrequency(FrequencyManager_GetCurrentFrequency(), writeX, writeY, FrequencyManager_GetFrequencyStepSize());

		previousFreq = curFreq;
		previousStep = curStep;
	}
}



/*
 * Draw the frequency on screen
 */
#define BUFF_SIZE   11
#define DECIMAL_IDX 6
static void writeNumberToBuffer(uint32_t number, char buffer[])
{
	int index = BUFF_SIZE - 2; // -2 = 0 indexed & room for \0.
	while (number > 0) {
		buffer[index--] = number % 10 + '0';
		number /= 10;

		if (index == DECIMAL_IDX)
			buffer[index--] = '.';
	}
}

// Return the number of characters to highlight in the frequency display
// based on how the frequency number will change as the dial spins.
static int numCharChangingByDial(int changeRate) {
	int colourChangeIdx = BUFF_SIZE - 2; // -2 = 0 indexed & room for \0.
	int fUnitCopy = changeRate;
	while (fUnitCopy > 1) {
		fUnitCopy /= 10;
		colourChangeIdx--;
	}
	if (colourChangeIdx > DECIMAL_IDX) {
		colourChangeIdx++;
	}
	return colourChangeIdx;
}

static void displayFrequency(uint32_t number, uint8_t x, uint8_t y, uint32_t changeRate)
{
	char buffer[BUFF_SIZE] = "         0";
	writeNumberToBuffer(number, buffer);

	// Display the whole number (in black):
	LCD_SetTextColor(LCD_COLOR_BLACK);
	GL_PrintString(x, y, buffer, 0);

	// Now display the part of the number changing
	buffer[numCharChangingByDial(changeRate)] = 0;
	LCD_SetTextColor(LCD_COLOR_YELLOW);
	GL_PrintString(x, y, buffer, 0);
}
