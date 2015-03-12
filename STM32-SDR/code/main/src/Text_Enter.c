/*
 * Code that makes Text entry possible from keyboard
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


/*
 * Text_Enter.c
 *
 *  Created on: Aug 21, 2013
 *      Author: CharleyK
 *  Extensively modified: April/May 2014
 *  	By: Chris S
 *
 *  This file contains the function key text editor, tag editor,
 *  parser and macro interpreter.
 */


#include "Text_Enter.h"
//#include <assert.h>
#include "Init_I2C.h"
#include "eeprom.h"
#include "widgets.h"
#include "Keyboard_Input.h"
#include "xprintf.h"
#include <stdlib.h>
#include "assert.h"
#include "STM32-SDR-Subroutines.h"
#include "screen_All.h"
#include "FrequencyManager.h"

extern char* Mode_GetCurrentUserModeName (void);

#define EEPROM_OFFSET_text 4000
#define EEPROM_OFFSET_count 3000
#define EEPROM_SENTINEL_LOC 75
#define EEPROM_SENTINEL_VAL 1077

#define text_length 78 // keep this an even number, screen width = 38 char
#define label_length 6
#define tagText_length 35
#define tagLabel_length 6
#define contact_length 10

uint8_t text_cnt;
int textPointer; //points to the cursor position when editing text
int labelPointer;
int tagTextPointer;
int tagLabelPointer;
int freqLabelPointer;
int i;

#define TRUE 1
#define FALSE 0

_Bool Text_HaveValidEEPROMData(void);
void Text_ReadFromEEPROM(void);
void Text_WriteToEEPROM(void);
//void Text_Initialize(void);
void Count_WriteToEEPROM(void);
void Count_ReadFromEEPROM(void);
void delete_character(char *a, int position);
void insert_substring(char *a, char *b, int position);
char *substring(char *string, int position, int length);

uint8_t test_value;

//static int textEdit = TRUE; // select to edit text or label

int editState;
#define TEXT_EDIT 1
#define LABEL_EDIT 2
#define TAG_EDIT 3
#define TAG_LABEL_EDIT 4
#define PROG_EDIT 5
#define PROG_LABEL_EDIT 6
#define FREQ_LABEL_EDIT 7
#define NO_EDIT 8


char buf[text_length+1]; //editor buffer for string manipulation

/*
typedef struct
{
char  Text[contact_length+1];
uint8_t count;
} ContactStruct;

ContactStruct s_ContactData[] =  {
		{"  Call    ",
			10
		},
		{"  Name    ",
			10
		}
		};
		*/

TextNumber s_currentTextNumber = Text_F1;
TagNumber s_currentTagNumber = Tag_1;
BandPreset s_currentFreqNumber = FREQBAND_20M_PSK;

typedef struct
{
char  Item[label_length+1];
char  Text[text_length+1];
uint8_t labelCount; // count is used when reading data from eeprom
uint8_t textCount; // count is used when reading data from eeprom
} TextStruct;

// Order must match OptionNumber in options.h

	TextStruct s_TextData[] = {
	{
		/*Item*/ "1 Call",
		/*Text*/ "<Tx>CQ CQ CQ de <MyCall> pse k k k <Rx>",
		//                 ^10       ^20       ^30       ^40
		/*Count*/ 0,
		/*Count*/ 0
	},

	{
		/*Item*/ "2 Name",
		/*Text*/ "My name is <MyName> and my QTH is <QTH>.",
		//                 ^10       ^20       ^30       ^40
		/*Count*/ 0,
		/*Count*/ 0
	},

	{
		/*Item*/ "3 Rig",
		/*Text*/ "My rig is an <Rig>.",
		//                 ^10       ^20       ^30       ^40
		/*Count*/ 0,
		/*Count*/ 0
	},

	{
		/*Item*/ "4 Ant",
		/*Text*/ "My ant is a <Ant>.",
		//                 ^10       ^20       ^30       ^40
		/*Count*/ 0,
		/*Count*/ 0
	},

	{
		/*Item*/ "5 BTU",
		/*Text*/ "Well <StnNM>, BTU <StnCS> de <MyCall> kn <Rx>",
		//                 ^10       ^20       ^30       ^40
		/*Count*/ 0,
		/*Count*/ 0
	},

	{
		/*Item*/ "6 TNX",
		/*Text*/ "TNX for the PSK31 QSO. 73's",
		//                 ^10       ^20       ^30       ^40,
		/*Count*/ 0,
		/*Count*/ 0
	},

	{
		/*Item*/ "7 USB",
		/*Text*/ "<USB>",
		/*Count*/ 0,
		/*Count*/ 0
	},

	{
		/*Item*/ "8 LSB",
		/*Text*/ "<LSB>",
		/*Count*/ 0,
		/*Count*/ 0
	},
	{
		/*Item*/ "9 CW",
		/*Text*/ "<CW>",
		/*Count*/ 0,
		/*Count*/ 0
	},

	{
		/*Item*/ "10TxRx",
		/*Text*/ "<Tx/Rx>",
		/*Count*/ 0,
		/*Count*/ 0
	},
	{
		/*Item*/ "11 Clr",
		/*Text*/ "<Clear>",
		/*Count*/ 0,
		/*Count*/ 0
	},
	{
		/*Item*/ "F12",
		/*Text*/ "This is F12",
		/*Count*/ 0,
		/*Count*/ 0
	},
	// Program buttons for SSB
	{
		/*Item*/ " AGC",
		/*Text*/ "<NxAGC>",
		/*Count*/ 0,
		/*Count*/ 0
	},
	{
		/*Item*/ "Tx/Rx",
		/*Text*/ "<Tx/Rx>",
		/*Count*/ 0,
		/*Count*/ 0
	},
	{
		/*Item*/ "Filter",
		/*Text*/ "<NxFil2>",
		/*Count*/ 0,
		/*Count*/ 0
	},
	{
		/*Item*/ " Mode",
		/*Text*/ "<NxMode><NoFilt>",
		/*Count*/ 0,
		/*Count*/ 0
	},
	// Program buttons for CW
	{
		/*Item*/ " AGC",
		/*Text*/ "<NxAGC>",
		/*Count*/ 0,
		/*Count*/ 0
	},
	{
		/*Item*/ "WF/Sp",
		/*Text*/ "<WF/Sp>",
		/*Count*/ 0,
		/*Count*/ 0
	},
	{
		/*Item*/ "Filter",
		/*Text*/ "<NxFil5>",
		/*Count*/ 0,
		/*Count*/ 0
	},
	{
		/*Item*/ " Mode",
		/*Text*/ "<NxMode><NoFilt>",
		/*Count*/ 0,
		/*Count*/ 0
	},
	// Program buttons for PSK
	{
		/*Item*/ "WF/Sp",
		/*Text*/ "<WF/Sp>",
		/*Count*/ 0,
		/*Count*/ 0
	},
	{
		/*Item*/ " Mode",
		/*Text*/ "<NxMode><NoFilt>",
		/*Count*/ 0,
		/*Count*/ 0
	}
};

