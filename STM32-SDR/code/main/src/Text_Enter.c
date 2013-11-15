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
 */


#include "Text_Enter.h"
//#include <assert.h>
#include "Init_I2C.h"
#include "eeprom.h"
#include "widgets.h"
#include "Keyboard_Input.h"

#define EEPROM_OFFSET_text 300
#define EEPROM_OFFSET_count 900
#define EEPROM_SENTINEL_LOC 75
#define EEPROM_SENTINEL_VAL 1037

#define text_length 30
#define contact_length 10
int i;


_Bool Text_HaveValidEEPROMData(void);
void Text_ReadFromEEPROM(void);
void Text_WriteToEEPROM(void);
void Text_Initialize(void);
void Count_WriteToEEPROM(void);
void Count_ReadFromEEPROM(void);

uint8_t test_value;

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


TextNumber s_currentTextNumber = Text_Call;

typedef struct
{
char* Item;
char  Text[text_length+1];
uint8_t count;

} TextStruct;

// Order must match OptionNumber in options.h
	TextStruct s_TextData[] = {
	{
		/*Item*/ "Call",
		/*Text*/ " ",
		/*count*/ 7
	},

	{
		/*Item*/ "Name",
		/*Text*/ " ",
		/*count*/ 7
	},

	{
		/*Item*/ "QTH ",
		/*Text*/ " ",
		/*count*/ 3
	},

	{
		/*Item*/ "RIG ",
		/*Text*/ " ",
		/*count*/ 5
	},

	{
		/*Item*/ "ANT ",
		/*Text*/ " ",
		/*count*/ 10
	},

	{
		/*Item*/ " F6 ",
		/*Text*/ " ",
		/*count*/ 5
	},

	{
		/*Item*/ " F7 ",
		/*Text*/ " ",
		/*count*/ 5
	},

	{
		/*Item*/ " F8 ",
		/*Text*/ " ",
		/*count*/ 5
	},
	{
		/*Item*/ " F9 ",
		/*Text*/ " ",
		/*count*/ 5
	},

	{
		/*Item*/ "F10 ",
		/*Text*/ " ",
		/*count*/ 10
	},

};



// Work with option data
const char* Text_GetName(int TextIdx)
{
	//assert(TextIdx >= 0 && TextIdx < Text_Items);
	return s_TextData[TextIdx].Item;
}
const char* Text_GetText(int TextIdx)
{
	//assert(TextIdx >= 0 && TextIdx < Text_Items);
	return s_TextData[TextIdx].Text;
}

char* Get_Contact(int ContactIdx)
{
	return s_ContactData[ContactIdx].Text;
}


void Text_Display(int TextIdx)
{
	GL_SetFont(GL_FONTOPTION_8x16);
	GL_SetBackColor(LCD_COLOR_BLACK);
	GL_SetTextColor(LCD_COLOR_WHITE);
	GL_PrintString(50, 170,"                              ", 0);
	GL_PrintString(50, 170,s_TextData[TextIdx].Text, 0);
}

void Text_Wipe(void)
{
	GL_SetFont(GL_FONTOPTION_8x16);
	GL_SetBackColor(LCD_COLOR_BLACK);
	GL_SetTextColor(LCD_COLOR_WHITE);
	GL_PrintString(50, 170,"", 0);
}


void Item_Display(int TextIdx)
{
	GL_SetFont(GL_FONTOPTION_8x16);
	GL_SetBackColor(LCD_COLOR_BLACK);
	GL_SetTextColor(LCD_COLOR_RED);
	GL_PrintString(0, 170,s_TextData[TextIdx].Item, 0);
}

void Text_Clear(int TextIdx)
{
	for (i = 0; i < text_length; i++)
	{
	s_TextData[TextIdx].Text[i] = ' ';
	s_TextData[TextIdx].count = 0;
	}
	text_cnt = 0;
	s_TextData[TextIdx].Text[text_length] = '\0';
}

void Contact_Clear(int ContactIdx)
{
	for (i=0; i <contact_length; i++)
	s_ContactData[ContactIdx].Text[i] =' ';
	s_ContactData[ContactIdx].count = 0;
	text_cnt = 0;
}




// Option selection
TextNumber Text_GetSelectedText(void)
{
	return s_currentTextNumber;
}

void Text_SetSelectedText(TextNumber newText)
{
//	assert(newText >= 0 && newText < Text_Items);
	if (newText >= 0 && newText < Text_Items) s_currentTextNumber = newText;
	}


void kybd_edit_text(char data){
	if(data != 0x0D && text_cnt< text_length)
	{
		if(data == 0x08 && text_cnt >0){
		text_cnt--;
		s_TextData[s_currentTextNumber].Text[text_cnt] = ' ';
		s_TextData[s_currentTextNumber].count = text_cnt;
		Text_Wipe();
		Text_Display(s_currentTextNumber);
		}
		else {
		s_TextData[s_currentTextNumber].Text[text_cnt] = data;
		text_cnt++;
		s_TextData[s_currentTextNumber].count = text_cnt;
		Text_Display(s_currentTextNumber);
		}
	}
	else
	{
		set_kybd_mode(0);
	}
}

void kybd_edit_contact(char data, int ContactIdx){

	if(data != 0x0D && text_cnt< contact_length)
	//if(data != 10 && text_cnt< contact_length)
	{
		if(data == 0x08 && text_cnt >0){
		//if(data == 13 && text_cnt >0){
		text_cnt--;
		s_ContactData[ContactIdx].Text[text_cnt] = ' ';
		s_ContactData[ContactIdx].count = text_cnt;
		}
		else{
		s_ContactData[ContactIdx].Text[text_cnt] = data;
		text_cnt++;
		s_ContactData[ContactIdx].count = text_cnt;
		}

	}
	else
	{
		set_kybd_mode(0);
	}
}


