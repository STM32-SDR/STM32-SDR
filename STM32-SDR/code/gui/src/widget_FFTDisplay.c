/*
 * Code for FFT widget
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
#include <string.h>
#include <stdio.h>

#include "arm_math.h"
#include "DSP_Processing.h"
#include "TSHal.h"
#include "DMA_IRQ_Handler.h"
#include "AGC_Processing.h"
#include "math.h"
#include "widget_FFTDisplay.h"
#include "ModeSelect.h"

//waterfall
//int		line;
int		WF_Line0 ;
int		WF_Count = 0;
int		WF_Bfr[15360];
int		*pWFBfr;
int		WF_Flag = 1; //Default to Spectrum Display

extern int NCO_Point;

// Constants
static const int FFT_WIDTH   = 240;
static const int FFT_HEIGHT  =  64;
static const int SELFREQ_ADJ =   4;
static const int CHARACTER_WIDTH = 8;
static const int MAX_FREQ_DIGITS = 5;
static const int SMETER_HEIGHT = 8;

static const int MAX_WF_COUNT = 2;

// Private function prototypes
static uint16_t WidgetFFT_GetWidth(GL_PageControls_TypeDef* pThis);
static uint16_t WidgetFFT_GetHeight(GL_PageControls_TypeDef* pThis);
static void WidgetFFT_EventHandler(GL_PageControls_TypeDef* pThis);
static void WidgetFFT_DrawHandler(GL_PageControls_TypeDef* pThis, _Bool force);
static void displayFFT(int x, int y);
static void displayFrequencyOffsetText(_Bool force);
static void displayAGCVariables(int RSL);
static void displaySMeter(int RSL);
static void displayFilterBW (void);
void	Acquire (void);

static _Bool showAFOffsetOnScreen(void);


// TODO: Remove these? Only set; never used!
double NCO_0;
double NCO_1;
double NCO_2;


int		NCO_Bin;
uint8_t FFT_Display[256];
extern	int RSL_Mag;
extern  unsigned int Flow;
extern	unsigned int Fhigh;

#define df 15.625

#define ID_FFTSelFreqNum_LABEL 50105


/*
 * Public Interface
 */

/*
 * Create a new widget and add it to the passed in page
 * Note: May create multiple controls (labels, buttons, ...) and add them to the page.
 */
void Widget_AddToPage_NewFFTDisplay(uint16_t x, uint16_t y, GL_Page_TypeDef *pPage)
{
	GL_PageControls_TypeDef* newFFT = NewCustomWidget(
				0,
				WidgetFFT_GetWidth,
				WidgetFFT_GetHeight,
				WidgetFFT_EventHandler,
				WidgetFFT_DrawHandler,
				0);
	AddPageControlObj(x,    y, newFFT, pPage);
}

/*
 * Private Functions
 */
static uint16_t WidgetFFT_GetWidth(GL_PageControls_TypeDef* pThis)
{
	return FFT_WIDTH;
}
static uint16_t WidgetFFT_GetHeight(GL_PageControls_TypeDef* pThis)
{
	return FFT_HEIGHT;
}

