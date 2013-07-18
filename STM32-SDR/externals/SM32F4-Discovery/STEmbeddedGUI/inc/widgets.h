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
);


GL_PageControls_TypeDef* Widget_NewPSKTextDisplay(void);

// Big Buttons:
GL_PageControls_TypeDef* Widget_NewBigButtonMode(void);
GL_PageControls_TypeDef* Widget_NewBigButtonFrequency(void);
GL_PageControls_TypeDef* Widget_NewBigButtonOptions(void);

// Write a signed integer to a right-justified string.
void intToCommaString(int16_t number, char *pDest, int numChar);

#endif
