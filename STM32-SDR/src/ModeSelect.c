/*
 * ModeSelect.c
 *
 *  Created on: Apr 7, 2013
 *      Author: CharleyK
 */

#include 	"arm_math.h"
#include 	"TFT_Display.h"
#include 	"ModeSelect.h"

uint16_t Mode;

void Init_Mode(void)
{
	LCD_SetBackColor(YELLOW);
	LCD_SetTextColor(RED);
	LCD_StringLine(88, 0, " SSB ");
	LCD_StringLine(144, 0, " CW ");
	LCD_StringLine(188, 0, " PSK ");
	LCD_SetTextColor(BLACK);
	LCD_SetBackColor(WHITE);

	Mode = 0;
	LCD_StringLine(0, 200, "SSB");
}

void Set_Mode_Display(void)
{
	switch (Mode) {

	case 0:
		LCD_StringLine(0, 200, "SSB");
		break;

	case 1:
		LCD_StringLine(0, 200, "CW ");
		break;

	case 2:
		LCD_StringLine(0, 200, "PSK");
		break;
	}
}

