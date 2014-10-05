/*
 * code the facilitates input from the keyboard and filling buffers
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

#include "PSKMod.h"
#include "widgets.h"
#include "Text_Enter.h"
#include "STM32-SDR-Subroutines.h"
#include "xprintf.h"
#include <string.h>

#define KBD_BUFF_LEN 41 // 40 characters on the screen, +1 for null.

int kybd_mode = 0;
char Call[]    = "   Call   " ;
char Name[]    = " Name     ";

int i;

void set_kybd_mode( int data)
	{
	 kybd_mode = data;
	}

volatile char kybd_string[KBD_BUFF_LEN]; // this will hold the keyboard string
volatile char macro_string[KBD_BUFF_LEN];

void kybd_addCharacter(char Char);
void kybd_dispFunctionKey(uint8_t data);
void kybd_char_switch (char data);
//void compose_F6(void);
void Text2Buffer(char *str);
void String2Buffer(char *str);

void kybd_char_switch(char data)
{

	debug(KEYBOARD, "kybd_char_switch: kybd_mode = %x, data = %x\n", kybd_mode, data);
	switch(kybd_mode) {
	case 0: kybd_addCharacter(data); break;
	case 1: kybd_edit_contact(data,0); break;
	case 2: kybd_edit_contact(data,1); break;
	case 3: kybd_edit_text(data); break;
	}
}

void kybd_addCharacter(char Char)
{
	debug(KEYBOARD, "kybd_addCharacter: Char = %c\n", Char);
	// Count characters already inserted
	static uint8_t cnt = 0;

	// Have we filled the buffer yet?
	if (cnt < KBD_BUFF_LEN - 2) {
		// Not full: fill from left to right.
		kybd_string[cnt] = Char;
		cnt++;
	}
	else {
		// Already full, shift characters in buffer left one space and
		// append new character to end.
		for (int i = 1; i < KBD_BUFF_LEN - 2; i++)
			kybd_string[i - 1] = kybd_string[i];

		kybd_string[KBD_BUFF_LEN - 3] = Char;
	}

	// Add character for PSK transmission:
	PSK_addCharToTx(Char);
}


void kybd_dispFunctionKey(uint8_t data)
{
	debug(KEYBOARD, "kybd_dispFunctionKey: data = %d\n", data);
	compose_Text (data);
}

void String2Buffer(char *str) {
	char TempChar;
	do {
		TempChar = *str++;
		kybd_addCharacter(TempChar);
	} while (*str != 0);
 }


void Text2Buffer(char *str) {
	char TempChar;
	int count = strlen(str);
	debug(KEYBOARD, "Text2Buffer: string = '%s', count = %d\n", str, count);
	do {
		TempChar = *str++;
		kybd_addCharacter(TempChar);
		count--;
	} while (count != 0);

 }


