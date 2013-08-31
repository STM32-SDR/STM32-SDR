/*
 * Skeleton code for a Big Button (Multi-line) widget
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
#include <assert.h>
#include <stdlib.h>
#include "TSHal.h"

// Sizes
#define BORDER_WIDTH         3
#define TEXT_TITLE_HEIGHT_PER_CHAR 12
#define TEXT_HEIGHT_PER_CHAR 12
#define TEXT_WIDTH_PER_CHAR  8
#define TEXT_WDITH_NUM_CHAR  9

// Offsets for text:
#define OFFSETY_TITLE  BORDER_WIDTH
#define OFFSETX_TITLE  (BORDER_WIDTH + TEXT_WIDTH_PER_CHAR)


typedef struct {
	const char* strTitle;
	int insideWidth;
	int insideHegiht;
	// Touch event handler, given the coordinates relative to the *inside* of the button's area.
	void (*pEventHandler)(GL_PageControls_TypeDef* pThis, int relX, int relY);
	// Callback function for redrawing the inside of the button; given top-left corner of inside area.
	void (*pDrawHandler)(GL_PageControls_TypeDef* pThis, _Bool force, int relX, int relY);
} BigButtonData;


// Prototypes:
static uint16_t getWidth(GL_PageControls_TypeDef* obj);
static uint16_t getHeight(GL_PageControls_TypeDef* obj);
static void eventHandler(GL_PageControls_TypeDef* pThis);
static void drawHandler(GL_PageControls_TypeDef* obj, _Bool force);

static BigButtonData* getInstDataFromPageCtrl(GL_PageControls_TypeDef *pPageCtrl);


/*
 * Public Interface
 */

/**
 * Create a new widget
 * @param strTitle: The title on the big-button.
 * @param insideWidth: The size in pixels of the inside area of the big-button
 * @param insideHeight: The size in pixels of the inside area of the big-button
 * @param pEventHandler: Callback function for touch events; passed the coord relative to inside area of button.
 * @param pDrawHandler: Callback function for redrawing the inside of the button; given top-left corner of inside area.
 */
GL_PageControls_TypeDef* Widget_NewBigButton(
		const char* strTitle,
		int insideWidth, int insideHeight,
		void (*pEventHandler)(GL_PageControls_TypeDef* pThis, int relX, int relY),
		void (*pDrawHandler)(GL_PageControls_TypeDef* pThis, _Bool force, int relX, int relY)
	)
{
	// Instance data for *this* button being created.
	BigButtonData *pInstanceData = (BigButtonData*) malloc(sizeof(BigButtonData));
	pInstanceData->strTitle = strTitle;
	pInstanceData->insideHegiht = insideHeight;
	pInstanceData->insideWidth = insideWidth;
	pInstanceData->pDrawHandler = pDrawHandler;
	pInstanceData->pEventHandler = pEventHandler;

	GL_PageControls_TypeDef *pControl = NewCustomWidget(
				0,
				getWidth,
				getHeight,
				eventHandler,
				drawHandler,
				pInstanceData
				);

	return pControl;
}


/*
 * Private interface
 */
static uint16_t getWidth(GL_PageControls_TypeDef* pThis)
{
	BigButtonData *pInstData = getInstDataFromPageCtrl(pThis);
	return (2 * BORDER_WIDTH) + pInstData->insideWidth;
}
static uint16_t getHeight(GL_PageControls_TypeDef* pThis)
{
	BigButtonData *pInstData = getInstDataFromPageCtrl(pThis);
	return (2 * BORDER_WIDTH) + TEXT_TITLE_HEIGHT_PER_CHAR + pInstData->insideHegiht;
}
static void eventHandler(GL_PageControls_TypeDef* pThis)
{
	// Where was the touch?
	uint16_t touchX, touchY;
	TS_GetTouchEventCoords(&touchX, &touchY);

	// Calculate the x-y offset for the button's insides for the client code.
	int x = pThis->objCoordinates.MinX;
	int y = pThis->objCoordinates.MinY;
	int pointInsideX = touchX - x - BORDER_WIDTH;
	int pointInsideY = touchY - y - BORDER_WIDTH - TEXT_TITLE_HEIGHT_PER_CHAR;

	BigButtonData *pInstData = getInstDataFromPageCtrl(pThis);
	pInstData->pEventHandler(pThis, pointInsideX, pointInsideY);
}
static void drawHandler(GL_PageControls_TypeDef* pThis, _Bool force)
{
	BigButtonData *pInstData = getInstDataFromPageCtrl(pThis);

	int x = pThis->objCoordinates.MinX;
	int y = pThis->objCoordinates.MinY;
	int insideX = x + BORDER_WIDTH;
	int insideY = y + BORDER_WIDTH + TEXT_TITLE_HEIGHT_PER_CHAR;

	// Display title:
	if (force) {
		// Box:
		GL_SetTextColor(BIGBUTTON_COLOR_BORDER);
		GL_LCD_DrawFilledRect(x, y, getHeight(pThis), getWidth(pThis));

		// Title
		GL_SetFont(GL_FONTOPTION_8x12Bold);
		GL_SetTextColor(BIGBUTTON_COLOR_INSIDE);
		GL_PrintString(x + OFFSETX_TITLE, y + OFFSETY_TITLE, pInstData->strTitle, 1);
	}

	// Have object redraw itself:
	pInstData->pDrawHandler(pThis, force, insideX, insideY);
}


static BigButtonData* getInstDataFromPageCtrl(GL_PageControls_TypeDef *pPageCtrl)
{
	assert(pPageCtrl && pPageCtrl->objPTR);
	GL_Custom_TypeDef *pCustom = (GL_Custom_TypeDef*)(pPageCtrl->objPTR);
	BigButtonData *pBigButtonData =(BigButtonData *)(pCustom->pInstanceData);
	return pBigButtonData;
}
