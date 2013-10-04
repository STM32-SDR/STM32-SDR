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


double NCO_0;
double NCO_1;
double NCO_2;

int 	Signal_Level;
int		S;
char	SMeter$[7];

static const int FFT_WIDTH   = 240;
static const int FFT_HEIGHT  =  64;
static const int SELFREQ_ADJ =   4;
//static const int TEXT_OFFSET_BELOW_FFT = 4;
static const int CHARACTER_WIDTH = 8;
static const int MAX_FREQ_DIGITS = 5;
static const int SMETER_HEIGHT = 8;

static uint16_t WidgetFFT_GetWidth(GL_PageControls_TypeDef* pThis);
static uint16_t WidgetFFT_GetHeight(GL_PageControls_TypeDef* pThis);
static void WidgetFFT_EventHandler(GL_PageControls_TypeDef* pThis);
static void WidgetFFT_DrawHandler(GL_PageControls_TypeDef* pThis, _Bool force);

int 	NCO_Point;
int		NCO_Bin;
void	Acquire ( void );
uint8_t FFT_Display[256];
int 	RSL;
extern	int RSL_Mag;
float selectedFreqX;

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
	//int NCO_Point;
	TS_GetTouchEventCoords(&X_Point, &Y_Point);


	//Update PSK NCO Frequency
	int fftLeftEdge = pThis->objCoordinates.MinX;
	NCO_Point = ((int)X_Point - fftLeftEdge) +8;
	NCO_Frequency = (double) ((float) ((X_Point - fftLeftEdge) + 8) * 15.625);
	NCO_0 = NCO_Frequency;
	Acquire();
}

void	Acquire ( void ){
	//extern double NCO_Frequency;
	extern int count;
	extern int char_count;
	long i, S1, S2, W;
	double delta;

				/* this is where I  add a correction to the NCO frequency
					based on the nearby spectral peaks */
				S1 = 0;
				S2 = 0;
				delta = 0.;
				//for (i=-2; i<3; i++){
				for (i=-4; i<5; i++){
					W = (long)FFT_Filter[NCO_Point + i];
					S1 += W*i;
					S2 += W;
				}
				if (S2 != 0) delta = (double) S1/((double)S2);
				NCO_Frequency +=  (double)((float)delta * 15.625);
				NCO_1 = NCO_Frequency;
				SetRXFrequency (NCO_Frequency );
				count = 0;
				char_count = 0;
				}

static void WidgetFFT_DrawHandler(GL_PageControls_TypeDef* pThis, _Bool force)
{
	// Bail if nothing to draw.
	if (!force && !DSP_Flag) {
		return;
	}

	int x = pThis->objCoordinates.MinX;
	int y = pThis->objCoordinates.MinY;
	//int Frequency_CursorX;

		for (int16_t j = 0; j < 256; j++) {
		if (FFT_Filter[j] > 64.0) {
		FFT_Display[j]  = 64;
		}
		else					{
		FFT_Display[j] = (uint8_t)FFT_Filter[j];
		}
	}

	RSL =  -115 +(int) ( 0.5 *(80.0-(float)PGAGain)+ (0.38 * (float)RSL_Mag));

	/*
	 * Display the FFT
	 * - Drop the bottom 8, and top 8 frequency-display bins to discard
	 *   noisy sections near band edges due to filtering.
	 */
	selectedFreqX = (float) (NCO_Frequency - 125) / 15.625;
	//Frequency_CursorX = (int)selectedFreqX;
	if (selectedFreqX < 0) {
		selectedFreqX = 0;
	}

	NCO_Bin = (int)selectedFreqX + 8;

	// Draw the FFT using direct memory writes (fast).
	LCD_SetDisplayWindow(x, y, FFT_HEIGHT, FFT_WIDTH);
	LCD_WriteRAM_PrepareDir(LCD_WriteRAMDir_Down);

	for (int x = 0; x < FFT_WIDTH; x++) {
		// Plot this column of the FFT.
		for (int y = 0; y < FFT_HEIGHT; y++) {

			// Draw red line for selected frequency
			if (x == (int) (selectedFreqX + 0.5)) {
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
		else GL_SetTextColor(LCD_COLOR_GREEN);
		char number[MAX_FREQ_DIGITS + 1];
		intToCommaString((int)NCO_Frequency, number, MAX_FREQ_DIGITS + 1);
		GL_PrintString(numberX, textY, number, 0);
	}




	//Display AGC Variables for Testing / Troubleshooting
	//GL_SetTextColor(LCD_COLOR_RED);
	//GL_SetBackColor(LCD_COLOR_BLACK);
	//char test2[7];
	//intToCommaString((int)AGC_Mag, test2, 7);
	//GL_PrintString(75, 80, test2, 0);

	GL_SetTextColor(LCD_COLOR_RED);
	GL_SetBackColor(LCD_COLOR_BLACK);
	char test3[3];
	intToCommaString(PGAGain/2, test3, 3);
	GL_PrintString(175, 80, test3, 0);

	//GL_SetTextColor(LCD_COLOR_RED);
	//GL_SetBackColor(LCD_COLOR_BLACK);
	//char test4[5];
	//intToCommaString(RSL_Mag, test4, 5);
	//GL_PrintString(195, 80, test4, 0);

	GL_SetTextColor(LCD_COLOR_RED);
	GL_SetBackColor(LCD_COLOR_BLACK);
	char test5[5];
	intToCommaString(RSL, test5, 5);
	GL_PrintString(277, 80, test5, 0);

	//Display SMeter
	x = 80;
	y = 68;
	LCD_SetDisplayWindow(x, y, SMETER_HEIGHT, FFT_WIDTH);
	LCD_WriteRAM_PrepareDir(LCD_WriteRAMDir_Down);

	Signal_Level = ((RSL + 120)*26)/10;
	//Signal_Level = ((RSL_Mag + 120)*26)/10;
	for (int x = 0; x < FFT_WIDTH; x++){
		for (int y = 0; y <SMETER_HEIGHT; y++){
			if (x <= Signal_Level){
				LCD_WriteRAM(LCD_COLOR_GREEN);
			}
			else {
				LCD_WriteRAM(LCD_COLOR_DGRAY);
			}
		}
	}
	if (Signal_Level < 120) {
		S = Signal_Level/12;
		sprintf(SMeter$,"S%i    ",S);
	}
	else {
		S = 9;
		int R = (Signal_Level-100)/3;
		sprintf(SMeter$,"S%i+%i",S,R);
	}
	GL_SetBackColor(LCD_COLOR_BLACK);
	GL_SetTextColor(LCD_COLOR_WHITE);
	GL_PrintString (1,64,SMeter$,0);

	DSP_Flag = 0;   // Final End of DSP and FFT Update Processing

}
