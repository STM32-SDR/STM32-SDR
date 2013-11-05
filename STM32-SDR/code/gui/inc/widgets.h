/*
 * Header file of interface to create screen widgets
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
#ifndef _WIDGETS_H_
#define _WIDGETS_H_
/*
 * Interface to creating widgets
 */
#include "graphicObjectTypes.h"
#include "graphicObject.h"
#include "LcdHal.h"


// Display the FFT onscreen and allow user to click to select a frequency.
void Widget_AddToPage_NewFFTDisplay(uint16_t x, uint16_t y, GL_Page_TypeDef *pPage);
void Init_Waterfall(void);



/*
 * Big Button
 */
// Colours:
#define BIGBUTTON_COLOR_BORDER         LCD_COLOR_BLUE
#define BIGBUTTON_COLOR_INSIDE         LCD_COLOR_WHITE
#define BIGBUTTON_COLOR_NORMAL_TEXT    LCD_COLOR_BLACK
#define BIGBUTTON_COLOR_NORMAL_BACK    LCD_COLOR_WHITE
#define BIGBUTTON_COLOR_EDIT_TEXT      LCD_COLOR_WHITE
#define BIGBUTTON_COLOR_EDIT_BACK      LCD_COLOR_DGRAY

/**
 * Create a new big button; often called from other more specific Widget_NewBigButtonXYZ() function.
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
);


// Big Buttons:
GL_PageControls_TypeDef* Widget_NewBigButtonMode(void);
GL_PageControls_TypeDef* Widget_NewBigButtonFrequency(void);
GL_PageControls_TypeDef* Widget_NewBigButtonOptions(void);


/**
 * Create a new label
 * @param strText: Text for display.
 * @param textColor: Color of text on screen.
 * @param backColor: Color of background on screen.
 * @param font: Font to use (must be a GL_FontOption)
 * @param pUpdateHandler: Callback function executed each time the label is able to redraw.
 *        Its forceRedisplay parameter is true (1) when entire screen is being redrawn.
 *        Function should return true if the label has been changed and must be redrawn.
 */
GL_PageControls_TypeDef* Widget_NewLabel(
		const char* strText,
		uint16_t textColor, uint16_t backColor, _Bool isTransparent,
		GL_FontOption font,
		_Bool (*pUpdateHandler)(GL_PageControls_TypeDef* pThis, _Bool forceRedisplay)
	);
void Widget_ChangeLabelText(GL_PageControls_TypeDef *pThis, const char* strText);
void Widget_ChangeLabelColour(GL_PageControls_TypeDef *pThis, uint16_t newTextColour);


GL_PageControls_TypeDef* Widget_NewPSKTextDisplay(void);


/*******************************************
 * General Shared Routines
 *******************************************/
// Write a signed integer to a right-justified string.
void intToCommaString(int16_t number, char *pDest, int numChar);


#endif
