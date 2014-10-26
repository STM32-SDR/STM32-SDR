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

extern uint8_t text_cnt;
extern int textPointer;
extern int labelPointer;
extern int tagTextPointer;
extern int tagLabelPointer;
extern int freqLabelPointer;

typedef enum {
	Text_F1 = 0, // keyboard function keys
	Text_F2,
	Text_F3,
	Text_F4,
	Text_F5,
	Text_F6,
	Text_F7,
	Text_F8,
	Text_F9,
	Text_F10,
	Text_F11,
	Text_F12,
	Prog_SSB1, // programmable buttons for USB screen
	Prog_SSB2,
	Prog_SSB3,
	Prog_SSB4,
	Prog_CW1, // programmable buttons for CW screen
	Prog_CW2,
	Prog_CW3,
	Prog_CW4,
	Prog_PSK1, // programmable buttons for PSK screen
	Prog_PSK2,
	Text_Items
} TextNumber;

typedef enum {
	Tag_1 = 0,
	Tag_2,
	Tag_3,
	Tag_4,
	Tag_5,
	Tag_6,
	Tag_7,
	Tag_8,
	Tag_9,
	Tag_10,
	Tag_11,
	Tag_12,
	stnCS,
	stnNM,
	noFilt,
	Filt1,
	Filt2,
	Filt3,
	Filt4,
	Filt5,
	NxFil5,
	Rx,
	Tx,
	TxRx,
	Spec,
	wFall,
	WfSp,
	SSB_U,
	SSB_L,
	CW_U,
	CW_L,
	PSK_U,
	PSK_L,
	Clear,
	Freq,
	Mode,
	AGC_Peak,
	AGC_Digital,
	AGC_SSB,
	AGC_Off,
	NxAGC,
	NxFil2,
	NxFil3,
	NxFil4,
	Tag_Items
} TagNumber;

TextNumber s_currentTextNumber;
TagNumber s_currentTagNumber;

void Text_Display(int TextIdx, int textPointer);
void Item_Display(int TextIdx);
void Text_Clear(int TextIdx);
void Text_Wipe(void);
void Text_Blank(void);

void TagText_Display(int TextIdx, int textPointer);
void TagItem_Display(int TextIdx);
void TagText_Clear(int TextIdx);
void TagText_Wipe(void);
void Tag_Blank(void);

_Bool Text_HaveValidEEPROMData(void);
void Text_ReadFromEEPROM(void);
void Text_WriteToEEPROM(void);
void Text_Initialize(void);
void Count_WriteToEEPROM(void);
void Count_ReadFromEEPROM(void);

void TagText_WriteToEEPROM(void);
void TagCount_WriteToEEPROM(void);

char* Get_Contact(int ContactIdx);

void compose_Text(int messageNumber);
//void compose_F1(void);
//void compose_F2(void);
//void compose_F3(void);
//void compose_F4(void);
//void compose_F5(void);
//void compose_F6(void);
//void compose_F7(void);
//void compose_F8(void);
//void compose_F9(void);
//void compose_F10(void);

 void Text_SetSelectedText(TextNumber newText);
 void Text_Display(int TextIdx, int textPointer);
 void Text_Clear(int TextIdx);
 void kybd_edit_text(char data);
 void kybd_edit_contact(char data, int ContactIdx);
 void Contact_Clear(int ContactIdx);
 void Tag_SetSelectedText(TagNumber newText);
 void TagText_Display(int TextIdx, int textPointer);
 void TagText_Clear(int TextIdx);
 void Freq_SetSelectedText(int newText);
 void FreqName_Display(void);

// Initialization
void Text_ResetToDefaults(void);

// Work with option data
const char* Text_GetName(int TextIdx);
char* Tag_GetName(int TextIdx);

// EEPROM Access
//void Options_WriteToEEPROM(void);
//_Bool Options_HaveValidEEPROMData(void);
//void Options_ReadFromEEPROM(void);
#endif
