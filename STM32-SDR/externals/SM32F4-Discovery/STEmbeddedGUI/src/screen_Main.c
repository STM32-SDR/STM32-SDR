// Code for the main (first) screen
#include "screen_All.h"
#include "ModeSelect.h"
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


static void updateModeLabel(void);

uint16_t WidgetFFT_GetWidth(void);
uint16_t WidgetFFT_GetHeight(void);
void WidgetFFT_EventHandler(void);
void WidgetFFT_DrawHandler(_Bool force);

#define ID_BAND_LABEL 102
#define ID_MODE_LABEL 103
#define ID_RxTx_LABEL 104
#define ID_FFTSelFreqNum_LABEL 105

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
	GL_PageControls_TypeDef* lblBand = NewLabel(ID_BAND_LABEL, "USB", GL_HORIZONTAL, GL_FONTOPTION_8x16, LCD_COLOR_BLUE);
	GL_PageControls_TypeDef* lblMode = NewLabel(ID_MODE_LABEL, "SSB", GL_HORIZONTAL, GL_FONTOPTION_8x16, LCD_COLOR_BLUE);
	GL_PageControls_TypeDef* lblRxTx = NewLabel(ID_RxTx_LABEL, "Rx",  GL_HORIZONTAL, GL_FONTOPTION_8x16, LCD_COLOR_BLUE);

	// FFT
	GL_PageControls_TypeDef* fft = NewCustomWidget(1, WidgetFFT_GetWidth, WidgetFFT_GetHeight, WidgetFFT_EventHandler, WidgetFFT_DrawHandler);
	GL_PageControls_TypeDef* lblFFTSelFreqText = NewLabel(2, "Offset:", GL_HORIZONTAL, GL_FONTOPTION_8x16, LCD_COLOR_WHITE);
	GL_PageControls_TypeDef* lblFFTSelFreqNum  = NewLabel(ID_FFTSelFreqNum_LABEL, "-----",  GL_HORIZONTAL, GL_FONTOPTION_8x16, LCD_COLOR_RED);

	// Mode Buttons
	GL_PageControls_TypeDef* btnMode_SSB = NewButton(5, "SSB", modeSSB_Click);
	GL_PageControls_TypeDef* btnMode_CW  = NewButton(6, " CW", modeCW_Click);
	GL_PageControls_TypeDef* btnMode_PSK = NewButton(7, "PSK", modePSK_Click);

	// Mode buttons
	GL_PageControls_TypeDef* btnDefaults = NewButton(8, "Defaults", defaults_Click);
	GL_PageControls_TypeDef* btnTx = NewButton(9, " Tx ", tx_Click);
	GL_PageControls_TypeDef* btnRx = NewButton(10, " Rx ", rx_Click);
	GL_PageControls_TypeDef* btnLSB = NewButton(11, "LSB", lsb_Click);
	GL_PageControls_TypeDef* btnUSB = NewButton(12, "USB", usb_Click);

	GL_PageControls_TypeDef* btnCalibrate = NewButton(13, "Cal. TS", calibrate_Click);


	/*
	 * Place the widgets onto the screen
	 */
	AddPageControlObj(0,     0, lblBand, s_pThisScreen);
	AddPageControlObj(0,    12, lblMode, s_pThisScreen);
	AddPageControlObj(296,   0, lblRxTx, s_pThisScreen);

	AddPageControlObj(40,    0, fft, s_pThisScreen);
	// characters 8 pixels wide; using 5 characters for number.
	AddPageControlObj(LCD_WIDTH-(8*14), 68, lblFFTSelFreqText, s_pThisScreen);
	AddPageControlObj(LCD_WIDTH-(8*6),  68, lblFFTSelFreqNum, s_pThisScreen);

	AddPageControlObj(100,  LCD_HEIGHT - 35, btnMode_SSB, s_pThisScreen);
	AddPageControlObj(140,  LCD_HEIGHT - 35, btnMode_CW, s_pThisScreen);
	AddPageControlObj(180,  LCD_HEIGHT - 35, btnMode_PSK, s_pThisScreen);

	AddPageControlObj(0,   150, btnCalibrate, s_pThisScreen);
	AddPageControlObj(65,  150, btnDefaults, s_pThisScreen);
	AddPageControlObj(145, 150, btnLSB, s_pThisScreen);
	AddPageControlObj(180, 150, btnUSB, s_pThisScreen);
	AddPageControlObj(225, 150, btnRx, s_pThisScreen);
	AddPageControlObj(270, 150, btnTx, s_pThisScreen);
}


/**
 * Button callbacks
 */
