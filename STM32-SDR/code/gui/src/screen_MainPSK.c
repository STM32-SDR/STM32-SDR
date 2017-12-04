/*
 * Code for PSK operations screen
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
#include "Text_Enter.h"
#include "Keyboard_Input.h"
#include <assert.h>
#include <PSKMod.h>
#include "ScrollingTextBox.h"
#include "KeyboardStatus.h"
#include "AGC_Processing.h"
#include "xprintf.h"

extern 	int	NCOTUNE;
//extern 	int WF_Flag;
extern 	int FilterNumber;

// Used in this file to refer to the correct screen (helps to keep code copy-paste friendly.
static GL_Page_TypeDef *s_pThisScreen = &g_screenMainPSK;
static GL_PageControls_TypeDef* pKeyboardLabel;
static GL_PageControls_TypeDef* pAGCLabel;
static GL_PageControls_TypeDef* pPGALabel;
static GL_PageControls_TypeDef* pDACLabel;
static GL_PageControls_TypeDef* pRSLLabel;
static GL_PageControls_TypeDef* pFiltLabel;
//static GL_PageControls_TypeDef* pNCOLabel;

/**
 * Call-back prototypes
 */
static void Screen_TextButtonClick(GL_PageControls_TypeDef* pThis);
//static void WS_Click(GL_PageControls_TypeDef* pThis);
static void TR_Click(GL_PageControls_TypeDef* pThis);
static void N_Click(GL_PageControls_TypeDef* pThis);
static void C_Click(GL_PageControls_TypeDef* pThis);
//static void Clear_Click(GL_PageControls_TypeDef* pThis);

