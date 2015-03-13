/*
 * Code for CW operation screen
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

#include "screen_All.h"
#include "widgets.h"
#include "ModeSelect.h"
#include "options.h"
#include <assert.h>
#include "AGC_Processing.h"
#include "xprintf.h"
#include "ScrollingTextBox.h"
#include "Text_Enter.h"
#include "FrequencyManager.h"


//extern 	int WF_Flag;

volatile int g_numDMAInterrupts = 0;
volatile int g_numTimer3Interrupts = 0;

// Used in this file to refer to the correct screen (helps to keep code copy-paste friendly.
static GL_Page_TypeDef *s_pThisScreen = &g_screenMainCW;

// following 4 lines added by MEC
static GL_PageControls_TypeDef* pAGCLabel;
static GL_PageControls_TypeDef* pPGALabel;
static GL_PageControls_TypeDef* pDACLabel;
static GL_PageControls_TypeDef* pRSLLabel;
static GL_PageControls_TypeDef* pFiltLabel;
static GL_PageControls_TypeDef* pFreqLabel;
static GL_PageControls_TypeDef* pTxFreqLabel;

extern 	int FilterNumber;

/**
 * Call-back prototypes
 */
//static void WS_Click(GL_PageControls_TypeDef* pThis);
//static void TR_Click(GL_PageControls_TypeDef* pThis);
//static void testButton_Click(GL_PageControls_TypeDef* pThis);

//extern void ClearTextDisplay(void);

static void Screen_SplitButtonClick(GL_PageControls_TypeDef* pThis);

static void Screen_TextButtonClick(GL_PageControls_TypeDef* pThis);

// following 4 functions added by MEC
static _Bool AGCStatusUpdateHandler(GL_PageControls_TypeDef* pThis, _Bool forceRedisplay)
{

     switch(AGC_Mode){

     case 0: Widget_ChangeLabelText(pAGCLabel, "CW AGC ");
     break;

     case 1: Widget_ChangeLabelText(pAGCLabel, "Digi AGC ");
     break;

     case 2: Widget_ChangeLabelText(pAGCLabel, "SSB AGC  ");
     break;

     case 3: Widget_ChangeLabelText(pAGCLabel, "AGC Off   ");
     break;
     return 0;

     }
      // No need to indicate update required because changing the
      // label text forces an update (redraw).
     return 0;
}

static _Bool FilterStatusUpdateHandler(GL_PageControls_TypeDef* pThis, _Bool forceRedisplay)
{
      // For CW, put this in code\gui\src\screen_MainCW.c
	  switch(FilterNumber) {
	  case 0:
    	  Widget_ChangeLabelText(pFiltLabel, "F0");
    	  break;
	  case 1:
    	  Widget_ChangeLabelText(pFiltLabel, "F1");
    	  break;
	  case 2:
    	  Widget_ChangeLabelText(pFiltLabel, "F2");
    	  break;
	  case 3:
    	  Widget_ChangeLabelText(pFiltLabel, "F3");
    	  break;
	  case 4:
    	  Widget_ChangeLabelText(pFiltLabel, "F4");
    	  break;
	  case 5:
    	  Widget_ChangeLabelText(pFiltLabel, "F5");
    	  break;
	  }

      // No need to indicate update required because changing the
      // label text forces an update (redraw).
      return 0;
}

static _Bool FreqStatusUpdateHandler(GL_PageControls_TypeDef* pThis, _Bool forceRedisplay)
{
      // For CW, put this in code\gui\src\screen_MainCW.c
	//and display the result
	static char buf[16] = {0};
	int val = FrequencyManager_GetCurrentFrequency();
	int i = 12, j=12;
	for(; i>=0 && j>=0 && val ; --i){
		if (i%3==0){
			buf[j] = ","[0];
//			xprintf("i = %d, j = %d, val = %d, buf[j] = %c\n", i, j, val, buf[j]);
			j--;
		}
		buf[j] = "0123456789"[val % 10];
//		xprintf("i = %d, j = %d, val = %d, buf[j] = %c\n", i, j, val, buf[j]);
		val /= 10;
		j--;
	}
		//replace leading 0 with space
	for(; i>=0 && j>=0; --i, val /= 10){
		buf[j] = " "[0];
		j--;
//		xprintf("i = %d, j = %d, val = %d, buf[j] = %c\n", i, j, val, buf[j]);
	}
	buf[12] = " "[0];
	buf[13] = "H"[0]; //replace last comma with Hz + null
	buf[14] = "z"[0];
	buf[15] = 0;
//		xprintf("Frequency = %s\n", buf);

	Widget_ChangeLabelText(pFreqLabel, buf);

      // No need to indicate update required because changing the
      // label text forces an update (redraw).
	return 0;
}

static _Bool PGAUpdateHandler(GL_PageControls_TypeDef* pThis, _Bool forceRedisplay){
	 return 0;
}