// tags used in macros
	// Macros tags must match STM32-SDR-Subroutines

	typedef struct
	{
	char  Item[tagLabel_length+1];
	char  Text[tagText_length+1];
	uint8_t labelCount; // count is used when reading data from eeprom
	uint8_t textCount; // count is used when reading data from eeprom
	} TagStruct;

	TagStruct s_TagData[] = {
	{
		/*Item*/ "MyName",
		/*Text*/ "John Doe",
		//                 ^10       ^20       ^30       ^40
		/*Count*/ 0,
		/*Count*/ 0
	},
	{
		/*Item*/ "MyCall",
		/*Text*/ "A1BCD",
		//                 ^10       ^20       ^30       ^40
		/*Count*/ 0,
		/*Count*/ 0
	},
	{
		/*Item*/ "QTH",
		/*Text*/ "My Home",
		//                 ^10       ^20       ^30       ^40
		/*Count*/ 0,
		/*Count*/ 0
	},
	{
		/*Item*/ "Rig",
		/*Text*/ "STM-32 SDR",
		//                 ^10       ^20       ^30       ^40
		/*Count*/ 0,
		/*Count*/ 0
	},
	{
		/*Item*/ "Ant",
		/*Text*/ "Dipole",
		//                 ^10       ^20       ^30       ^40
		/*Count*/ 0,
		/*Count*/ 0
	},
	{
		/*Item*/ "T6",
		/*Text*/ "",
		//                 ^10       ^20       ^30       ^40
		/*Count*/ 0,
		/*Count*/ 0
	},
	{
		/*Item*/ "T7",
		/*Text*/ "",
		//                 ^10       ^20       ^30       ^40
		/*Count*/ 0,
		/*Count*/ 0
	},
	{
		/*Item*/ "T8",
		/*Text*/ "",
		//                 ^10       ^20       ^30       ^40
		/*Count*/ 0,
		/*Count*/ 0
	},
	{
		/*Item*/ "T9",
		/*Text*/ "",
		//                 ^10       ^20       ^30       ^40
		/*Count*/ 0,
		/*Count*/ 0
	},
	{
		/*Item*/ "T10",
		/*Text*/ "",
		//                 ^10       ^20       ^30       ^40
		/*Count*/ 0,
		/*Count*/ 0
	},
	{
		/*Item*/ "T11",
		/*Text*/ "",
		//                 ^10       ^20       ^30       ^40
		/*Count*/ 0,
		/*Count*/ 0
	},
	{
		/*Item*/ "T12",
		/*Text*/ "",
		//                 ^10       ^20       ^30       ^40
		/*Count*/ 0,
		/*Count*/ 0
	},
	{
		/*Item*/ "StnCS",
		/*Text*/ "Stn Call",
		//                 ^10       ^20       ^30       ^40
		/*Count*/ 0,
		/*Count*/ 0
	},
	{
		/*Item*/ "StnNM",
		/*Text*/ "Stn Name",
		//                 ^10       ^20       ^30       ^40
		/*Count*/ 0,
		/*Count*/ 0
	},
	{
		/*Item*/ "NoFilt",
		/*Text*/ {0x1B, 0x00},
		//                 ^10       ^20       ^30       ^40
		/*Count*/ 0,
		/*Count*/ 0
	},
	{
		/*Item*/ "Filt1",
		/*Text*/ {0x1B, 0x01},
		//                 ^10       ^20       ^30       ^40
		/*Count*/ 0,
		/*Count*/ 0
	},
	{
		/*Item*/ "Filt2",
		/*Text*/ {0x1B, 0x02},
		//                 ^10       ^20       ^30       ^40
		/*Count*/ 0,
		/*Count*/ 0
	},
	{
		/*Item*/ "Filt3",
		/*Text*/ {0x1B, 0x03},
		//                 ^10       ^20       ^30       ^40
		/*Count*/ 0,
		/*Count*/ 0
	},
	{
		/*Item*/ "Filt4",
		/*Text*/ {0x1B, 0x04},
		//                 ^10       ^20       ^30       ^40
		/*Count*/ 0,
		/*Count*/ 0
	},
	{
		/*Item*/ "Filt5",
		/*Text*/ {0x1B, 0x05},
		//                 ^10       ^20       ^30       ^40
		/*Count*/ 0,
		/*Count*/ 0
	},
	{
		/*Item*/ "NxFil5",
		/*Text*/ {0x1B, 0x06},
		//                 ^10       ^20       ^30       ^40
		/*Count*/ 0,
		/*Count*/ 0
	},
	{
		/*Item*/ "Rx",
		/*Text*/ {0x1B, 0x08},
		//                 ^10       ^20       ^30       ^40
		/*Count*/ 0,
		/*Count*/ 0
	},
	{
		/*Item*/ "Tx",
		/*Text*/ {0x1B, 0x09},
		//                 ^10       ^20       ^30       ^40
		/*Count*/ 0,
		/*Count*/ 0
	},
	{
		/*Item*/ "Tx/Rx",
		/*Text*/ {0x1B, 0x0A},
		//                 ^10       ^20       ^30       ^40
		/*Count*/ 0,
		/*Count*/ 0
	},
	{
		/*Item*/ "Spect",
		/*Text*/ {0x1B, 0x0B},
		//                 ^10       ^20       ^30       ^40
		/*Count*/ 0,
		/*Count*/ 0
	},
	{
		/*Item*/ "WFall",
		/*Text*/ {0x1B, 0x0C},
		//                 ^10       ^20       ^30       ^40
		/*Count*/ 0,
		/*Count*/ 0
	},
	{
		/*Item*/ "WF/Sp",
		/*Text*/ {0x1B, 0x0D},
		//                 ^10       ^20       ^30       ^40
		/*Count*/ 0,
		/*Count*/ 0
	},
	{
		/*Item*/ "USB",
		/*Text*/ {0x1B, 0x0E},
		//                 ^10       ^20       ^30       ^40
		/*Count*/ 0,
		/*Count*/ 0
	},
	{
		/*Item*/ "LSB",
		/*Text*/ {0x1B, 0x0F},
		//                 ^10       ^20       ^30       ^40
		/*Count*/ 0,
		/*Count*/ 0
	},
	{
		/*Item*/ "CW",
		/*Text*/ {0x1B, 0x10},
		//                 ^10       ^20       ^30       ^40
		/*Count*/ 0,
		/*Count*/ 0
	},
	{
		/*Item*/ "CW-R",
		/*Text*/ {0x1B, 0x11},
		//                 ^10       ^20       ^30       ^40
		/*Count*/ 0,
		/*Count*/ 0
	},
	{
		/*Item*/ "PSK-U",
		/*Text*/ {0x1B, 0x12},
		//                 ^10       ^20       ^30       ^40
		/*Count*/ 0,
		/*Count*/ 0
	},
	{
		/*Item*/ "PSK-L",
		/*Text*/ {0x1B, 0x13},
		//                 ^10       ^20       ^30       ^40
		/*Count*/ 0,
		/*Count*/ 0
	},
	{
		/*Item*/ "Clear",
		/*Text*/ {0x1B, 0x14},
		//                 ^10       ^20       ^30       ^40
		/*Count*/ 0,
		/*Count*/ 0
	},
	{
		/*Item*/ "Freq",
		/*Text*/ {0x1B, 0x15},
		//                 ^10       ^20       ^30       ^40
		/*Count*/ 0,
		/*Count*/ 0
	},
	{
		/*Item*/ "NxMode",
		/*Text*/ {0x1B, 0x16},
		//                 ^10       ^20       ^30       ^40
		/*Count*/ 0,
		/*Count*/ 0
	},
	{
		/*Item*/ "PkAGC",
		/*Text*/ {0x1B, 0x17},
		//                 ^10       ^20       ^30       ^40
		/*Count*/ 0,
		/*Count*/ 0
	},
	{
		/*Item*/ "DigAGC",
		/*Text*/ {0x1B, 0x18},
		//                 ^10       ^20       ^30       ^40
		/*Count*/ 0,
		/*Count*/ 0
	},
	{
		/*Item*/ "SSBAGC",
		/*Text*/ {0x1B, 0x19},
		//                 ^10       ^20       ^30       ^40
		/*Count*/ 0,
		/*Count*/ 0
	},
	{
		/*Item*/ "OffAGC",
		/*Text*/ {0x1B, 0x1A},
		//                 ^10       ^20       ^30       ^40
		/*Count*/ 0,
		/*Count*/ 0
	},
	{
		/*Item*/ "NxAGC",
		/*Text*/ {0x1B, 0x1B},
		//                 ^10       ^20       ^30       ^40
		/*Count*/ 0,
		/*Count*/ 0
	},
	{
		/*Item*/ "NxFil2",
		/*Text*/ {0x1B, 0x1C},
		//                 ^10       ^20       ^30       ^40
		/*Count*/ 0,
		/*Count*/ 0
	},
	{
		/*Item*/ "NxFil3",
		/*Text*/ {0x1B, 0x1D},
		//                 ^10       ^20       ^30       ^40
		/*Count*/ 0,
		/*Count*/ 0
	},
	{
		/*Item*/ "NxFil4",
		/*Text*/ {0x1B, 0x1E},
		//                 ^10       ^20       ^30       ^40
		/*Count*/ 0,
		/*Count*/ 0
	}
};

