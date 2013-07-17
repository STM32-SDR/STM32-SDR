/**
 * Options control widget
 */
#include "widgets.h"
#include <assert.h>
#include "options.h"

// Colours:
#define COLOR_EDGE      LCD_COLOR_BLUE
#define COLOR_MAIN      LCD_COLOR_WHITE
#define COLOR_NAME_BACK LCD_COLOR_BLACK
#define COLOR_EDIT_TEXT LCD_COLOR_WHITE
#define COLOR_EDIT_BACK LCD_COLOR_DGRAY

// Pick some (random)
#define ID_LABEL_OPTION 52200
#define ID_LABEL_VALUE  52201


#define BORDER_WIDTH         3
#define TEXT_TITLE_HEIGHT_PER_CHAR 12
#define TEXT_HEIGHT_PER_CHAR 12
#define TEXT_WIDTH_PER_CHAR  8
#define TEXT_WDITH_NUM_CHAR  9

// Offsets for text:
#define OFFSETY_TITLE   BORDER_WIDTH
#define OFFSETY_OPTION  (TEXT_TITLE_HEIGHT_PER_CHAR + OFFSETY_TITLE)
#define OFFSETY_VALUE   (TEXT_HEIGHT_PER_CHAR + OFFSETY_OPTION)

#define OFFSETX_TITLE  (BORDER_WIDTH + TEXT_WIDTH_PER_CHAR)
#define OFFSETX_OPTION BORDER_WIDTH
#define OFFSETX_VALUE  BORDER_WIDTH

#define TOTAL_WIDTH    (2*BORDER_WIDTH + TEXT_WIDTH_PER_CHAR * TEXT_WDITH_NUM_CHAR)
#define TOTAL_HEIGHT   (2*BORDER_WIDTH + TEXT_TITLE_HEIGHT_PER_CHAR + 2 * TEXT_HEIGHT_PER_CHAR)

// Prototypes:
static uint16_t getWidth(GL_PageControls_TypeDef* obj);
static uint16_t getHeight(GL_PageControls_TypeDef* obj);
static void eventHandler(GL_PageControls_TypeDef* obj);
static void drawHandler(GL_PageControls_TypeDef* obj, _Bool force);

// TODO: Make widgets not require to know the screen because this saves it to the file...
static GL_Page_TypeDef *s_pThisScreen;


/*
 * Public Interface
 */

/*
 * Create a new widget and add it to the passed in page
 * Note: May create multiple controls (labels, buttons, ...) and add them to the page.
 */
void Widget_AddToPage_NewOptionsDisplay(uint16_t x, uint16_t y, GL_Page_TypeDef *pPage)
{
	s_pThisScreen = pPage;
	GL_PageControls_TypeDef* newControl = NewCustomWidget(
				0,
				getWidth,
				getHeight,
				eventHandler,
				drawHandler,
				0);
	AddPageControlObj(x, y, newControl, pPage);


//	// Add text
//	GL_PageControls_TypeDef* lblTitle  = NewLabel(0, "OPTIONS", GL_HORIZONTAL, GL_FONTOPTION_8x12Bold, LCD_COLOR_WHITE);
//	GL_PageControls_TypeDef* lblOption = NewLabel(ID_LABEL_OPTION, "Opt Name", GL_HORIZONTAL, GL_FONTOPTION_8x16, LCD_COLOR_WHITE);
//	GL_PageControls_TypeDef* lblValue  = NewLabel(ID_LABEL_VALUE, "-----",  GL_HORIZONTAL, GL_FONTOPTION_8x16, LCD_COLOR_RED);
//
//	AddPageControlObj(x+10, y + OFFSET_TITLE,  lblTitle, pPage);
//	AddPageControlObj(x, y + OFFSET_OPTION, lblOption, pPage);
//	AddPageControlObj(x, y + OFFSET_VALUE,  lblValue, pPage);
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
	// Setup impossible values (at least for the idx)
	static int lastOptIdx = -1;
	static int16_t lastOptValue = -30215;

	int selOptIdx = Options_GetSelectedOption();
	int16_t value = Options_GetValue(selOptIdx);

	// Redraw only when needed:
	_Bool redrawTitle = force;
	_Bool redrawName = force || selOptIdx != lastOptIdx;
	_Bool redrawValue = force || redrawName || lastOptValue != value;

	int x = pThis->objCoordinates.MinX;
	int y = pThis->objCoordinates.MinY;

	// Display title:
	if (redrawTitle) {
		// Box:
		GL_SetTextColor(COLOR_EDGE);
		GL_LCD_DrawFilledRect(x, y, getHeight(pThis), getWidth(pThis));

		// Title
		GL_SetFont(GL_FONTOPTION_8x12Bold);
		GL_SetTextColor(LCD_COLOR_WHITE);
		GL_PrintString(x + OFFSETX_TITLE, y + OFFSETY_TITLE, "OPTIONS", 1);
	}

	// Change name display
	if (redrawName) {
		GL_SetFont(GL_FONTOPTION_8x12Bold);
		GL_SetTextColor(COLOR_NAME_BACK);
		GL_SetBackColor(COLOR_MAIN);
		GL_PrintString(x + OFFSETX_OPTION, y + OFFSETY_OPTION, Options_GetName(selOptIdx), 0);
		lastOptIdx = selOptIdx;
	}

	// Change value display
	if (redrawValue) {
		#define MAX_LEN 10
		char numberStr[MAX_LEN];
		intToCommaString(value, numberStr, MAX_LEN);

		GL_SetFont(GL_FONTOPTION_8x12Bold);
		GL_SetTextColor(COLOR_EDIT_TEXT);
		GL_SetBackColor(COLOR_EDIT_BACK);
		GL_PrintString(x + OFFSETX_VALUE, y + OFFSETY_VALUE, numberStr, 0);

		lastOptValue = value;
	}
}