void compose_F1(void){  //Call CQ

	String2Buffer(" CQ CQ CQ de ");
	if(s_TextData[Text_Call].count != 0)
	Text2Buffer(s_TextData[Text_Call].Text, s_TextData[Text_Call].count);
	String2Buffer(" ");
	if(s_TextData[Text_Name].count != 0)
	Text2Buffer(s_TextData[Text_Name].Text, s_TextData[Text_Name].count);
	String2Buffer(" pse kkk ");
}

void compose_F2(void){    // Call a station by call and name
	if(s_ContactData[0].count != 0)
	Text2Buffer(s_ContactData[0].Text, s_ContactData[0].count);
	String2Buffer(" ");
	if(s_ContactData[1].count != 0)
	Text2Buffer(s_ContactData[1].Text, s_ContactData[1].count);
	String2Buffer(" de ");
	if(s_TextData[Text_Call].count != 0) {
	Text2Buffer(s_TextData[Text_Call].Text, s_TextData[Text_Call].count);
	String2Buffer(" ");
	//Text2Buffer(s_TextData[Text_Call].Text, s_TextData[Text_Call].count);
	//String2Buffer(" ");
	}
}

void compose_F3(void){  //Xmit QTH
	String2Buffer("Thanks for the call ");
	if(s_ContactData[1].count != 0)	Text2Buffer(s_ContactData[1].Text, s_ContactData[1].count);
	if(s_TextData[Text_Name].count != 0)
	{
		String2Buffer(" ");
	String2Buffer("My Name is ");
	Text2Buffer(s_TextData[Text_Name].Text, s_TextData[Text_Name].count);
	}
	String2Buffer(" ");
	if(s_TextData[Text_QTH].count != 0)
	{
	String2Buffer("My QTH is ");
	Text2Buffer(s_TextData[Text_QTH].Text, s_TextData[Text_QTH].count);
	}
	String2Buffer(" ");
}

void compose_F4(void){   //Station Info
	if(s_TextData[Text_Pwr].count != 0)
	{String2Buffer("My rig is an STM32-SDR interfaced with a ");
	Text2Buffer(s_TextData[Text_Pwr].Text, s_TextData[Text_Pwr].count);
	String2Buffer(" ");
	}
	if(s_TextData[Text_Ant].count != 0)
	{
	String2Buffer("My antenna is a ");
	}
	Text2Buffer(s_TextData[Text_Ant].Text, s_TextData[Text_Ant].count);
	String2Buffer(" ");
}
void compose_F5(void){
	if(s_ContactData[1].count != 0)
	String2Buffer(" Well, ");
	Text2Buffer(s_ContactData[1].Text,s_ContactData[1].count);
	String2Buffer(" TNX for the PSK31 QSO");
	String2Buffer(" ");
	String2Buffer("Vy 73's, ");
	if(s_ContactData[0].count != 0)
	Text2Buffer(s_ContactData[0].Text,s_ContactData[0].count);
	String2Buffer(" de ");
	if(s_TextData[Text_Call].count != 0) {
	Text2Buffer(s_TextData[Text_Call].Text, s_TextData[Text_Call].count);
	String2Buffer(" kkkk ");
	}
}
void compose_F6(void){
	if(s_TextData[Text_F6].count != 0)
	Text2Buffer(s_TextData[Text_F6].Text, s_TextData[Text_F6].count);
	String2Buffer(" ");
}

void compose_F7(void){
	if(s_TextData[Text_F7].count != 0)
	Text2Buffer(s_TextData[Text_F7].Text, s_TextData[Text_F7].count);
	String2Buffer(" ");
}
void compose_F8(void){
	if(s_TextData[Text_F8].count != 0)
	Text2Buffer(s_TextData[Text_F8].Text, s_TextData[Text_F8].count);
	String2Buffer(" ");
}
void compose_F9(void){
	if(s_TextData[Text_F9].count != 0)
	Text2Buffer(s_TextData[Text_F9].Text, s_TextData[Text_F9].count);
	String2Buffer(" ");
}

//void compose_F10(void){
//	if(s_TextData[Text_F10].count != 0)
//	Text2Buffer(s_TextData[Text_F10].Text, s_TextData[Text_F10].count);
//	String2Buffer(" ");
//}


void Text_Initialize(void)
{
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

	for (int i = 0; i < Text_Items; i++) {
		for (int j =0; j<s_TextData[i].count; j++)
		I2C_WriteEEProm(EEPROM_OFFSET_text + i*30+j , s_TextData[i].Text[j]);
	}
	// Add Sentinel
	Write_Int_EEProm(EEPROM_SENTINEL_LOC, EEPROM_SENTINEL_VAL);
}

void Count_WriteToEEPROM(void)
{
	for (int i = 0; i < Text_Items; i++)
		I2C_WriteEEProm(EEPROM_OFFSET_count + i, s_TextData[i].count);
}

_Bool Text_HaveValidEEPROMData(void)
{
	int16_t sentinel = Read_Int_EEProm(EEPROM_SENTINEL_LOC);
	return (sentinel == EEPROM_SENTINEL_VAL);
}
void Text_ReadFromEEPROM(void)
{
	for (int i = 0; i < Text_Items; i++) {
		for (int j =0; j<s_TextData[i].count; j++)
			s_TextData[i].Text[j] = (char)I2C_ReadEEProm(EEPROM_OFFSET_text + i*30+j);
	}
}

void Count_ReadFromEEPROM(void)
{
	for (int i = 0; i < Text_Items; i++) {
		s_TextData[i].count = (uint8_t)I2C_ReadEEProm(EEPROM_OFFSET_count + i);
	}
}