// Work with option data
const char* Text_GetName(int TextIdx)
{
	debug(KEYBOARD, "Text_GetName:\n");

	static char buf1[label_length+1] = {0}; // +1 to allow space for for null
	int length = strlen(s_TextData[TextIdx].Item);
	for (int c=0; c<label_length; c++){
			if (c< length)
				buf1[c] = s_TextData[TextIdx].Item[c];
			else
				buf1[c] = ' '; //pad remainder of line with spaces
	}
	debug(KEYBOARD, "Text_GetName: buf1 = %s\n", buf1);
	return &buf1[0];

}
const char* Text_GetText(int TextIdx)
{
	//assert(TextIdx >= 0 && TextIdx < Text_Items);
	return s_TextData[TextIdx].Text;
}

char* Get_Contact(int ContactIdx)
{
//	return s_ContactData[ContactIdx].Text;
	ContactIdx += stnCS; // Offset to other station call sign in tagData
	return s_TagData[ContactIdx].Text;
}

void Text_Display(int TextIdx, int textPointer)
{
	debug(KEYBOARD, "Text_Display: TextIdx = %d, textPointer = %d\n", TextIdx, textPointer);
	char buf1[text_length/2+1] = {0}; // +1 to allow space for for null
	char buf2[text_length/2+1] = {0};
	char buf3[text_length/2+1] = {0};
	char buf4[text_length/2+1] = {0};
	int length = strlen(s_TextData[TextIdx].Text);

	for (int c=0; c<text_length/2; c++){
		if (c< length)
			buf1[c] = s_TextData[TextIdx].Text[c];
		else
			buf1[c] = ' '; //pad remainder of line with spaces
		if (c == textPointer)
			buf2[c] = "^"[0];
		else
			buf2[c] = ' ';
	}

	for (int c=0; c<text_length/2; c++){
		if (c + text_length/2 < length)
			buf3[c] = s_TextData[TextIdx].Text[c+text_length/2];
		else
			buf3[c] = ' '; //pad remainder of line with spaces
		if (c + text_length/2 == textPointer)
			buf4[c] = "^"[0];
		else
			buf4[c] = ' ';
	}

	debug(KEYBOARD, "Text_Display: buf1 = '%s'\n", buf1);
	debug(KEYBOARD, "Text_Display: buf2 = '%s'\n", buf2);
	debug(KEYBOARD, "Text_Display: buf3 = '%s'\n", buf3);
	debug(KEYBOARD, "Text_Display: buf4 = '%s'\n", buf4);

	GL_SetFont(GL_FONTOPTION_8x16);
	GL_SetBackColor(LCD_COLOR_BLACK);
	GL_SetTextColor(LCD_COLOR_WHITE);
	GL_PrintString(4, 142, buf1, 0);
	GL_PrintString(0, 158, buf2, 0);
	GL_PrintString(4, 174, buf3, 0);
	GL_PrintString(0, 190, buf4, 0);
}

void Text_Blank(void){
	debug(KEYBOARD, "Text_Blank:\n");
	char buf[text_length/2+1] = {0};
	for (int c=0; c<text_length/2; c++)
		buf[c] = ' ';
	GL_SetFont(GL_FONTOPTION_8x16);
	GL_SetBackColor(LCD_COLOR_BLACK);
	GL_SetTextColor(LCD_COLOR_WHITE);
	GL_PrintString(4, 142, buf, 0);
	GL_PrintString(0, 158, buf, 0);
	GL_PrintString(4, 174, buf, 0);
	GL_PrintString(0, 190, buf, 0);
	GL_PrintString(185, 105,"       ", 0);
	GL_PrintString(236, 105,"       ", 0);
	GL_PrintString(232, 121,"       ", 0);
}

void Text_Wipe(void)
{
	debug(KEYBOARD, "Text_Wipe:\n");
	GL_SetFont(GL_FONTOPTION_8x16);
	GL_SetBackColor(LCD_COLOR_BLACK);
	GL_SetTextColor(LCD_COLOR_WHITE);
	GL_PrintString(50, 170,"", 0);
}


void Item_Display(int TextIdx)
{
	debug(KEYBOARD, "Item_Display:\n");
	char buf1[label_length+1] = {0};

	int length = strlen(s_TextData[TextIdx].Item);
	for (int c=0; c<label_length; c++){
		if (c< length)
			buf1[c] = s_TextData[TextIdx].Item[c];
		else
			buf1[c] = ' '; //pad remainder of line with spaces
	}

	GL_SetFont(GL_FONTOPTION_8x16);
	GL_SetBackColor(LCD_COLOR_BLACK);
	GL_SetTextColor(LCD_COLOR_RED);
	GL_PrintString(200, 105,"Label:", 0);
	GL_PrintString(250, 105, buf1, 0);
}

void Item_Edit_Display(int TextIdx, int textPointer)
{
	debug(KEYBOARD, "Item_Edit_Display:\n");
	char buf1[label_length+2] = {0};
	char buf2[label_length+2] = {0};

	int length = strlen(s_TextData[TextIdx].Item);
	for (int c=0; c<=label_length; c++){
		if (c< length)
			buf1[c] = s_TextData[TextIdx].Item[c];
		else
			buf1[c] = ' '; //pad remainder of line with spaces
		if (c == textPointer)
			buf2[c] = "^"[0];
		else
			buf2[c] = ' ';
	}

	GL_SetFont(GL_FONTOPTION_8x16);
	GL_SetBackColor(LCD_COLOR_BLACK);
	GL_SetTextColor(LCD_COLOR_RED);
	GL_PrintString(200, 105,"Label:", 0);
	GL_SetTextColor(LCD_COLOR_WHITE);
	GL_PrintString(250, 105, buf1, 0);
	GL_PrintString(246, 121, buf2, 0);
}

void Text_Clear(int TextIdx)
{
	debug(KEYBOARD, "Text_Clear:\n");
	strcpy(s_TextData[TextIdx].Text, "");
	textPointer = 0;

}

char* Tag_GetName(int TextIdx)
{
	debug(KEYBOARD, "Tag_GetName:\n");
	static char buf1[label_length+1] = {0}; // +1 to allow space for for null
	int length = strlen(s_TagData[TextIdx].Item);
	for (int c=0; c<label_length; c++){
			if (c< length)
				buf1[c] = s_TagData[TextIdx].Item[c];
			else
				buf1[c] = ' '; //pad remainder of line with spaces
	}
	debug(KEYBOARD, "Tag_GetName: buf1 = %s\n", buf1);
	return &buf1[0];
}

void TagText_Display(int TextIdx, int textPointer)
{
	debug(KEYBOARD, "TagText_Display: TextIdx = %d, textPointer = %d\n", TextIdx, textPointer);
	char buf1[text_length+2] = {0}; // +1 to allow space for for null
	char buf2[text_length+2] = {0};
	int length = strlen(s_TagData[TextIdx].Text);

	for (int c=0; c<text_length; c++){
		if (c< length)
			buf1[c] = s_TagData[TextIdx].Text[c];
		else
			buf1[c] = ' '; //pad remainder of line with spaces
		if (c == textPointer)
			buf2[c] = "^"[0];
		else
			buf2[c] = ' ';
	}

	debug(KEYBOARD, "TagText_Display: buf1 = '%s'\n", buf1);
	debug(KEYBOARD, "TagText_Display: buf2 = '%s'\n", buf2);

	GL_SetFont(GL_FONTOPTION_8x16);
	GL_SetBackColor(LCD_COLOR_BLACK);
	GL_SetTextColor(LCD_COLOR_WHITE);
	GL_PrintString(4, 142, buf1, 0);
	GL_PrintString(0, 158, buf2, 0);
}

