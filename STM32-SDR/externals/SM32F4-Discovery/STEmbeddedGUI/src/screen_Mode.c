// Code for the mode selection screen (PSK, SSB, CW; USB/LSB)
#include "screen_All.h"
#include "widgets.h"
#include "ModeSelect.h"
#include "DMA_IRQ_Handler.h"
#include <assert.h>

// Used in this file to refer to the correct screen (helps to keep code copy-paste friendly.
static GL_Page_TypeDef *s_pThisScreen = &g_screenMode;

/**
 * Call-back prototypes
 */
static void modeSSB_Click(GL_PageControls_TypeDef* pThis);
static void modeCW_Click(GL_PageControls_TypeDef* pThis);
static void modePSK_Click(GL_PageControls_TypeDef* pThis);
static void lsb_Click(GL_PageControls_TypeDef* pThis);
static void usb_Click(GL_PageControls_TypeDef* pThis);
static void done_Click(GL_PageControls_TypeDef* pThis);

#define ID_CUR_MODE     104
#define ID_CUR_SIDEBAND 105

static _Bool updateCurMode(GL_PageControls_TypeDef* pThis, _Bool forceRedisplay);
static _Bool updateCurSideBand(GL_PageControls_TypeDef* pThis, _Bool forceRedisplay);

/**
 * Create the screen
 */
void ScreenMode_Create(void)
{
	Create_PageObj(s_pThisScreen);

	/*
	 * Create the UI widgets
	 */
	GL_PageControls_TypeDef* lblTitle = Widget_NewLabel("Mode & Side Band", LCD_COLOR_WHITE, LCD_COLOR_BLACK, 0, GL_FONTOPTION_16x24, 0);
	GL_PageControls_TypeDef* lblDirections = Widget_NewLabel("Tap button for desired mode & side band.", LCD_COLOR_BLUE, LCD_COLOR_BLACK, 0, GL_FONTOPTION_8x12Bold, 0);

	// Mode Buttons
	GL_PageControls_TypeDef* lblMode = Widget_NewLabel("Selected Mode:", LCD_COLOR_YELLOW, LCD_COLOR_BLACK, 0, GL_FONTOPTION_8x12Bold, 0);
	GL_PageControls_TypeDef* lblCurMode = Widget_NewLabel("ERR", LCD_COLOR_RED, LCD_COLOR_BLACK, 0, GL_FONTOPTION_8x12Bold, updateCurMode);
	GL_PageControls_TypeDef* btnMode_SSB = NewButton(5, " SSB ", modeSSB_Click);
	GL_PageControls_TypeDef* btnMode_CW  = NewButton(6, "  CW ", modeCW_Click);
	GL_PageControls_TypeDef* btnMode_PSK = NewButton(7, " PSK ", modePSK_Click);

	GL_PageControls_TypeDef* lblSideBand = Widget_NewLabel("Selected Side Band:", LCD_COLOR_YELLOW, LCD_COLOR_BLACK, 0, GL_FONTOPTION_8x12Bold, 0);
	GL_PageControls_TypeDef* lblCurSideBand = Widget_NewLabel("ERR", LCD_COLOR_RED, LCD_COLOR_BLACK, 0, GL_FONTOPTION_8x12Bold, updateCurSideBand);
	GL_PageControls_TypeDef* btnLSB = NewButton(11, " LSB ", lsb_Click);
	GL_PageControls_TypeDef* btnUSB = NewButton(12, " USB ", usb_Click);

	GL_PageControls_TypeDef* btnDone = NewButton(13, "  Done  ", done_Click);


	/*
	 * Place the widgets onto the screen
	 */
	// Title
	AddPageControlObj(0,     10, lblTitle,     s_pThisScreen);
	AddPageControlObj(0,     35, lblDirections,     s_pThisScreen);

	// Mode
	AddPageControlObj(0,    80, lblMode,     s_pThisScreen);
	AddPageControlObj(160,  80, lblCurMode,  s_pThisScreen);
	AddPageControlObj(160,  95, btnMode_SSB, s_pThisScreen);
	AddPageControlObj(215,  95, btnMode_CW,  s_pThisScreen);
	AddPageControlObj(270,  95, btnMode_PSK, s_pThisScreen);

	// Side Band
	AddPageControlObj(0,   150, lblSideBand,     s_pThisScreen);
	AddPageControlObj(160, 150, lblCurSideBand,  s_pThisScreen);
	AddPageControlObj(160, 165, btnLSB, s_pThisScreen);
	AddPageControlObj(215, 165, btnUSB, s_pThisScreen);

	AddPageControlObj(LCD_WIDTH - 75, LCD_HEIGHT - 30, btnDone, s_pThisScreen);


}


/**
 * Callbacks
 */
// Mode:
static _Bool updateCurMode(GL_PageControls_TypeDef* pThis, _Bool forceRedisplay)
{
	static int previousMode = -1;
	if (forceRedisplay | (previousMode != Mode)) {
		switch (Mode) {
		case MODE_SSB:  Widget_ChangeLabelText(pThis, "SSB "); break;
		case MODE_CW:   Widget_ChangeLabelText(pThis, "CW  ");  break;
		case MODE_PSK:  Widget_ChangeLabelText(pThis, "PSK "); break;
		default:        Widget_ChangeLabelText(pThis, "ERR "); break;
		}
		previousMode = Mode;
		return 1;
	}
	return 0;
}
static void modeSSB_Click(GL_PageControls_TypeDef* pThis) {
	Mode = MODE_SSB;
}
static void modeCW_Click(GL_PageControls_TypeDef* pThis) {
	Mode = MODE_CW;
}
static void modePSK_Click(GL_PageControls_TypeDef* pThis) {
	Mode = MODE_PSK;
}


// Side Band
static _Bool updateCurSideBand(GL_PageControls_TypeDef* pThis, _Bool forceRedisplay)
{
	static float previousSideBand = -1;
	if (forceRedisplay | (previousSideBand != rgain)) {
		if (rgain < 0) {
			Widget_ChangeLabelText(pThis, "LSB");
		} else if (rgain > 0) {
			Widget_ChangeLabelText(pThis, "USB");
		} else {
			Widget_ChangeLabelText(pThis, "ERR");
		}
		previousSideBand = rgain;
		return 1;
	}
	return 0;
}
#include "DMA_IRQ_Handler.h"
static void lsb_Click(GL_PageControls_TypeDef* pThis) {
	rgain = -0.5;
}
static void usb_Click(GL_PageControls_TypeDef* pThis) {
	rgain = 0.5;
}

static void done_Click(GL_PageControls_TypeDef* pThis) {
	Screen_ShowScreen(&g_screenMain);
}


