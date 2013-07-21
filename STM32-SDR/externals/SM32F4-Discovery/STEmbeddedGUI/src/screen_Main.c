// Code for the main (first) screen
#include "screen_All.h"
#include "widgets.h"
#include "ModeSelect.h"
#include "options.h"
#include <assert.h>

// Used in this file to refer to the correct screen (helps to keep code copy-paste friendly.
static GL_Page_TypeDef *s_pThisScreen = &g_screenMain;

/**
 * Call-back prototypes
 */

static void tx_Click(GL_PageControls_TypeDef* pThis);
static void rx_Click(GL_PageControls_TypeDef* pThis);
static void calibrate_Click(GL_PageControls_TypeDef* pThis);


//static void updateModeLabel(void);

#define ID_RxTx_LABEL 104

/**
 * Create the screen
 */
void ScreenMain_Create(void)
{
	Create_PageObj(s_pThisScreen);

	/*
	 * Create the UI widgets
	 */
	// PSK:
	GL_PageControls_TypeDef* ctrlPskText = Widget_NewPSKTextDisplay();

	GL_PageControls_TypeDef* btnCalibrate = NewButton(13, "Cal. TS", calibrate_Click);

	GL_PageControls_TypeDef* btnRx  = NewButton(10, " Rx ", rx_Click);
	GL_PageControls_TypeDef* btnTx  = NewButton(9,  " Tx ", tx_Click);

	GL_PageControls_TypeDef* btnMode = Widget_NewBigButtonMode();
	GL_PageControls_TypeDef* btnOptions = Widget_NewBigButtonOptions();
	GL_PageControls_TypeDef* btnFreq = Widget_NewBigButtonFrequency();



	/*
	 * Place the widgets onto the screen
	 */
	// FFT
	Widget_AddToPage_NewFFTDisplay(80, 0, s_pThisScreen);
	AddPageControlObj(0,  80, ctrlPskText, s_pThisScreen);

	AddPageControlObj(0,   160, btnCalibrate, s_pThisScreen);
	AddPageControlObj(225, 160, btnRx, s_pThisScreen);
	AddPageControlObj(270, 160, btnTx, s_pThisScreen);

	AddPageControlObj(0, 0, btnMode, s_pThisScreen);


	// Bottom row of buttons
	AddPageControlObj(0, LCD_HEIGHT - 42, btnOptions, s_pThisScreen);
	AddPageControlObj(
			LCD_WIDTH - ((GL_Custom_TypeDef*)(btnFreq->objPTR))->GetWidth(btnFreq),
			LCD_HEIGHT - ((GL_Custom_TypeDef*)(btnFreq->objPTR))->GetHeight(btnFreq),
			btnFreq, s_pThisScreen);
}


/**
 * Button callbacks
 */
#include "ChangeOver.h"
static void tx_Click(GL_PageControls_TypeDef* pThis)
{
	switch (Mode_GetCurrentMode()) {
	case MODE_SSB:
		Xmit_SSB_Sequence();
		break;
	case MODE_CW:
		Xmit_CW_Sequence();
		break;
	case MODE_PSK:
		Xmit_PSK_Sequence();
		break;
	}  // End of switch
}
static void rx_Click(GL_PageControls_TypeDef* pThis)
{
	Receive_Sequence();
}

static void calibrate_Click(GL_PageControls_TypeDef* pThis) {
	Screen_ShowScreen(&g_screenCalibrate);
}

