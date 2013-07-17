/**
 * PSK Text display widget
 */
#include "widgets.h"
#include <assert.h>

#include "PSKDet.h"
#include "PSKMod.h"
#include "Keyboard_Input.h"

// Colours:

// Offsets for text:
#define TEXT_LINE_HEIGHT    (16)

#define OFFSETX_TITLE    0
#define OFFSETX_ONAIR    0
#define OFFSETX_TX       0
#define OFFSETX_KEYBOARD 0
#define OFFSETY_TITLE    (0 * TEXT_LINE_HEIGHT)
#define OFFSETY_ONAIR    (1 * TEXT_LINE_HEIGHT)
#define OFFSETY_TX       (2 * TEXT_LINE_HEIGHT)
#define OFFSETY_KEYBOARD (3 * TEXT_LINE_HEIGHT)

#define TOTAL_WIDTH    (LCD_WIDTH)
#define TOTAL_HEIGHT   (4*TEXT_LINE_HEIGHT)

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
	static uint32_t lastOnAirHash = 0;
	static uint32_t lastTxHash = 0;
	static uint32_t lastKeyboardHash = 0;

	uint32_t curOnAirHash = calculateStringHash((char*) LCD_buffer);
	uint32_t curTxHash = calculateStringHash(XmitBuffer);
	uint32_t curKeyboardHash = calculateStringHash((char*) kybd_string);

	// Redraw only when needed:
	_Bool redrawTitle = force;
	_Bool redrawOnAirBuffer = force || lastOnAirHash != curOnAirHash;
	_Bool redrawTxBuffer = force || lastTxHash != curTxHash;
	_Bool redrawKeyboardBuffer = force || lastKeyboardHash != curKeyboardHash;

	int x = pThis->objCoordinates.MinX;
	int y = pThis->objCoordinates.MinY;

	// Display title:
	if (redrawTitle) {
		// Title
		GL_SetFont(GL_FONTOPTION_8x12Bold);
		GL_SetTextColor(LCD_COLOR_YELLOW);
		GL_SetBackColor(LCD_COLOR_BLACK);
		GL_PrintString(x + OFFSETX_TITLE, y + OFFSETY_TITLE, "Rx/Tx/Keyboard PSK Data:", 0);
	}

	// Display the on-air buffer
	// (Was previously displayed in main())
	if (redrawOnAirBuffer) {
		GL_SetFont(GL_FONTOPTION_8x12Bold);
		GL_SetTextColor(LCD_COLOR_BLACK);
		GL_SetBackColor(LCD_COLOR_GREEN);
		GL_PrintString(x + OFFSETX_ONAIR, y + OFFSETY_ONAIR, (char*) LCD_buffer, 0);
		lastOnAirHash = curOnAirHash;
	}

	// Display the Queue
	if (redrawTxBuffer) {
		GL_SetFont(GL_FONTOPTION_8x12Bold);
		GL_SetTextColor(LCD_COLOR_BLACK);
		GL_SetBackColor(LCD_COLOR_YELLOW);
		GL_PrintString(x + OFFSETX_TX, y + OFFSETY_TX, XmitBuffer, 0);
		lastTxHash = curTxHash;
	}

	// Display the keyboard buffer
	if (redrawKeyboardBuffer) {
		GL_SetFont(GL_FONTOPTION_8x12Bold);
		GL_SetTextColor(LCD_COLOR_BLACK);
		GL_SetBackColor(LCD_COLOR_RED);
		GL_PrintString(x + OFFSETX_KEYBOARD, y + OFFSETY_KEYBOARD, (char*) kybd_string, 0);
		lastKeyboardHash = curKeyboardHash;
	}
}


// Compute a hash function for a string.
// TODO: Should this be changed to guarantee that all text changes result in a new hash? Can it?
static uint32_t calculateStringHash(char* str)
{
	uint32_t hash = 5371;
	for (int idx = 0; str[idx] != 0; idx++) {
		// Hash = hash * 33 + character
		hash = ((hash << 5) + hash) + str[idx];
	}

	return hash;
}