void Tag_Blank(void){
	debug(KEYBOARD, "Tag_Blank:\n");
	char buf[tagText_length+1] = {0};
	for (int c=0; c<tagText_length; c++)
		buf[c] = ' ';
	GL_SetFont(GL_FONTOPTION_8x16);
	GL_SetBackColor(LCD_COLOR_BLACK);
	GL_SetTextColor(LCD_COLOR_WHITE);
	GL_PrintString(4, 142, buf, 0);
	GL_PrintString(0, 158, buf, 0);
	GL_PrintString(200, 105,"        ", 0);
	GL_PrintString(250, 105,"        ", 0);
	GL_PrintString(246, 121,"        ", 0);
}

void TagText_Wipe(void)
{
	debug(KEYBOARD, "TagText_Wipe:\n");
	GL_SetFont(GL_FONTOPTION_8x16);
	GL_SetBackColor(LCD_COLOR_BLACK);
	GL_SetTextColor(LCD_COLOR_WHITE);
	GL_PrintString(50, 170,"", 0);
}

void TagItem_Display(int TextIdx)
{
	debug(KEYBOARD, "TagItem_Display:\n");
	char buf1[label_length+1] = {0};

	int length = strlen(s_TagData[TextIdx].Item);
	for (int c=0; c<label_length; c++){
		if (c< length)
			buf1[c] = s_TagData[TextIdx].Item[c];
		else
			buf1[c] = ' '; //pad remainder of line with spaces
	}

	GL_SetFont(GL_FONTOPTION_8x16);
	GL_SetBackColor(LCD_COLOR_BLACK);
	GL_SetTextColor(LCD_COLOR_RED);
	GL_PrintString(185, 105,"Label:", 0);
	GL_PrintString(236, 105,buf1, 0);
}

void TagItem_Edit_Display(int TextIdx, int textPointer)
{
	debug(KEYBOARD, "TagItem_Edit_Display:\n");
	char buf1[label_length+2] = {0};
	char buf2[label_length+2] = {0};

	int length = strlen(s_TagData[TextIdx].Item);
	for (int c=0; c<=label_length; c++){
		if (c< length)
			buf1[c] = s_TagData[TextIdx].Item[c];
		else
			buf1[c] = ' '; //pad remainder of line with spaces
		if (c == textPointer)
			buf2[c] = "^"[0];
		else
			buf2[c] = ' ';
	}

	GL_SetFont(GL_FONTOPTION_8x16);
	GL_SetBackColor(LCD_COLOR_BLACK);
	GL_SetTextColor(LCD_COLOR_RED);
	GL_PrintString(200, 105,"Label:", 0);
	GL_SetTextColor(LCD_COLOR_WHITE);
	GL_PrintString(250, 105, buf1, 0);
	GL_PrintString(246, 121, buf2, 0);
}

void Contact_Clear(int ContactIdx)
{
	debug(KEYBOARD, "Contact_Clear:\n");
//	for (i=0; i <contact_length; i++)
//	s_ContactData[ContactIdx].Text[i] =' ';
//	s_ContactData[ContactIdx].count = 0;
	ContactIdx += stnCS; // Offset to other station call sign in tagData
	for (i=0; i <contact_length; i++)
		s_TagData[ContactIdx].Text[i] =' ';
}

// Option selection
TextNumber Text_GetSelectedText(void)
{
	return s_currentTextNumber;
}

void Text_SetSelectedText(TextNumber newText)
{
	debug(KEYBOARD, "Text_SetSelectedText:\n");
	assert(newText >= 0 && newText < Text_Items);
	if (newText >= 0 && newText < Text_Items){
		s_currentTextNumber = newText;
		textPointer = strlen(s_TextData[s_currentTextNumber].Text);
		labelPointer = strlen(s_TextData[s_currentTextNumber].Item);
		if (newText < Prog_SSB1)
			editState = TEXT_EDIT;
		else
			editState = PROG_EDIT;
		debug(KEYBOARD, "Text_SetSelectedText: newText = %d, textPointer = %d, labelPointer = %d\n", newText, textPointer, labelPointer);
	}
}

void Tag_SetSelectedText(TagNumber newText)
{
	debug(KEYBOARD, "Tag_SetSelectedText:\n");
	assert(newText >= 0 && newText < Tag_Items);
	if (newText >= 0 && newText < Tag_Items)
		s_currentTagNumber = newText;
		tagTextPointer = strlen(s_TagData[s_currentTagNumber].Text);
		tagLabelPointer = strlen(s_TagData[s_currentTagNumber].Item);
		editState = TAG_EDIT;
		debug(KEYBOARD, "Tag_SetSelectedText: newText = %d, tagTextPointer = %d, tagLabelPointer = %d\n", newText, tagTextPointer, tagLabelPointer);
	}

void Freq_SetSelectedText(int newText)
{
	debug(KEYBOARD, "Freq_SetSelectedText:\n");
	assert(newText >= 0 && newText < FREQBAND_NUMBER_OF_BANDS);
	if (newText >= 0 && newText < FREQBAND_NUMBER_OF_BANDS){
		s_currentFreqNumber = newText;
//		char* str = FrequencyManager_GetBandName(s_currentFreqNumber);
		freqLabelPointer = strlen(FrequencyManager_GetBandName(s_currentFreqNumber));
		editState = FREQ_LABEL_EDIT;
		debug(KEYBOARD, "Freq_SetSelectedText: newText = %d, freqLabelPointer = %d\n", newText, freqLabelPointer);
	}
}

void FreqName_Display(){
	debug(KEYBOARD, "FreqName_Display:\n");

	char buf1[freqLabel_length+2] = {0};
	char buf2[freqLabel_length+2] = {0};
//	char* str = FrequencyManager_BandName(s_currentFreqNumber);
	int length = strlen(FrequencyManager_GetBandName(s_currentFreqNumber));
	for (int c=0; c<=freqLabel_length; c++){
		if (c < length)
			buf1[c] = FrequencyManager_GetBandName(s_currentFreqNumber)[c];
		else
			buf1[c] = ' '; //pad remainder of line with spaces
		if (c == freqLabelPointer)
			buf2[c] = "^"[0];
		else
			buf2[c] = ' ';
	}

	char* str = Mode_GetCurrentUserModeName();

	GL_SetFont(GL_FONTOPTION_8x16);
	GL_SetBackColor(LCD_COLOR_BLACK);
	GL_SetTextColor(LCD_COLOR_WHITE);
	GL_PrintString(0, LCD_HEIGHT - 62,"Label:", 0);
	GL_PrintString(50, LCD_HEIGHT - 62, buf1, 0);
	GL_PrintString(46, LCD_HEIGHT - 46, buf2, 0);
	GL_PrintString(150, LCD_HEIGHT - 62,"Mode:", 0);
	GL_PrintString(200, LCD_HEIGHT - 62,str, 0);
}

/*This is the main state machine that determines the correct action for a
 * key stroke according to which string being edited. There are four strings
 * (editState):
 *  - text label (Item)
 *  - text
 *  - tag label
 *  - tag text
 *
 * Any key could be pressed so need to handle all options.
 * Some operations are common: scroll right/left, delete and insert character
 * Others are unique to each string: scroll up/down, tab
 *
 * Changes are applied to the data immediately.
 * In all cases the screen needs to be blanked and refreshed with the changes.
 */