static _Bool KeyboardStatusUpdateHandler(GL_PageControls_TypeDef* pThis, _Bool forceRedisplay)
{
      // For CW, put this in code\gui\src\screen_MainCW.c
      if (KeyboardStatus_IsKeyboardWorking()) {
    	  Widget_ChangeLabelText(pKeyboardLabel, "Kbd attached");
      } else if(KeyboardStatus_IsUSBDeviceAttached()) {
    	  Widget_ChangeLabelText(pKeyboardLabel, "USB attached");
      } else {
    	  Widget_ChangeLabelText(pKeyboardLabel, "   No Kbd   ");
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



static _Bool AGCStatusUpdateHandler(GL_PageControls_TypeDef* pThis, _Bool forceRedisplay)
{

     switch(AGC_Mode){

     case 0: Widget_ChangeLabelText(pAGCLabel, "CW AGC  ");
     break;

     case 1: Widget_ChangeLabelText(pAGCLabel, "Digi AGC ");
     break;

     case 2: Widget_ChangeLabelText(pAGCLabel, "SSB AGC ");
     break;

     case 3: Widget_ChangeLabelText(pAGCLabel, "AGC Off ");
     break;
     return 0;

     }
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

/*
static _Bool NCOUpdateHandler(GL_PageControls_TypeDef* pThis, _Bool forceRedisplay){

	if (NCOTUNE) {
		Widget_ChangeLabelText(pNCOLabel, "^");
	}
	else {
		Widget_ChangeLabelText(pNCOLabel, "V");
	}
	return 0;
}
*/

/**
 * Create the screen
 */
void ScreenMainPSK_Create(void)
{
	Create_PageObj(s_pThisScreen);

	// PSK
	GL_PageControls_TypeDef* ctrlPskText = Widget_NewPSKTextDisplay();
	AddPageControlObj(0,  85, ctrlPskText, s_pThisScreen);

	// FFT
	Widget_AddToPage_NewFFTDisplay(80, 0, s_pThisScreen);


	// "Standard" complement of main screen buttons:
	// .. Mode
	GL_PageControls_TypeDef* btnMode = Widget_NewBigButtonMode();
	AddPageControlObj(0, 0, btnMode, s_pThisScreen);
	// .. Options
	GL_PageControls_TypeDef* btnOptions = Widget_NewBigButtonOptions();
	AddPageControlObj(0, LCD_HEIGHT - 42, btnOptions, s_pThisScreen);
	// .. Frequency
	GL_PageControls_TypeDef* btnFreq = Widget_NewBigButtonFrequency();
	AddPageControlObj(
			LCD_WIDTH - ((GL_Custom_TypeDef*)(btnFreq->objPTR))->GetWidth(btnFreq),
			LCD_HEIGHT - ((GL_Custom_TypeDef*)(btnFreq->objPTR))->GetHeight(btnFreq),
			btnFreq, s_pThisScreen);

	// Keyboard status
	pKeyboardLabel = Widget_NewLabel("Your keyboard...", LCD_COLOR_YELLOW, LCD_COLOR_BLACK, 0, GL_FONTOPTION_8x8,KeyboardStatusUpdateHandler);
	AddPageControlObj(110, LCD_HEIGHT - 40, pKeyboardLabel, s_pThisScreen);

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

	//NCO label
//	pNCOLabel = Widget_NewLabel("V", LCD_COLOR_YELLOW, LCD_COLOR_BLACK, 0, GL_FONTOPTION_8x16,NCOUpdateHandler);
//	AddPageControlObj(LCD_WIDTH - 12, 177, pNCOLabel, s_pThisScreen);

	// .. Rx & Tx buttons (Remove when code can automatically switch)
	GL_PageControls_TypeDef* btnFT  = NewButton(10, "Tune ^ ", TR_Click);
//	GL_PageControls_TypeDef* btnWS  = NewButton(9,  "Wf/Sp", WS_Click);
	GL_PageControls_TypeDef* btnN  = NewButton(13,  "NM ", N_Click);
	GL_PageControls_TypeDef* btnC  = NewButton(14,  "CS ", C_Click);
//	GL_PageControls_TypeDef* btnClear  = NewButton(16,  " Clear ", Clear_Click);


	AddPageControlObj(255, 170, btnFT, s_pThisScreen);
//	AddPageControlObj(170, LCD_HEIGHT - 42, btnWS, s_pThisScreen);
	AddPageControlObj(126,   170, btnN, s_pThisScreen);
	AddPageControlObj(0, 170, btnC, s_pThisScreen);
//	AddPageControlObj(255, 170, btnClear, s_pThisScreen);

	// Programmable buttons
		for (int i = 0; i < Text_Items - Prog_PSK1; i++) {
			static GL_PageControls_TypeDef* btnText;
			btnText = NewButton(250 + Prog_PSK1 + i, Text_GetName(i + Prog_PSK1), Screen_TextButtonClick);
			int x = i * 70 + 95;
//			int y = (i % 2) * 25 + LCD_HEIGHT - 55 ;
			int y = LCD_HEIGHT - 30 ;
			AddPageControlObj(x, y, btnText, s_pThisScreen);
		}

}

void Screen_PSK_SetTune (void){
	if ((Screen_GetScreenMode() == MAIN) & (Mode_GetCurrentMode() == MODE_PSK)){
		NCOTUNE=1;
		ChangeButtonText(s_pThisScreen, 10, "Tune ^ ");
	} else
		NCOTUNE=0;
}

/**
 * Button callbacks
 */
#include "ChangeOver.h"
/*
static void WS_Click(GL_PageControls_TypeDef* pThis)
{
	WF_Flag = !WF_Flag;
	if (WF_Flag) Init_Waterfall();
}
*/
static void TR_Click(GL_PageControls_TypeDef* pThis)
{
	/*if (RxTx_InTxMode()){
		RxTx_SetReceive();
		ClearTextDisplay();
	}
	else {
		RxTx_SetTransmit();
	} */
	Screen_ButtonAnimate(pThis);
	NCOTUNE = !NCOTUNE;
	if (NCOTUNE)
		ChangeButtonText(s_pThisScreen, 10, "Tune ^ ");
	else
		ChangeButtonText(s_pThisScreen, 10, "Tune v ");
}


static void N_Click(GL_PageControls_TypeDef* pThis)
{
	Screen_ButtonAnimate(pThis);
	set_kybd_mode(2);
	Contact_Clear(1);
	text_cnt = 0;
}

static void C_Click(GL_PageControls_TypeDef* pThis)
{
	Screen_ButtonAnimate(pThis);
	set_kybd_mode(1);
	Contact_Clear(0);
	text_cnt = 0;
}
/*
static void Clear_Click(GL_PageControls_TypeDef* pThis)
{
	ClearXmitBuffer();

}
*/
static void Screen_TextButtonClick(GL_PageControls_TypeDef* pThis)
{
	Screen_ButtonAnimate(pThis);
	debug(GUI, "Screen_TextButtonClick:\n");
	uint16_t id = pThis->ID - 250;
	compose_Text (id);
}
