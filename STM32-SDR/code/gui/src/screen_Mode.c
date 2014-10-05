/*
 * Code for mode selection screen ( PSK, SSB,CW: USb/LSB)
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
#include "DMA_IRQ_Handler.h"
#include <assert.h>

// Used in this file to refer to the correct screen (helps to keep code copy-paste friendly.
static GL_Page_TypeDef *s_pThisScreen = &g_screenMode;

#define ID_MODEBTN_START    100
#define FIRST_BUTTON_Y       30
#define SPACE_PER_BUTTON_Y   30
#define BUTTON_LEFT           0
#define LABEL_LEFT           60
#define LABEL_OFFSET_Y        5


/**
 * Call-back prototypes
 */
static void modeButton_Click(GL_PageControls_TypeDef* pThis);

/**
 * Create the screen
 */
void ScreenMode_Create(void)
{
	Create_PageObj(s_pThisScreen);

	/*
	 * Create the title and directions texts
	 */
	GL_PageControls_TypeDef* lblTitle = Widget_NewLabel("Mode Selection", LCD_COLOR_WHITE, LCD_COLOR_BLACK, 0, GL_FONTOPTION_16x24, 0);
//	GL_PageControls_TypeDef* lblDirections = Widget_NewLabel("Tap button for desired mode.", LCD_COLOR_BLUE, LCD_COLOR_BLACK, 0, GL_FONTOPTION_8x12Bold, 0);
	AddPageControlObj(0, 0, lblTitle,     s_pThisScreen);
//	AddPageControlObj(0,     35, lblDirections,     s_pThisScreen);

	// Populate the Mode buttons and descriptions:
	for (int i = 0; i < USERMODE_NUM_MODES; i++) {
		GL_PageControls_TypeDef* btnMode = NewButton(ID_MODEBTN_START + i, Mode_GetUserModeName(i), modeButton_Click);
		int btnX = BUTTON_LEFT;
		int btnY = i * SPACE_PER_BUTTON_Y + FIRST_BUTTON_Y;
		AddPageControlObj(btnX, btnY, btnMode, s_pThisScreen);

		GL_PageControls_TypeDef* lblDescription = Widget_NewLabel(Mode_GetUserModeDescription(i), LCD_COLOR_YELLOW, LCD_COLOR_BLACK, 1, GL_FONTOPTION_8x12Bold, 0);
		int lblX = LABEL_LEFT;
		int lblY = i * SPACE_PER_BUTTON_Y + FIRST_BUTTON_Y + LABEL_OFFSET_Y;
		AddPageControlObj(lblX, lblY, lblDescription, s_pThisScreen);
	}
}

/**
 * Callbacks
 */
static void modeButton_Click(GL_PageControls_TypeDef* pThis)
{
	uint16_t userMode = pThis->ID - ID_MODEBTN_START;
	assert(userMode >= 0 && userMode <= USERMODE_NUM_MODES);

	Mode_SetCurrentMode(userMode);
	Screen_ShowMainScreen();
}