void kybd_edit_text(char data){

	int textID = s_currentTextNumber;
	int tagID = s_currentTagNumber;
	int freqID = s_currentFreqNumber;

	int text_cnt = strlen(s_TextData[textID].Text);
	int label_cnt = strlen(s_TextData[textID].Item);
	int tagText_cnt = strlen(s_TagData[tagID].Text);
	int tagLabel_cnt = strlen(s_TagData[tagID].Item);
//	char* str = FrequencyManager_BandName(s_currentFreqNumber);
	int freqLabel_cnt = strlen(FrequencyManager_GetBandName(s_currentFreqNumber));

	debug(KEYBOARD, "kybd_edit_text: data = %x, editState = %d\n", data, editState);

	switch (data){

		case 0xa2: // down arrow
		case 0x0d: { // carriage return = next item
			switch (editState){
				case (TEXT_EDIT):
				case (LABEL_EDIT): {
					Screen_ChangeButtonTextLabel(textID);
					if (textID < Text_F12)
						textID++;
					else
						textID = Text_F1;
					debug(KEYBOARD, "kybd_edit_text: move down text, textID = %d\n", textID);
					textPointer = strlen(s_TextData[textID].Text);
					labelPointer = strlen(s_TextData[textID].Item);
					break;
				}
				case (PROG_EDIT):
				case (PROG_LABEL_EDIT): {
					Screen_ChangeButtonProgLabel(textID);
					if (textID < Prog_PSK2)
						textID++;
					else
						textID = Prog_SSB1;
					debug(KEYBOARD, "kybd_edit_text: move down text, textID = %d\n", textID);
					textPointer = strlen(s_TextData[textID].Text);
					labelPointer = strlen(s_TextData[textID].Item);
					break;
				}
				case (TAG_EDIT):
				case (TAG_LABEL_EDIT):{
					Screen_ChangeButtonTagLabel(tagID);
					if (tagID < Tag_10)
						tagID++;
					else
						tagID = 0;
					debug(KEYBOARD, "kybd_edit_text: move down tag, tagID = %d\n", tagID);
					tagTextPointer = strlen(s_TagData[tagID].Text);
					tagLabelPointer = strlen(s_TagData[tagID].Item);
					break;
				}
				case (FREQ_LABEL_EDIT):{
					Screen_ChangeButtonFreqLabel(freqID);
					if (freqID < FREQBAND_NUMBER_OF_BANDS-1)
						freqID++;
					else
						freqID = 0;
					debug(KEYBOARD, "kybd_edit_text: move down freq, freqID = %d\n", freqID);
//					char* str = FrequencyManager_BandName(s_currentFreqNumber);
					freqLabelPointer = strlen(FrequencyManager_GetBandName(freqID));
					FrequencyManager_SetSelectedBand(freqID);
					break;
				}
			}
			break;
		}

		case 0xa3: { // up arrow = previous item
			switch (editState){
				case (TEXT_EDIT):
				case (LABEL_EDIT): {
					Screen_ChangeButtonTextLabel(textID);
					if (textID > Text_F1)
						textID--;
					else
						textID = Text_F12;
					debug(KEYBOARD, "kybd_edit_text: move up text, textID = %d\n", textID);
					textPointer = strlen(s_TextData[textID].Text);
					labelPointer = strlen(s_TextData[textID].Item);
					break;
				}
				case (PROG_EDIT):
				case (PROG_LABEL_EDIT): {
					Screen_ChangeButtonProgLabel(textID);
					if (textID > Prog_SSB1)
						textID--;
					else
						textID = Prog_PSK2;
					debug(KEYBOARD, "kybd_edit_text: move up text, textID = %d\n", textID);
					textPointer = strlen(s_TextData[textID].Text);
					labelPointer = strlen(s_TextData[textID].Item);
					break;
				}

				case (TAG_EDIT):
				case (TAG_LABEL_EDIT):{
					Screen_ChangeButtonTagLabel(tagID);
					if (tagID > 0)
						tagID--;
					else
						tagID = Tag_10;
					debug(KEYBOARD, "kybd_edit_text: move up tag, tagID = %d\n", tagID);
					tagTextPointer = strlen(s_TagData[tagID].Text);
					tagLabelPointer = strlen(s_TagData[tagID].Item);
					break;
				}
				case (FREQ_LABEL_EDIT):{
					Screen_ChangeButtonFreqLabel(freqID);
					if (freqID > 0)
						freqID--;
					else
						freqID = FREQBAND_NUMBER_OF_BANDS-1;
					debug(KEYBOARD, "kybd_edit_text: move down freq, freqID = %d\n", freqID);
//					char* str = FrequencyManager_BandName(s_currentFreqNumber);
					freqLabelPointer = strlen(FrequencyManager_GetBandName(freqID));
					FrequencyManager_SetSelectedBand(freqID);
					break;
				}

			}
			break;
		}

		case 0xa1: { // left arrow = move left
			switch (editState){
				case (TEXT_EDIT):
				case (PROG_EDIT): {
					if (textPointer > 0){
						textPointer--;
						debug(KEYBOARD, "kybd_edit_text: move left text, textPointer = %d\n", textPointer);
					}
					break;
				}
				case (LABEL_EDIT):
				case (PROG_LABEL_EDIT): {
					if (labelPointer > 0){
						labelPointer--;
						debug(KEYBOARD, "kybd_edit_text: move left label, textPointer = %d\n", labelPointer);
					}
					break;
				}
				case (TAG_EDIT):{
					if (tagTextPointer > 0){
						tagTextPointer--;
						debug(KEYBOARD, "kybd_edit_text: move left tag text, tagTextPointer = %d\n", tagTextPointer);
					}
					break;
				}
				case (TAG_LABEL_EDIT):{
					if (tagLabelPointer > 0){
						tagLabelPointer--;
						debug(KEYBOARD, "kybd_edit_text: move left tag label, tagLabelPointer = %d\n", tagLabelPointer);
					}
					break;
				}
				case (FREQ_LABEL_EDIT):{
					if (freqLabelPointer > 0){
						freqLabelPointer--;
						debug(KEYBOARD, "kybd_edit_text: move left freq label, freqLabelPointer = %d\n", freqLabelPointer);
					}
					break;
				}

			}
			break;
		}

		case 0xa0: { // right arrow = move right
			switch (editState){
				case (TEXT_EDIT):
				case (PROG_EDIT): {
					if (textPointer < text_cnt){
						textPointer++;
						debug(KEYBOARD, "kybd_edit_text: move right text, textPointer = %d\n", textPointer);
					}
					break;
				}
				case (LABEL_EDIT):
				case (PROG_LABEL_EDIT): {
					if (labelPointer < label_cnt){
						labelPointer++;
						debug(KEYBOARD, "kybd_edit_text: move right label, labelPointer = %d\n", labelPointer);
					}
					break;
				}
				case (TAG_EDIT):{
					if (tagTextPointer < tagText_cnt){
						tagTextPointer++;
					debug(KEYBOARD, "kybd_edit_text: move right tag text, tagTextPointer = %d\n", tagTextPointer);
					}
					break;
				}
				case (TAG_LABEL_EDIT):{
					if (tagLabelPointer < tagLabel_cnt){
						tagLabelPointer++;
					debug(KEYBOARD, "kybd_edit_text: move right tag label, tagLabelPointer = %d\n", tagLabelPointer);
					}
					break;
				}
				case (FREQ_LABEL_EDIT):{
					if (freqLabelPointer < freqLabel_cnt){
						freqLabelPointer++;
					debug(KEYBOARD, "kybd_edit_text: move right freq label, tagLabelPointer = %d\n", tagLabelPointer);
					}
					break;
				}

			}
			break;
		}

		case 0x08: { // delete = remove character
			switch (editState){
				case (TEXT_EDIT):
				case (PROG_EDIT): {
					if (textPointer >0){ // delete back
						delete_character(s_TextData[textID].Text, textPointer+1);
						textPointer--;
						debug(KEYBOARD, "kybd_edit_text: delete text, s_TextData.Text = '%s', textPointer = %d\n", s_TextData[textID].Text, textPointer);
					}
					break;
				}
				case (LABEL_EDIT):
				case (PROG_LABEL_EDIT): {
					if (labelPointer >0){ // delete back
						delete_character(s_TextData[textID].Item, labelPointer+1);
						labelPointer--;
						debug(KEYBOARD, "kybd_edit_text: delete label, s_TextData.Item = '%s', labelPointer = %d\n", s_TextData[textID].Item, labelPointer);
					}
					break;
				}
				case (TAG_EDIT):{
					if (tagTextPointer >0){ // delete back
						delete_character(s_TagData[tagID].Text, tagTextPointer+1);
						tagTextPointer--;
						debug(KEYBOARD, "kybd_edit_text: delete tag text, s_TagData.Text = '%s', tagPointer = %d\n", s_TagData[tagID].Text, tagTextPointer);
					}
					break;
				}
				case (TAG_LABEL_EDIT):{
					if (tagLabelPointer >0){ // delete back
						delete_character(s_TagData[tagID].Item, tagLabelPointer+1);
						tagLabelPointer--;
						debug(KEYBOARD, "kybd_edit_text: delete tag label, s_TextData.Item = '%s', tagLabelPointer = %d\n", s_TagData[tagID].Item, tagLabelPointer);
					}
					break;
				}
				case (FREQ_LABEL_EDIT):{
					if (freqLabelPointer >0){ // delete back
//						char* str = FrequencyManager_BandName(freqID);
						delete_character(FrequencyManager_GetBandName(freqID), freqLabelPointer+1);
						freqLabelPointer--;
						debug(KEYBOARD, "kybd_edit_text: delete freq label, FrequencyManager_GetBandName(freqID) = '%s', freqLabelPointer = %d\n", FrequencyManager_GetBandName(freqID), freqLabelPointer);
					}
					break;
				}
			}
			break;
		}

		case 0x1b: { // escape = done
			switch (editState){
				case (TEXT_EDIT):
				case (PROG_EDIT): {
					editState = NO_EDIT;
					Screen_TextDone();
					debug(KEYBOARD, "kybd_edit_text: changing to main screen\n");
					break;
				}
				case (LABEL_EDIT): {
					Screen_ChangeButtonTextLabel(textID);
					editState = TEXT_EDIT; // change to edit text
					Text_Blank ();
					debug(KEYBOARD, "kybd_edit_text: changing to edit text\n");
					break;
				}
				case (PROG_LABEL_EDIT): {
					Screen_ChangeButtonProgLabel(textID);
					editState = PROG_EDIT; // change to edit text
					Text_Blank ();
					debug(KEYBOARD, "kybd_edit_text: changing to edit program text\n");
					break;
				}
				case (TAG_EDIT):{
					editState = NO_EDIT;
					Text_Blank ();
					Screen_TagDone();
					debug(KEYBOARD, "kybd_edit_text: changing to function screen\n");
					break;
				}
				case (TAG_LABEL_EDIT):{
					Screen_ChangeButtonTagLabel(tagID);
					editState = TAG_EDIT; // change to edit tag text
					Text_Blank ();
					debug(KEYBOARD, "kybd_edit_text: changing to edit tag text\n");
					break;
				}
				case (FREQ_LABEL_EDIT):{
					Screen_ChangeButtonFreqLabel(freqID);
					debug(KEYBOARD, "kybd_edit_text: changing to main screen\n");
					Screen_FreqDone();
					break;
				}
			}
			break;
		}

		case 0x09: { // tab = switch text and label
			switch (editState){
				case (TEXT_EDIT): {
					editState = LABEL_EDIT; // change to edit label
					labelPointer = label_cnt;
					Text_Blank();
					debug(KEYBOARD, "kybd_edit_text: changing to edit label\n");
					break;
				}
				case (PROG_EDIT): {
					editState = PROG_LABEL_EDIT; // change to edit label
					labelPointer = label_cnt;
					Text_Blank();
					debug(KEYBOARD, "kybd_edit_text: changing to edit prog label\n");
					break;
				}
				case (LABEL_EDIT): {
					Screen_ChangeButtonTextLabel(textID);
					editState = TEXT_EDIT; // change to edit text
					Text_Blank ();
					debug(KEYBOARD, "kybd_edit_text: changing to edit text\n");
					break;
				}
				case (PROG_LABEL_EDIT): {
					Screen_ChangeButtonProgLabel(textID);
					editState = PROG_EDIT; // change to edit text
					Text_Blank ();
					debug(KEYBOARD, "kybd_edit_text: changing to edit prog text\n");
					break;
				}
				case (TAG_EDIT):{
					editState = TAG_LABEL_EDIT; // change to edit tag label
					tagLabelPointer = tagLabel_cnt;
					Text_Blank ();
					debug(KEYBOARD, "kybd_edit_text: changing to edit tag label\n");
					break;
				}
				case (TAG_LABEL_EDIT):{
					Screen_ChangeButtonTagLabel(tagID);
					editState = TAG_EDIT; // change to edit tag text
					Text_Blank ();
					debug(KEYBOARD, "kybd_edit_text: changing to edit tag text\n");
					break;
				}
			}
			Text_Blank();
			break;
		}

		default: {
			if (data >= 0x20 && data <= 0x7E) { // printable text = insert character
				switch (editState){
					case (TEXT_EDIT):
					case (PROG_EDIT): {
						if (textPointer < text_length){
							buf[0] = data; // form substring of char to be inserted
							buf[1] = 0x00;
							s_TextData[textID].Text[text_length-1]= 0x00; // truncate to prevent string exceeding text_length
							insert_substring(s_TextData[textID].Text, buf, textPointer+1);
							debug(KEYBOARD, "kybd_edit_text: inserting text, s_TextData.Text = '%s'\n", s_TextData[textID].Text);
							textPointer++;
						}
						break;
					}
					case (LABEL_EDIT):
					case (PROG_LABEL_EDIT): {
						if (labelPointer < label_length){
							buf[0] = data; // form substring of char to be inserted
							buf[1] = 0x00;
							s_TextData[textID].Item[label_length-1]= 0x00; // truncate to prevent string exceeding text_length
							insert_substring(s_TextData[textID].Item, buf, labelPointer+1);
							debug(KEYBOARD, "kybd_edit_text: inserting label, s_TextData.Item = '%s'\n", s_TextData[textID].Item);
							labelPointer++;
						}
						break;
					}
					case (TAG_EDIT):{
						if (tagTextPointer < tagText_length){
							buf[0] = data; // form substring of char to be inserted
							buf[1] = 0x00;
							s_TagData[tagID].Text[tagText_length-1]= 0x00; // truncate to prevent string exceeding text_length
							insert_substring(s_TagData[tagID].Text, buf, tagTextPointer+1);
							debug(KEYBOARD, "kybd_edit_text: inserting tag text, s_TagData.Text = '%s'\n", s_TagData[tagID].Text);
							tagTextPointer++;
						}
						break;
					}
					case (TAG_LABEL_EDIT):{
						if (tagLabelPointer < tagLabel_length){
							buf[0] = data; // form substring of char to be inserted
							buf[1] = 0x00;
							s_TagData[tagID].Item[tagLabel_length-1]= 0x00; // truncate to prevent string exceeding text_length
							insert_substring(s_TagData[tagID].Item, buf, tagLabelPointer+1);
							debug(KEYBOARD, "kybd_edit_text: inserting tag label, s_TagData.Item = '%s'\n", s_TagData[tagID].Item);
							tagLabelPointer++;
						}
						break;
					}
					case (FREQ_LABEL_EDIT):{
						if (freqLabelPointer < freqLabel_length){
							buf[0] = data; // form substring of char to be inserted
							buf[1] = 0x00;
//							char* str = FrequencyManager_BandName(freqID);
							FrequencyManager_GetBandName(freqID)[freqLabel_length-1]= 0x00; // truncate to prevent string exceeding text_length
							insert_substring(FrequencyManager_GetBandName(freqID), buf, freqLabelPointer+1);
							debug(KEYBOARD, "kybd_edit_text: inserting freq label, FrequencyManager_GetBandName(freqID) = '%s'\n", FrequencyManager_GetBandName(freqID));
							freqLabelPointer++;
						}
						break;
					}
				}
			}
		}
	}
	switch (editState){
		case (TEXT_EDIT):
		case (PROG_EDIT): {
			Item_Display(textID);
			Text_Display(textID, textPointer);
			debug(KEYBOARD, "kybd_edit_text: displaying text\n");
			break;
		}
		case (LABEL_EDIT):
		case (PROG_LABEL_EDIT): {
			Text_Blank ();
			Item_Edit_Display(textID, labelPointer);
			debug(KEYBOARD, "kybd_edit_text: displaying label\n");
			break;
		}
		case (TAG_EDIT):{
			TagItem_Display (tagID);
			TagText_Display(tagID, tagTextPointer);
			debug(KEYBOARD, "kybd_edit_text: displaying tag text\n");
			break;
		}
		case (TAG_LABEL_EDIT):{
			Text_Blank ();
			TagItem_Edit_Display(tagID, tagLabelPointer);
			debug(KEYBOARD, "kybd_edit_text: displaying tag label\n");
			break;
		}
		case (FREQ_LABEL_EDIT):{
			s_currentFreqNumber = freqID;
			if (data != 0x1b) // Escape should not write to the display
				FreqName_Display();
			debug(KEYBOARD, "kybd_edit_text: displaying freq label\n");
			break;
		}
		case (NO_EDIT):{ // must be NO_EDIT so don't refresh the display
		}
	}

	s_currentTextNumber = textID;
	s_currentTagNumber = tagID;
	s_currentFreqNumber = freqID;
	debug(KEYBOARD, "kybd_edit_text: saving indices, s_currentTextNumber = %d, s_currentTagNumber = %d, s_currentFreqNumber = %d\n", s_currentTextNumber, s_currentTagNumber, s_currentFreqNumber);
}