// Write a signed integer to a right-justified string.
void intToCommaString(int16_t number, char *pDest, int numChar)
{
	int index = numChar - 1;
	// Null terminate
	pDest[index--] = 0;

	_Bool isNeg = 0;
	if (number < 0) {
		number = -number;
		isNeg = 1;
	}

	int digitsWritten = 0;
	// Handle 0
	if (number == 0) {
		pDest[index--] = '0';
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


static void WidgetFFT_EventHandler(GL_PageControls_TypeDef* pThis)
{
	// Get the coordinates:
	uint16_t X_Point, Y_Point;
	TS_GetTouchEventCoords(&X_Point, &Y_Point);


	//Update PSK NCO Frequency
	int fftLeftEdge = pThis->objCoordinates.MinX;
	NCO_Point = (X_Point - fftLeftEdge);
	NCO_Frequency = (double) ((NCO_Point * 15.625) + 125.);
	Acquire();
}


static void WidgetFFT_DrawHandler(GL_PageControls_TypeDef* pThis, _Bool force)
{
	// Bail if nothing to draw.
	if (!force && !DSP_Flag) {
		return;
	}

	// Extract the FFT's screen coordinates.
	int x = pThis->objCoordinates.MinX;
	int y = pThis->objCoordinates.MinY;

	// Compute RSL
	// TODO: Remove hard-coded numbers? Change to constants?
	//int RSL =  -115 +(int) ( 0.5 *(80.0-(float)PGAGain)+ (0.38 * (float)RSL_Mag));
	int RSL =  RSL_Cal +(int) ( 0.5 *(80.0-(float)PGAGain)+ (0.38 * (float)RSL_Mag));

	// Draw the FFT
	displayFFT(x, y);

	// Write the frequency offset text
	if (showAFOffsetOnScreen()) {
		displayFrequencyOffsetText(force);
	}

	// Display AGC Variables for Testing / Troubleshooting
	displayAGCVariables(RSL);

	// Display SMeter
	displaySMeter(RSL);

	// display Filter Passband
	displayFilterBW();

	// Final End of DSP and FFT Update Processing
	// chh DSP_Flag = 0;
}

static void displayFFT(int x, int y)
{
	for (int16_t j = 0; j < 256; j++) {
		if (FFT_Filter[j] > 64.0) {
			FFT_Display[j]  = 64;
		} else {
			FFT_Display[j] = (uint8_t)FFT_Filter[j];
		}
	}

	/*
	 * Display the FFT
	 * - Drop the bottom 8, and top 8 frequency-display bins to discard
	 *   noisy sections near band edges due to filtering.
	 */
	//float selectedFreqX = (float) (NCO_Frequency - 125) / 15.625;
	//if (selectedFreqX < 0) {
	//	selectedFreqX = 0;
	//}

	//NCO_Bin = (int)selectedFreqX + 8;

	NCO_Bin = NCO_Point + 8;

	if (!WF_Flag) {

		// Draw the FFT using direct memory writes (fast).
		LCD_SetDisplayWindow(x, y, FFT_HEIGHT, FFT_WIDTH);
		LCD_WriteRAM_PrepareDir(LCD_WriteRAMDir_Down);


		for (int x = 0; x < FFT_WIDTH; x++) {
			// Plot this column of the FFT.
			for (int y = 0; y < FFT_HEIGHT; y++) {

					// Draw red line for selected frequency
					//if ((x == (int) (selectedFreqX + 0.5)) && isShowingAFOffset){
					if ((x == NCO_Point && Mode_GetCurrentMode() == MODE_PSK)
						|| (x == 27 && Mode_GetCurrentMode() == MODE_CW)){
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
	}
	else
	{

		if (WF_Count == 0){
			// Draw the Waterfall using direct memory writes (fast).
			LCD_SetDisplayWindow(x, y, FFT_HEIGHT, FFT_WIDTH);
			LCD_WriteRAM_PrepareDir(LCD_WriteRAMDir_Right);

			// Send colorized FFT data to Waterfall Buffer
			for (int x = 0; x < FFT_WIDTH; x++)
			{
				*(pWFBfr + (FFT_WIDTH*WF_Line0) + x) = WFPalette[(FFT_Display[x + 8] )];
			}

			// Refresh the waterfall display--scrolling begins after 64 lines
			for (int y = WF_Line0; y < FFT_HEIGHT; y++){
				for (int x = 0; x <FFT_WIDTH; x++) {
					//if (x == (int)(selectedFreqX +0.5) && isShowingAFOffset) {
					if ((x == NCO_Point && Mode_GetCurrentMode() == MODE_PSK)
						|| (x == 27 && Mode_GetCurrentMode() == MODE_CW)){
						//LCD_WriteRAM(LCD_COLOR_RED);
						LCD_WriteRAM(LCD_COLOR_WHITE);
					}
					else {
						LCD_WriteRAM(*(pWFBfr + y*FFT_WIDTH + x));
					}
				}
			}
			for ( int y = 0; y < WF_Line0; y++){
				for (int x = 0; x < FFT_WIDTH; x++){
					//if (x == (int)(selectedFreqX +0.5) && isShowingAFOffset){
					if ((x == NCO_Point && Mode_GetCurrentMode() == MODE_PSK)
						|| (x == 27 && Mode_GetCurrentMode() == MODE_CW)){
						//LCD_WriteRAM(LCD_COLOR_RED);
						LCD_WriteRAM(LCD_COLOR_WHITE);
					}
					else {
						LCD_WriteRAM(*(pWFBfr + y*FFT_WIDTH + x));
					}
				}
			}
		}

		if (++WF_Count == MAX_WF_COUNT) {
			WF_Count = 0;
			WF_Line0--;
			if (WF_Line0 < 0) WF_Line0 = FFT_HEIGHT-1;
		}
	}
}

static _Bool showAFOffsetOnScreen(void)
{
	ModeType currentMode = Mode_GetCurrentMode();
	return (currentMode == MODE_PSK);
}

static void displayFrequencyOffsetText(_Bool force)
{
	// Update the frequency offset displayed (text):
	static double oldSelectedFreq = -1;
	static double old_m_SQOpen = -1;
	if (force || oldSelectedFreq != NCO_Frequency || old_m_SQOpen != m_SQOpen) {
		oldSelectedFreq = NCO_Frequency;
		old_m_SQOpen = m_SQOpen;

		int textY = FFT_HEIGHT - 18;
		int numberX = 4 * CHARACTER_WIDTH;
		int labelX = 1;

		GL_SetFont(GL_FONTOPTION_8x16);
		GL_SetBackColor(LCD_COLOR_BLACK);
		GL_SetTextColor(LCD_COLOR_WHITE);
		GL_PrintString(labelX, textY, "AF", 0);

		// Display location on label.
		if (m_SQOpen == 0)
			GL_SetTextColor(LCD_COLOR_RED);
		else
			GL_SetTextColor(LCD_COLOR_GREEN);
		char number[MAX_FREQ_DIGITS + 1];
		intToCommaString((int) NCO_Frequency, number, MAX_FREQ_DIGITS + 1);
		GL_PrintString(numberX, textY, number, 0);
	}
}

static void displayAGCVariables(int RSL)
{
	// Display AGC Variables for Testing / Troubleshooting
	//GL_SetTextColor(LCD_COLOR_RED);
	//GL_SetBackColor(LCD_COLOR_BLACK);
	//char test2[7];
	//intToCommaString((int)AGC_Mag, test2, 7);
	//GL_PrintString(75, 80, test2, 0);

	GL_SetFont(GL_FONTOPTION_8x16);
	GL_SetTextColor(LCD_COLOR_RED);
	GL_SetBackColor(LCD_COLOR_BLACK);
	char test3[3];
	intToCommaString(PGAGain, test3, 3);
	GL_PrintString(110, 80, test3, 0);

	GL_SetTextColor(LCD_COLOR_RED);
	GL_SetBackColor(LCD_COLOR_BLACK);
	char test4[5];
	intToCommaString(AGC_Signal, test4, 5);
	GL_PrintString(180, 80, test4, 0);

	GL_SetTextColor(LCD_COLOR_RED);
	GL_SetBackColor(LCD_COLOR_BLACK);
	char test5[5];
	intToCommaString(RSL, test5, 5);
	GL_PrintString(277, 80, test5, 0);
}


static void displaySMeter(int RSL)
{
	int x = 80;
	int y = 68;
	LCD_SetDisplayWindow(x, y, SMETER_HEIGHT, FFT_WIDTH);
	LCD_WriteRAM_PrepareDir(LCD_WriteRAMDir_Down);
	int Signal_Level = ((RSL + 120) * 26) / 10;
	for (int x = 0; x < FFT_WIDTH; x++) {
		for (int y = 0; y < SMETER_HEIGHT; y++) {
			if (x <= Signal_Level) {
				LCD_WriteRAM(LCD_COLOR_GREEN);
			}
			else {
				LCD_WriteRAM(LCD_COLOR_DGRAY);
			}
		}
	}

	char SMeter$[7];
	if (Signal_Level < 120) {
		if (Signal_Level < 0)
			Signal_Level = 0;

		int SLevel = Signal_Level / 12;
		sprintf(SMeter$, "S%i    ", SLevel);
	}
	else {
		int SLevel = 9;
		int R = (Signal_Level - 100) / 3;
		sprintf(SMeter$, "S%i+%2i", SLevel, R);
	}
	GL_SetBackColor(LCD_COLOR_BLACK);
	GL_SetTextColor(LCD_COLOR_WHITE);

	GL_PrintString(1, 64, SMeter$, 0);

}

static void displayFilterBW( void )
{
	int x = 80;
	int y = 66;
	LCD_SetDisplayWindow(x, y, 2, FFT_WIDTH);
	LCD_WriteRAM_PrepareDir(LCD_WriteRAMDir_Down);

	int x1 = (int)(Flow/df)-8;
	int x2 = (int)(Fhigh/df)-8;

	for (int x = 0; x < FFT_WIDTH; x++) {
		for (int y = 0; y < 2; y++) {
			if ((x >= x1)&&(x<=x2)) {
				LCD_WriteRAM(LCD_COLOR_YELLOW);
			}
			else {
				LCD_WriteRAM(LCD_COLOR_BLACK);
			}
		}
	}
}


void Init_Waterfall (void)
{
	pWFBfr = &WF_Bfr[0];
	WF_Count = 0;
	WF_Line0 = FFT_HEIGHT - 1;
}