static _Bool DACUpdateHandler(GL_PageControls_TypeDef* pThis, _Bool forceRedisplay){
	 return 0;
}

static _Bool RSLUpdateHandler(GL_PageControls_TypeDef* pThis, _Bool forceRedisplay){
	return 0;
}



/**
 * Create the screen
 */
void ScreenMainCW_Create(void)
{
	Create_PageObj(s_pThisScreen);

	// Title
	GL_PageControls_TypeDef* ctrlPskText = Widget_NewLabel("", LCD_COLOR_YELLOW, LCD_COLOR_BLACK, 1, GL_FONTOPTION_16x24, 0);
	AddPageControlObj(0,  85, ctrlPskText, s_pThisScreen);

	// FFT
	Widget_AddToPage_NewFFTDisplay(80, 0, s_pThisScreen);


	// "Standard" complement of main screen buttons:
	// .. Mode
	GL_PageControls_TypeDef* btnMode = Widget_NewBigButtonMode();
	AddPageControlObj(0, 0, btnMode, s_pThisScreen);
	// .. Options
	GL_PageControls_TypeDef* btnOptions = Widget_NewBigButtonOptions();
	AddPageControlObj(0, LCD_HEIGHT - 54, btnOptions, s_pThisScreen);
	// .. Frequency
	GL_PageControls_TypeDef* btnFreq = Widget_NewBigButtonFrequency();
	AddPageControlObj(
			LCD_WIDTH - ((GL_Custom_TypeDef*)(btnFreq->objPTR))->GetWidth(btnFreq),
			LCD_HEIGHT - 12 - ((GL_Custom_TypeDef*)(btnFreq->objPTR))->GetHeight(btnFreq),
			btnFreq, s_pThisScreen);

	// .. Rx & Tx buttons (Remove when code can automatically switch)
	//GL_PageControls_TypeDef* btnTR  = NewButton(10, "Tx/Rx", TR_Click);
	//GL_PageControls_TypeDef* btnWS  = NewButton(9,  "Wf/Sp", WS_Click);
	//AddPageControlObj(100, LCD_HEIGHT - 42, btnTR, s_pThisScreen);
	//AddPageControlObj(170, LCD_HEIGHT - 42, btnWS, s_pThisScreen);

	// following 4 sections added by MEC
	//AGC Mode Label
	pAGCLabel = Widget_NewLabel("AGC_Mode ", LCD_COLOR_YELLOW, LCD_COLOR_BLACK, 0, GL_FONTOPTION_8x16,AGCStatusUpdateHandler);
	AddPageControlObj(0,  80, pAGCLabel, s_pThisScreen);

	//PGA Label
	pPGALabel = Widget_NewLabel(" RF ", LCD_COLOR_YELLOW, LCD_COLOR_BLACK, 0, GL_FONTOPTION_8x16,PGAUpdateHandler);
	AddPageControlObj(75,  80, pPGALabel, s_pThisScreen);

	//DAC Label
	pDACLabel = Widget_NewLabel(" AF ", LCD_COLOR_YELLOW, LCD_COLOR_BLACK, 0, GL_FONTOPTION_8x16,DACUpdateHandler);
	AddPageControlObj(150,  80, pDACLabel, s_pThisScreen);


	//RSL Label
	pRSLLabel = Widget_NewLabel(" RSL ", LCD_COLOR_YELLOW, LCD_COLOR_BLACK, 0, GL_FONTOPTION_8x16,RSLUpdateHandler);
	AddPageControlObj(235,  80, pRSLLabel, s_pThisScreen);

	//Filt Label
		pFiltLabel = Widget_NewLabel("F0", LCD_COLOR_YELLOW, LCD_COLOR_BLACK, 0, GL_FONTOPTION_8x16,FilterStatusUpdateHandler);
		AddPageControlObj(52, 12, pFiltLabel, s_pThisScreen);

	//Frequency Label
		pFreqLabel = Widget_NewLabel("", LCD_COLOR_GREEN, LCD_COLOR_BLACK, 0, GL_FONTOPTION_16x24,FreqStatusUpdateHandler);
		AddPageControlObj(82, 145, pFreqLabel, s_pThisScreen);

		pTxFreqLabel = Widget_NewLabel("", LCD_COLOR_GREEN, LCD_COLOR_BLACK, 0, GL_FONTOPTION_16x24,FreqStatusUpdateHandler);
		AddPageControlObj(34, 115, pTxFreqLabel, s_pThisScreen); // IsTransparent

		GL_PageControls_TypeDef* btnSplit = NewButton(123, " Split ", Screen_SplitButtonClick);
		AddPageControlObj(0, 145, btnSplit, s_pThisScreen);

	// Programmable buttons
		for (int i = 0; i < Prog_PSK1 - Prog_CW1; i++) {
			static GL_PageControls_TypeDef* btnText;
			btnText = NewButton(250 + Prog_CW1 + i, Text_GetName(i + Prog_CW1), Screen_TextButtonClick);
			int x = (i / 2) * 70 + 95;
			int y = (i % 2) * 25 + LCD_HEIGHT - 55 ;
			AddPageControlObj(x, y, btnText, s_pThisScreen);
		}

	// Just for testing
//	GL_PageControls_TypeDef* btnTest  = NewButton(11,  " Test ", testButton_Click);
//	AddPageControlObj(0, 150, btnTest, s_pThisScreen);

}