void delete_character(char *a, int position)
{
	debug(KEYBOARD, "delete_character: a = '%s', position = %d\n", a, position);
   char *f, *e;
   int length;

   length = strlen(a);

   f = substring(a, 1, position - 2 ); //one character less than the substring length
   e = substring(a, position, length-position+1);

   strcpy(a, "");
   strcat(a, f);
   free(f);
   strcat(a, e);
   free(e);
}

void insert_substring(char *a, char *b, int position)
{
	debug(KEYBOARD, "insert_substring: a = '%s', b = '%s', position = %d\n", a, b, position);
   char *f, *e;
   int length;

   length = strlen(a);

   f = substring(a, 1, position - 1 );
   e = substring(a, position, length-position+1);

   strcpy(a, "");
   strcat(a, f);
   free(f);
   strcat(a, b);
   strcat(a, e);
   free(e);
}

char *substring(char *string, int position, int length)
{
   char *pointer;
   int c;
   pointer = malloc(length+1);
   debug(KEYBOARD, "substring: pointer = %d\n", pointer);

   for( c = 0 ; c < length ; c++ ){
      *(pointer+c) = *((string+position-1)+c);
   }

   *(pointer+c) = '\0';
   return pointer;
}

void kybd_edit_contact(char data, int ContactIdx){
	ContactIdx += stnCS; // offset to the other station call sign in tagData
	debug (KEYBOARD, "kybd_edit_contact: ContactIdx = %d\n", ContactIdx);

	if(data != 0x0D && text_cnt< contact_length)
	//if(data != 10 && text_cnt< contact_length)
	{
		if(data == 0x08 && text_cnt >0){
			//if(data == 13 && text_cnt >0){
			text_cnt--;

			s_TagData[ContactIdx].Text[text_cnt] = ' ';
		}
		else{
			s_TagData[ContactIdx].Text[text_cnt] = data;
			text_cnt++;
		}
		debug (KEYBOARD, "kybd_edit_contact: text_cnt = %d\n", text_cnt);
	}
	else // finished
	{
		s_TagData[ContactIdx].Text[text_cnt] = 0x00;
		set_kybd_mode(0);
	}
}

