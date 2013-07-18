/*
 * Keyboard_Input.c
 *
 *  Created on: Jun 15, 2013
 *      Author: CharleyK
 */

#include	"TFT_Display.h"
#include	"PSKMod.h"


#define KBD_BUFF_LEN 41 // 40 characters on the screen, +1 for null.

volatile char kybd_string[KBD_BUFF_LEN]; // this will hold the keyboard string
void kybd_addCharacter(char Char);
void String2Buffer(char *str);
void kybd_dispFunctionKey(uint8_t data);

void kybd_addCharacter(char Char)
{
	// Count characters already inserted
	static uint8_t cnt = 0;

	// Have we filled the buffer yet?
	if (cnt < KBD_BUFF_LEN - 2) {
		// Not full: fill from left to right.
		kybd_string[cnt] = Char;
		cnt++;
	}
	else {
		// Already full, shift characters in buffer left one spat and
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
	// Ensure queue is empty before adding data:
	if(!PSK_isQueueEmpty()) {
		return;
	}

	// Place data from macro into the buffer:
	switch(data) {
	case 0: String2Buffer(" CQ CQ CQ de W5BAA W5BAA "); break;
	case 1: String2Buffer("  de W5BAA "); break;
	case 2: String2Buffer(" tnx for  call, handle is Charley "); break;
	case 3: String2Buffer("  QTH is Marble Falls,  near Austin "); break;
	case 4: String2Buffer("  Rig is homebrew PSK xcvr @ 2 watts"); break;
	case 5: String2Buffer(" BTU "); break;
	case 6: String2Buffer("  de W5BAA k"); break;
	}
}

void String2Buffer(char *str) {
	char TempChar;
	do {
		TempChar = *str++;
		kybd_addCharacter(TempChar);
	} while (*str != 0);
 }





