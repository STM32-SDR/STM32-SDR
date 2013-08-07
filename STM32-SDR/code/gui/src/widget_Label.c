/**
 * Widget for a label on the screen which gives touch and update callbacks.
 */
#include "widgets.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>



#define WIDGET_LABEL_TEXT_MAXLENGTH 46
#define WIDGET_LABEL_TEXT_ARRAY_SIZE (WIDGET_LABEL_TEXT_MAXLENGTH +1)
typedef struct {
	// Text info for display
	char strText[WIDGET_LABEL_TEXT_ARRAY_SIZE];
	uint16_t textColor;
	uint16_t backColor;
	_Bool isTransparent;
	GL_FontOption font;

	// Callback (possibly null) for execution before label is drawn to screen.
	// forceRedraw set to true (1) indicates full page draw; false means just an update.
	// Returns true if a redraw is required.
	_Bool (*pUpdateHandler)(GL_PageControls_TypeDef* pThis, _Bool forceRedisplay);
	_Bool isRedrawRequired;	// Redraw text when changed.
} LabelData;


// Prototypes for custom control callback:
static uint16_t getWidth(GL_PageControls_TypeDef* obj);
static uint16_t getHeight(GL_PageControls_TypeDef* obj);
static void eventHandler(GL_PageControls_TypeDef* pThis);
static void drawHandler(GL_PageControls_TypeDef* obj, _Bool force);

static LabelData* getInstDataFromPageCtrl(GL_PageControls_TypeDef *pPageCtrl);

/*
 * Public Interface
 */

/**
 * Create a new widget
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
	)
{
	// Instance data for *this* button being created.
	LabelData *pInstanceData = (LabelData*) malloc(sizeof(LabelData));
	assert (pInstanceData != 0);

	// Create object to return
	GL_PageControls_TypeDef *pControl = NewCustomWidget(
				0,
				getWidth,
				getHeight,
				eventHandler,
				drawHandler,
				pInstanceData
				);

	// Store parameters
	Widget_ChangeLabelText(pControl, strText);
	pInstanceData->textColor = textColor;
	pInstanceData->backColor = backColor;
	pInstanceData->isTransparent = isTransparent;
	pInstanceData->font = font;
	pInstanceData->pUpdateHandler = pUpdateHandler;
	pInstanceData->isRedrawRequired = 0;

	return pControl;
}

void Widget_ChangeLabelText(GL_PageControls_TypeDef *pThis, const char* strText)
{
	LabelData *pInstData = getInstDataFromPageCtrl(pThis);

	// Store Text
	// Note: Buffer is 1 bigger than WIDGET_LABEL_TEXT_MAXLENGTH
	strncpy(pInstData->strText, strText, WIDGET_LABEL_TEXT_MAXLENGTH);
	pInstData->strText[WIDGET_LABEL_TEXT_MAXLENGTH] = 0;
	pInstData->isRedrawRequired = 1;
}

void Widget_ChangeLabelColour(GL_PageControls_TypeDef *pThis, uint16_t newTextColour)
{
	LabelData *pInstData = getInstDataFromPageCtrl(pThis);
	pInstData->textColor = newTextColour;
	pInstData->isRedrawRequired = 1;
}


/*
 * Private interface
 */
static uint16_t getWidth(GL_PageControls_TypeDef* pThis)
{
	LabelData *pInstData = getInstDataFromPageCtrl(pThis);
	return strlen(pInstData->strText) * GL_GetFontLetterWidth(pInstData->font);
}
static uint16_t getHeight(GL_PageControls_TypeDef* pThis)
{
	LabelData *pInstData = getInstDataFromPageCtrl(pThis);
	return GL_GetFontLetterHeight(pInstData->font);
}
static void eventHandler(GL_PageControls_TypeDef* pThis)
{
	// Nothing: no click event on a label.
}

static void drawHandler(GL_PageControls_TypeDef* pThis, _Bool force)
{
	LabelData *pInstData = getInstDataFromPageCtrl(pThis);

	// Check if a call to Widget_ChangeLabelText() has happened:
	_Bool change = pInstData->isRedrawRequired;
	pInstData->isRedrawRequired = 0;

	// Allow code to update itself (via an update handler, if any).
	if (pInstData->pUpdateHandler != 0) {
		change = pInstData->pUpdateHandler(pThis, force);
	}

	int x = pThis->objCoordinates.MinX;
	int y = pThis->objCoordinates.MinY;

	// Redraw?
	if (force || change) {
		GL_SetTextColor(pInstData->textColor);
		GL_SetBackColor(pInstData->backColor);
		GL_SetFont(pInstData->font);

		GL_PrintString(x, y, pInstData->strText, pInstData->isTransparent);
	}
}


static LabelData* getInstDataFromPageCtrl(GL_PageControls_TypeDef *pPageCtrl)
{
	assert(pPageCtrl && pPageCtrl->objPTR);
	GL_Custom_TypeDef *pCustom = (GL_Custom_TypeDef*)(pPageCtrl->objPTR);
	LabelData *pBigButtonData =(LabelData *)(pCustom->pInstanceData);
	return pBigButtonData;
}