/*compose_Text takes the function key and determines what should be
 * be done with the text string.
 *
 */

void compose_Text(int messageNumber){

	debug(KEYBOARD, "compose_Text: message = %d\n", messageNumber);
	int lengthMessage = strlen(s_TextData[messageNumber].Text);
	char tag[10], message[text_length];
	int i, j, k, l;
	int found;
	if(lengthMessage != 0){
		for (k=0; k<text_length; k++)
			message[k] = 0x00;
		//parse the text string to find tags and replace with action
		// i = pointer in source, j = pointer in destination, k = pointer in tag
		j=0;
		for (i=0; i<lengthMessage; i++){
			if (s_TextData[messageNumber].Text[i] != "<"[0]){
				//copy byte-for-byte
				message[j] = s_TextData[messageNumber].Text[i];
				debug(KEYBOARD, "j = %d, copied message[j] = %c\n", j, message[j]);
				j++;
			}
			else {
				//if "<" extract remaining tag
				debug(KEYBOARD, "start of tag\n");
				for (k=0; k<10; k++)
					tag[k] = 0x00;
				for (k = 0; s_TextData[messageNumber].Text[i+k+1] != ">"[0]; k++) {
					tag[k] = s_TextData[messageNumber].Text[i+k+1];
					debug(KEYBOARD, "copied tag[k] = %c\n", tag[k]);
				}
				i = i + strlen(tag) + 1; // +2 because <> and -1 because next loop will i++
				debug(KEYBOARD, "end of tag, now lookup tag\n");

				//lookup tag
				found = FALSE;
				for (k = 0; k < Tag_Items; k++){
					debug(KEYBOARD, "comparing '%s' with '%s'\n", tag, s_TagData[k].Item);
					if (strcmp(tag, s_TagData[k].Item) == 0){
						found = TRUE;
						break;
					}
				}

				// check for <Frequency>
				if (found && s_TagData[k].Text[0] == 0x1B && s_TagData[k].Text[1] == 0x15){
				// copy string until end of tag
					debug(KEYBOARD, "Frequency tag detected\n");
					for (k=0; k<10; k++)
						tag[k] = 0x00;
					for (k = 0; s_TextData[messageNumber].Text[i+k+1] != ";"[0]; k++) {
						tag[k] = s_TextData[messageNumber].Text[i+k+1];
						debug(KEYBOARD, "copied tag[k] = %c\n", tag[k]);
					}
					debug(KEYBOARD, "New Freq = %s\n", tag);
					SetFrequency(tag); // setup the new frequency
					i = i + strlen(tag) + 1; // +1 because of ;
					debug(KEYBOARD, "Frequency end tag detected\n");
					found = FALSE;
				}

				// escape to execute macro, <Rx> = 0x08 is handled in PSK buffer
				if (found && s_TagData[k].Text[0] == 0x1B && s_TagData[k].Text[1] != 0x08){
					int macroNumber = s_TagData[k].Text[1];
					debug(KEYBOARD, "macro detected, macroNumber = %d\n", macroNumber);
					PlayMacro (macroNumber); // Execute macro
					found = FALSE;
				}

				if (found) { // must be text or <Rx> so concatenate
					debug(KEYBOARD, "tag match at k = %d\n", k);
				//insert tag text
					for (l = 0; l < strlen(s_TagData[k].Text); l++){
						message[j+l] = s_TagData[k].Text[l];
						debug(KEYBOARD, "j+l = %d, copied message[j] = %c\n", j+l, message[j+l]);
					}
					j = j + l; // note length "l" not one
				}

				//continue parsing from end of tag
				debug(KEYBOARD, "text appended, message = '%s'\n", message);
				debug(KEYBOARD, "continue from i = %d, j = %d\n", i, j);
			}
		}
	}
	if (strlen(message) != 0){ // check that we still have something to send
		Text2Buffer(message);
		String2Buffer(" ");
	}
}

void Text_Initialize(void)
{
	debug(KEYBOARD, "Text_Initialize:");
	// Load from EEPROM if valid:
	if (Text_HaveValidEEPROMData()) {
		Count_ReadFromEEPROM();
		Text_ReadFromEEPROM();
	} else {
		Count_WriteToEEPROM();
		Text_WriteToEEPROM();
	}
}

