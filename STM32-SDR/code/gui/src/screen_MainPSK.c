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
#include <ScrollingTextBox.h>
#include "KeyboardStatus.h"


// Used in this file to refer to the correct screen (helps to keep code copy-paste friendly.
static GL_Page_TypeDef *s_pThisScreen = &g_screenMainPSK;
static GL_PageControls_TypeDef* pKeyboardLabel;
/**
 * Call-back prototypes
 */
static void tx_Click(GL_PageControls_TypeDef* pThis);
static void rx_Click(GL_PageControls_TypeDef* pThis);
static void N_Click(GL_PageControls_TypeDef* pThis);
static void C_Click(GL_PageControls_TypeDef* pThis);
static void T_Click(GL_PageControls_TypeDef* pThis);
static void Clear_Click(GL_PageControls_TypeDef* pThis);

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

/**
 * Create the screen
 */
void ScreenMainPSK_Create(void)
{
	Create_PageObj(s_pThisScreen);

	// PSK
	GL_PageControls_TypeDef* ctrlPskText = Widget_NewPSKTextDisplay();
	//AddPageControlObj(0,  80, ctrlPskText, s_pThisScreen);
	AddPageControlObj(0,  60, ctrlPskText, s_pThisScreen);

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

		AddPageControlObj(115,  228, pKeyboardLabel, s_pThisScreen);
	// .. Rx & Tx buttons (Remove when code can automatically switch)
	GL_PageControls_TypeDef* btnRx  = NewButton(10, " Rx ", rx_Click);
	GL_PageControls_TypeDef* btnTx  = NewButton(9,  " Tx ", tx_Click);
	GL_PageControls_TypeDef* btnN  = NewButton(13,  "N ", N_Click);
	GL_PageControls_TypeDef* btnC  = NewButton(14,  "C ", C_Click);
	GL_PageControls_TypeDef* btnT  = NewButton(15,  "T ", T_Click);
	GL_PageControls_TypeDef* btnClear  = NewButton(16,  "* ", Clear_Click);


	AddPageControlObj(120, LCD_HEIGHT - 42, btnRx, s_pThisScreen);
	AddPageControlObj(165, LCD_HEIGHT - 42, btnTx, s_pThisScreen);
	//AddPageControlObj(150,   165, btnN, s_pThisScreen);
	AddPageControlObj(126,   165, btnN, s_pThisScreen);
	AddPageControlObj(0, 165, btnC, s_pThisScreen);
	//AddPageControlObj(280, 165, btnT, s_pThisScreen);
	AddPageControlObj(240, 165, btnT, s_pThisScreen);
	AddPageControlObj(280, 165, btnClear, s_pThisScreen);
}


/**
 * Button callbacks
 */
#include "ChangeOver.h"
static void tx_Click(GL_PageControls_TypeDef* pThis)
{
	RxTx_SetTransmit();
}
static void rx_Click(GL_PageControls_TypeDef* pThis)
{
	RxTx_SetReceive();
	ClearTextDisplay();
}

static void N_Click(GL_PageControls_TypeDef* pThis)
{
	set_kybd_mode(2);
	Contact_Clear(1);
	text_cnt = 0;
}

static void C_Click(GL_PageControls_TypeDef* pThis)
{
	set_kybd_mode(1);
	Contact_Clear(0);
	text_cnt = 0;
}

static void T_Click(GL_PageControls_TypeDef* pThis)
{
	Screen_ShowScreen(&g_screenEditText);

}

static void Clear_Click(GL_PageControls_TypeDef* pThis)
{
	ClearXmitBuffer();

}
