/*
 * Header file Text Enter code
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
#ifndef _TEXT_ENTER_H_
#define _TEXT_ENTER_H_
#include <stdint.h>

uint8_t text_cnt;

typedef enum {
	Text_Call = 0,
	Text_Name,
	Text_QTH,
	Text_Pwr,
	Text_Ant,
	Text_F6,
	Text_F7,
	Text_F8,
	Text_F9,
	Text_F10,
	Text_Items
} TextNumber;

TextNumber s_currentTextNumber;

void Text_Display(int TextIdx);
void Item_Display(int TextIdx);
void Text_Clear(int TextIdx);
void Text_Wipe(void);


_Bool Text_HaveValidEEPROMData(void);
void Text_ReadFromEEPROM(void);
void Text_WriteToEEPROM(void);
void Text_Initialize(void);
void Count_WriteToEEPROM(void);
void Count_ReadFromEEPROM(void);

char* Get_Contact(int ContactIdx);

void compose_F1(void);
void compose_F2(void);
void compose_F3(void);
void compose_F4(void);
void compose_F5(void);
void compose_F6(void);
void compose_F7(void);
void compose_F8(void);
void compose_F9(void);
void compose_F10(void);

 void Text_SetSelectedText(TextNumber newText);
 void Text_Display(int TextIdx);
 void Text_Clear(int TextIdx);
 void kybd_edit_text(char data);
 void kybd_edit_contact(char data, int ContactIdx);
 void Contact_Clear(int ContactIdx);

// Initialization
void Text_ResetToDefaults(void);

// Work with option data
const char* Text_GetName(int TextIdx);


// EEPROM Access
//void Options_WriteToEEPROM(void);
//_Bool Options_HaveValidEEPROMData(void);
//void Options_ReadFromEEPROM(void);
#endif