/**
 * Button callbacks
 */

static void Screen_SplitButtonClick(GL_PageControls_TypeDef* pThis)
{
	static char buf[19] = {0};
	buf[0]="T"[0];
	buf[1]="x"[0];
	buf[2]=" "[0];
	debug(GUI, "Screen_SplitButtonClick:\n");
	Screen_ButtonAnimate(pThis);
	debug(GUI, "Screen_SplitButtonClick: split = %d\n", FrequencyManager_isSplit());
	if (!FrequencyManager_isSplit()&&!TxSplit_isEntered()) {		// State when Split is not engaged and TX freq has not been set  .. JDG
		FrequencyManager_setSplit(1);
		uint32_t val = FrequencyManager_GetCurrentFrequency();
		debug(GUI, "Screen_SplitButtonClick: currentFreq = %d\n", val);
		debug(GUI, "Screen_SplitButtonClick: savedFreq = %d\n", FrequencyManager_GetCurrentFrequency());
		FrequencyManager_SaveRxFrequency(val);
		ChangeButtonText(s_pThisScreen, 123, " SetTx  ");
		}
		else if (FrequencyManager_isSplit()&&!TxSplit_isEntered()) {	// State when Split is engaged and TX freq has not been set  .. JDG
			ChangeButtonText(s_pThisScreen, 123, " Join  ");
			uint32_t val = FrequencyManager_GetCurrentFrequency();
			debug(GUI, "Screen_SplitButtonClick: currentFreq = %d\n", val);
			debug(GUI, "Screen_SplitButtonClick: savedFreq = %d\n", FrequencyManager_GetCurrentFrequency());
			FrequencyManager_SaveTxFrequency(val);
			int i = 15, j=15;
			for(; i>=0 && j>=0 && val ; --i){
				if (i%3==0){
					buf[j] = ","[0]; //put in commas
		//			xprintf("i = %d, j = %d, val = %d, buf[j] = %c\n", i, j, val, buf[j]);
					j--;
				}
				buf[j] = "0123456789"[val % 10];
		//		xprintf("i = %d, j = %d, val = %d, buf[j] = %c\n", i, j, val, buf[j]);
				val /= 10;
				j--;
			}
				//replace leading 0 with space, first 3 chars are "Tx "
			for(; i>=3 && j>=3; --i, val /= 10){
				buf[j] = " "[0];
				j--;
		//		xprintf("i = %d, j = %d, val = %d, buf[j] = %c\n", i, j, val, buf[j]);
			}
			buf[15] = " "[0];
			buf[16] = "H"[0]; //replace last comma with Hz + null
			buf[17] = "z"[0];
			buf[18] = 0;
			TxSplit_set(1);
			FrequencyManager_SetRxFrequency();
			}
		else {							// State when Split is engaged and TX freq has been set  .. JDG
				FrequencyManager_setSplit(0);
				TxSplit_set(0);
				strcpy(buf, "                  ");
				ChangeButtonText(s_pThisScreen, 123, " Split ");
			}
	Widget_ChangeLabelText(pTxFreqLabel, buf);

	RefreshPageControl (s_pThisScreen, 1);

}

static void Screen_TextButtonClick(GL_PageControls_TypeDef* pThis)
{
	debug(GUI, "Screen_TextButtonClick:\n");
	Screen_ButtonAnimate(pThis);
	uint16_t id = pThis->ID - 250;
	compose_Text (id);
}

/*
#include "ChangeOver.h"
static void WS_Click(GL_PageControls_TypeDef* pThis)
{
	WF_Flag = !WF_Flag;
	if (WF_Flag) Init_Waterfall();
}
static void TR_Click(GL_PageControls_TypeDef* pThis)
{
	if (RxTx_InTxMode()){
		RxTx_SetReceive();
		ClearTextDisplay();
	}
	else {
		RxTx_SetTransmit();
	}
}
static void testButton_Click(GL_PageControls_TypeDef* pThis)
{
	// Grab the values to prevent UART timing from affecting the count.
	int dmaCount = g_numDMAInterrupts;
	int timerCount = g_numTimer3Interrupts;
	g_numDMAInterrupts = 0;
	g_numTimer3Interrupts = 0;

	xprintf("\n\n\n");
	xprintf("Debug dump\n");
	xprintf("==========\n");
	xprintf("Number of DMA Interrupts serviced since last dump:    %10d\n", dmaCount);
	xprintf("Number of Timer3 Interrupts serviced since last dump: %10d\n", timerCount);

}
*/
