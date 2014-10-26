/*
 * Header file for Keyboard input code
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


void kybd_addCharacter(char c);
void kybd_dispFunctionKey(uint8_t d);
void set_kybd_mode(int data);
void kybd_char_switch(char data);
void kybd_clear_call(void);
void kybd_clear_name(void);

void kybd_edit_my_call(void);
void kybd_clear_my_call(void);
void kybd_clear_my_name(void);

//extern uint8_t call_cnt;
//extern uint8_t my_call_cnt;

#define KBD_BUFF_LEN 40
extern volatile char kybd_string[KBD_BUFF_LEN];

void String2Buffer(char *str);
void Text2Buffer(char *str);