// EEPROM Access
void Text_WriteToEEPROM(void)
{
	debug(KEYBOARD, "Text_WriteToEEPROM:\n");
	int length;

	uint16_t offset = EEPROM_OFFSET_text;
	for (uint16_t i = 0; i < Text_Items; i++) {
		length = strlen(s_TextData[i].Item);
		for (uint16_t j =0; j<=length; j++)
			I2C_WriteEEProm(offset + i*(label_length+1)+j, s_TextData[i].Item[j]);
		debug(KEYBOARD, "Text_WriteToEEPROM: i = %d, s_TextData[i].Item = %s\n", i, s_TextData[i].Item);
	}

	offset += Text_Items*(label_length+1);
	for (uint16_t i = 0; i < Text_Items; i++) {
		length = strlen(s_TextData[i].Text);
		for (uint16_t j =0; j<=length; j++)
			I2C_WriteEEProm(offset + i*(text_length+1)+j , s_TextData[i].Text[j]);
		debug(KEYBOARD, "Text_WriteToEEPROM: i = %d, s_TextData[i].Text = %s\n", i, s_TextData[i].Text);
	}

	offset += Text_Items*(text_length+1);
	for (uint16_t i = 0; i <= Tag_12; i++) {
		length = strlen(s_TagData[i].Item);
		for (uint16_t j =0; j<=length; j++)
			I2C_WriteEEProm(offset + i*(tagLabel_length+1) + j , s_TagData[i].Item[j]);
		debug(KEYBOARD, "Text_WriteToEEPROM: i = %d, written s_TagData[i].Item = %s\n", i, s_TagData[i].Item);
	}

	offset += (Tag_12+1)*(tagLabel_length+1);
	for (uint16_t i = 0; i <= Tag_12; i++) {
		length = strlen(s_TagData[i].Text);
		for (uint16_t j =0; j<=length; j++)
			I2C_WriteEEProm(offset + i*(tagText_length+1) + j , s_TagData[i].Text[j]);
		debug(KEYBOARD, "Text_WriteToEEPROM: i = %d, written s_TagData[i].Text = %s\n", i, s_TagData[i].Text);
	}

	// Add Sentinel
	Write_Int_EEProm(EEPROM_SENTINEL_LOC, EEPROM_SENTINEL_VAL);
}

void Count_WriteToEEPROM(void)
{
	debug(KEYBOARD, "Count_WriteToEEPROM:\n");
	uint8_t length;
	uint16_t offset = EEPROM_OFFSET_count;

	for (uint16_t i = 0; i < Text_Items; i++){
		length = strlen(s_TextData[i].Item);
		I2C_WriteEEProm(offset + i, length);
		debug(KEYBOARD, "Count_WriteToEEPROM: address = %d, i = %d, length = %d\n", offset + i, i, length);
	}

	offset += Text_Items;
	for (uint16_t i = 0; i < Text_Items; i++){
		length = strlen(s_TextData[i].Text);
		I2C_WriteEEProm(offset + i, length);
		debug(KEYBOARD, "Count_WriteToEEPROM: address = %d, i = %d, length = %d\n", offset + i, i, length);
	}

	offset += Text_Items;
	for (uint16_t i = 0; i <= Tag_12; i++){
		length = strlen(s_TagData[i].Item);
		I2C_WriteEEProm(offset + i, length);
		debug(KEYBOARD, "Count_WriteToEEPROM: address = %d, i = %d, length = %d\n", offset + i, i, length);
	}

	offset += Tag_12+1;
	for (uint16_t i = 0; i <= Tag_12; i++){
		length = strlen(s_TagData[i].Text);
		I2C_WriteEEProm(offset + i, length);
		debug(KEYBOARD, "Count_WriteToEEPROM: address = %d, i = %d, length = %d\n", offset + i, i, length);
	}
}

_Bool Text_HaveValidEEPROMData(void)
{
	debug(KEYBOARD, "Text_HaveValidEEPROMData:\n");
	int16_t sentinel = Read_Int_EEProm(EEPROM_SENTINEL_LOC);
	return (sentinel == EEPROM_SENTINEL_VAL);
}

void Text_ReadFromEEPROM(void){

	debug(KEYBOARD, "Text_ReadFromEEPROM:\n");

	uint16_t offset = EEPROM_OFFSET_text;
	for (uint16_t i = 0; i < Text_Items; i++) {
		for (uint16_t j =0; j<=s_TextData[i].labelCount; j++)
			s_TextData[i].Item[j] = (char)I2C_ReadEEProm(offset + i*(label_length+1)+j);
		debug(KEYBOARD, "Text_ReadFromEEPROM: i = %d, s_TextData[i].Item = %s\n", i, s_TextData[i].Item);
	}

	offset += Text_Items*(label_length+1);
	for (uint16_t i = 0; i < Text_Items; i++) {
		for (uint16_t j =0; j<=s_TextData[i].textCount; j++)
			s_TextData[i].Text[j] = (char)I2C_ReadEEProm(offset + i*(text_length+1)+j);
		debug(KEYBOARD, "Text_ReadFromEEPROM: i = %d, s_TextData[i].Text = %s\n", i, s_TextData[i].Text);
	}

	offset += Text_Items*(text_length+1);
	for (uint16_t i = 0; i <= Tag_12; i++) {
		for (uint16_t j =0; j<=s_TagData[i].labelCount; j++)
			s_TagData[i].Item[j] = (char)I2C_ReadEEProm(offset + i*(tagLabel_length+1)+j);
		debug(KEYBOARD, "Text_ReadFromEEPROM: i = %d, s_TagData[i].Item = %s\n", i, s_TagData[i].Item);
	}

	offset += (Tag_12+1)*(tagLabel_length+1);
	for (uint16_t i = 0; i <= Tag_12; i++) {
		for (uint16_t j =0; j<=s_TagData[i].textCount; j++)
			s_TagData[i].Text[j] = (char)I2C_ReadEEProm(offset + i*(tagText_length+1)+j);
		debug(KEYBOARD, "Text_ReadFromEEPROM: i = %d, s_TagData[i].Text = %s\n", i, s_TagData[i].Text);
	}
}

void Count_ReadFromEEPROM(void)
{
	debug(KEYBOARD, "Count_ReadFromEEPROM:\n");
	uint16_t offset = EEPROM_OFFSET_count;

	for (uint16_t i = 0; i < Text_Items; i++) {
		s_TextData[i].labelCount = (uint8_t)I2C_ReadEEProm(offset + i);
		debug(KEYBOARD, "Count_ReadFromEEPROM: address = %d, i = %d, s_TextData[i].labelCount = %d\n", offset + i, i, s_TextData[i].labelCount);
	}

	offset += Text_Items;
	for (uint16_t i = 0; i < Text_Items; i++) {
		s_TextData[i].textCount = (uint8_t)I2C_ReadEEProm(offset + i);
		debug(KEYBOARD, "Count_ReadFromEEPROM: address = %d, i = %d, s_TextData[i].textCount = %d\n", offset + i, i, s_TextData[i].textCount);
	}

	offset += Text_Items;
	for (uint16_t i = 0; i <= Tag_12; i++) {
		s_TagData[i].labelCount = (uint8_t)I2C_ReadEEProm(offset + i);
		debug(KEYBOARD, "Count_ReadFromEEPROM: address = %d, i = %d, s_TagData[i].labelCount = %d\n", offset + i, i, s_TagData[i].labelCount);
	}

	offset += Tag_12+1;
	for (uint16_t i = 0; i <= Tag_12; i++) {
		s_TagData[i].textCount = (uint8_t)I2C_ReadEEProm(offset + i);
		debug(KEYBOARD, "Count_ReadFromEEPROM: address = %d, i = %d, s_TagData[i].textCount = %d\n", offset + i, i, s_TagData[i].textCount);
	}
}
