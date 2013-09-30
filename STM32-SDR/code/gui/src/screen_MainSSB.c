/*
 * Code for SSB operations screen
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

// Used in this file to refer to the correct screen (helps to keep code copy-paste friendly.
static GL_Page_TypeDef *s_pThisScreen = &g_screenMainSSB;

/**
 * Call-back prototypes
 */
static void tx_Click(GL_PageControls_TypeDef* pThis);
static void rx_Click(GL_PageControls_TypeDef* pThis);


static _Bool UpdateHandler(GL_PageControls_TypeDef* pThis, _Bool forceRedisplay)
{
	if (forceRedisplay) {
		return 1;
	}
	return 0;
}

/**
 * Create the screen
 */
void ScreenMainSSB_Create(void)
{
	Create_PageObj(s_pThisScreen);

	// Title
	GL_PageControls_TypeDef* ctrlPskText = Widget_NewLabel("SSB Screen", LCD_COLOR_YELLOW, LCD_COLOR_BLACK, 1, GL_FONTOPTION_16x24, UpdateHandler);
	AddPageControlObj(0,  80, ctrlPskText, s_pThisScreen);

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

	// .. Rx & Tx buttons (Remove when code can automatically switch)
	GL_PageControls_TypeDef* btnRx  = NewButton(10, " Rx ", rx_Click);
	GL_PageControls_TypeDef* btnTx  = NewButton(9,  " Tx ", tx_Click);
	AddPageControlObj(120, LCD_HEIGHT - 42, btnRx, s_pThisScreen);
	AddPageControlObj(165, LCD_HEIGHT - 42, btnTx, s_pThisScreen);
}


/**
 * Button callbacks
 */
#include "ChangeOver.h"
static void tx_Click(GL_PageControls_TypeDef* pThis)
{
	assert(Mode_GetCurrentMode() == MODE_SSB);
	Xmit_SSB_Sequence();
}
static void rx_Click(GL_PageControls_TypeDef* pThis)
{
	Receive_Sequence();
}