static void modeSSB_Click(void)
{
	Mode = MODE_SSB;
	updateModeLabel();

}
static void modeCW_Click(void)
{
	Mode = MODE_CW;
	updateModeLabel();

}
static void modePSK_Click(void)
{
	Mode = MODE_PSK;
	updateModeLabel();

}
#include "Encoder_2.h"
static void defaults_Click(void)
{
	Store_Defaults();
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
static void lsb_Click(void)
{
	rgain = -0.5;
	Set_Label(s_pThisScreen, ID_BAND_LABEL, "LSB");
}
static void usb_Click(void)
{
	rgain = 0.5;
	Set_Label(s_pThisScreen, ID_BAND_LABEL, "USB");
}


static void calibrate_Click(void)
{
	Screen_ShowScreen(&g_screenCalibrate);
}


/*
 * Label Updates
 */
static void updateModeLabel(void)
{
	switch (Mode) {
	case MODE_SSB:  Set_Label(s_pThisScreen, ID_MODE_LABEL, "SSB"); break;
	case MODE_CW:   Set_Label(s_pThisScreen, ID_MODE_LABEL, "CW "); break;
	case MODE_PSK:  Set_Label(s_pThisScreen, ID_MODE_LABEL, "PSK"); break;
	default:        Set_Label(s_pThisScreen, ID_MODE_LABEL, "ERR");	break;
	}
}


/**
 * FFT Widget (Move to own file?!?)
 */
#include "arm_math.h"
#include "DSP_Processing.h"
#include "TSHal.h"
#include "TFT_Display.h"
#include "DMA_IRQ_Handler.h"

const int FFT_WIDTH   = 240;
const int FFT_HEIGHT  =  64;
const int OFFSET_X    =  40;
const int OFFSET_Y    =   0;
const int SELFREQ_ADJ =   4;

uint16_t WidgetFFT_GetWidth(void)
{
	return FFT_WIDTH;
}
uint16_t WidgetFFT_GetHeight(void)
{
	return FFT_HEIGHT;
}


// Write a signed integer to a right-justified string.
void intToCommaString(int16_t number, char *pDest, int numChar)
{
	int index = numChar - 1;
	// Null terminate, and ensure we at least put in a zero (overwrite).
	pDest[index--] = 0;
	pDest[index] = '0';

	int digitsWritten = 0;

	_Bool isNeg = 0;
	if (number < 0) {
		number = -number;
		isNeg = 1;
	}

	while (number > 0) {
		pDest[index--] = number % 10 + 0x30;
		number /= 10;

		digitsWritten++;
		if (digitsWritten > 0 && digitsWritten % 3 == 0 && number > 0)
			pDest[index--] = ',';
	}

	if (isNeg)
		pDest[index--] = '-';

	while (index >= 0) {
		pDest[index--] = ' ';
	}

	// Check that we did not overflow memory.
	assert(index >= -1);
}

void WidgetFFT_EventHandler(void)
{
	// Get the coordinates:
	uint16_t X_Point, Y_Point;
	TS_GetTouchEventCoords(&X_Point, &Y_Point);

	//Update PSK NCO Frequency
	NCO_Frequency = (double) ((float) ((X_Point - OFFSET_X) + 8) * 15.625);

	// Display location on label.
	const int MAX_LEN = 6;
	char number[MAX_LEN];
	intToCommaString((int)NCO_Frequency, number, MAX_LEN);
	Set_Label(s_pThisScreen, ID_FFTSelFreqNum_LABEL, number);

	SetRXFrequency(NCO_Frequency);
}

uint8_t FFT_Output[128];
void WidgetFFT_DrawHandler(_Bool force)
{
	// Bail if nothing to draw.
	if (!force && !DSP_Flag) {
		return;
	}

	uint8_t FFT_Display[256];


	/*
	 * Calculate the data to draw:
	 */
	// TODO: Where are all these FFT constants from?
	for (int16_t j = 0; j < 128; j++) {
		//Changed for getting right display with SR6.3
		FFT_Output[j] = (uint8_t) (6 * log((float32_t) (FFT_Magnitude[j] + 1)));

		if (FFT_Output[j] > 64)
			FFT_Output[j] = 64;
		FFT_Display[2 * j] = FFT_Output[j];
		// TODO: FFT Display calculation seems to use old data in buffer.
		FFT_Display[2 * j + 1] = (FFT_Output[j] + FFT_Output[j + 1]) / 2;
	}


	/*
	 * Display the FFT:
	 */
	float selectedFreqX = (float) (NCO_Frequency - 125) / 15.625;
	if (selectedFreqX < 0) {
		selectedFreqX = 0;
	}

	// Draw the FFT using direct memory writes (fast).
	LCD_SetDisplayWindow(OFFSET_X, OFFSET_Y, FFT_HEIGHT, FFT_WIDTH);
	LCD_WriteRAM_PrepareDir(LCD_WriteRAMDir_Down);

	for (int x = 0; x < FFT_WIDTH; x++) {
		// Plot this column of the FFT.
		for (int y = 0; y < FFT_HEIGHT; y++) {

			// Draw red line for selected frequency
			if (x == (int) selectedFreqX) {
				// Leave some white at the top
				if (y <= SELFREQ_ADJ) {
					LCD_WriteRAM(LCD_COLOR_WHITE);
				} else {
					LCD_WriteRAM(LCD_COLOR_RED);
				}
			}

			// Draw data
			else if (FFT_HEIGHT - y < FFT_Display[x + 8]) {
				LCD_WriteRAM(LCD_COLOR_BLUE);
			}

			// Draw background
			else {
				LCD_WriteRAM(LCD_COLOR_WHITE);
			}
		}
	}

	GL_SetTextColor(LCD_COLOR_RED);
//	GL_SetBackColor(LCD_COLOR_GREY);
	LCD_StringLine(0, 130, (char*) &LCD_buffer[0]);
}
