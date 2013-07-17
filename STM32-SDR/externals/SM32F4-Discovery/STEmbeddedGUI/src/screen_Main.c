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

static void modeSSB_Click(void);
static void modeCW_Click(void);
static void modePSK_Click(void);
static void defaults_Click(void);
static void tx_Click(void);
static void rx_Click(void);
static void lsb_Click(void);
static void usb_Click(void);
static void calibrate_Click(void);


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
	// Mode display text
	GL_PageControls_TypeDef* lblRxTx = NewLabel(ID_RxTx_LABEL, "Rx",  GL_HORIZONTAL, GL_FONTOPTION_8x16, LCD_COLOR_BLUE);

	// Mode Buttons
	GL_PageControls_TypeDef* btnMode_SSB = NewButton(5, "SSB", modeSSB_Click);
	GL_PageControls_TypeDef* btnMode_CW  = NewButton(6, " CW", modeCW_Click);
	GL_PageControls_TypeDef* btnMode_PSK = NewButton(7, "PSK", modePSK_Click);

	// Mode buttons
	GL_PageControls_TypeDef* btnCalibrate = NewButton(13, "Cal. TS", calibrate_Click);
	GL_PageControls_TypeDef* btnDefaults = NewButton(8, "Defaults", defaults_Click);

	GL_PageControls_TypeDef* btnLSB = NewButton(11, "LSB", lsb_Click);
	GL_PageControls_TypeDef* btnUSB = NewButton(12, "USB", usb_Click);
	GL_PageControls_TypeDef* btnRx  = NewButton(10, " Rx ", rx_Click);
	GL_PageControls_TypeDef* btnTx  = NewButton(9,  " Tx ", tx_Click);


	GL_PageControls_TypeDef* btnMode = Widget_NewBigButtonMode();
	GL_PageControls_TypeDef* btnFreq = Widget_NewBigButtonFrequency();



	/*
	 * Place the widgets onto the screen
	 */
	// Mode labels
	AddPageControlObj(0,    50, lblRxTx, s_pThisScreen);

	// FFT
	Widget_AddToPage_NewFFTDisplay(80, 0, s_pThisScreen);

	// Buttons
	AddPageControlObj(100,  LCD_HEIGHT - 35, btnMode_SSB, s_pThisScreen);
	AddPageControlObj(140,  LCD_HEIGHT - 35, btnMode_CW, s_pThisScreen);
	AddPageControlObj(180,  LCD_HEIGHT - 35, btnMode_PSK, s_pThisScreen);

	AddPageControlObj(0,   150, btnCalibrate, s_pThisScreen);
	AddPageControlObj(65,  150, btnDefaults, s_pThisScreen);
	AddPageControlObj(145, 150, btnLSB, s_pThisScreen);
	AddPageControlObj(180, 150, btnUSB, s_pThisScreen);
	AddPageControlObj(225, 150, btnRx, s_pThisScreen);
	AddPageControlObj(270, 150, btnTx, s_pThisScreen);

	AddPageControlObj(0, 0, btnMode, s_pThisScreen);


	// Options
	Widget_AddToPage_NewOptionsDisplay(0, LCD_HEIGHT - 42, s_pThisScreen);
	AddPageControlObj(
			LCD_WIDTH - ((GL_Custom_TypeDef*)(btnFreq->objPTR))->GetWidth(btnFreq),
			LCD_HEIGHT - ((GL_Custom_TypeDef*)(btnFreq->objPTR))->GetHeight(btnFreq),
			btnFreq, s_pThisScreen);
}


/**
 * Button callbacks
 */
static void modeSSB_Click(void) {
	Mode = MODE_SSB;
}
static void modeCW_Click(void) {
	Mode = MODE_CW;
}
static void modePSK_Click(void) {
	Mode = MODE_PSK;
}

static void defaults_Click(void) {
	Options_WriteToEEPROM();
}

#include "ChangeOver.h"
static void tx_Click(void)
{
	switch (Mode) {
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

	Set_Label(s_pThisScreen, ID_RxTx_LABEL, "TX");
}
static void rx_Click(void)
{
	Receive_Sequence();
	//Tx_Flag=0;
	Set_Label(s_pThisScreen, ID_RxTx_LABEL, "RX");
}

#include "DMA_IRQ_Handler.h"
static void lsb_Click(void) {
	rgain = -0.5;
}
static void usb_Click(void) {
	rgain = 0.5;
}


static void calibrate_Click(void) {
	Screen_ShowScreen(&g_screenCalibrate);
}

