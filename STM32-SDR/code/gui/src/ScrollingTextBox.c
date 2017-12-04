/*
 * Code for scrolling text box
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

#include "widgets.h"
#include <assert.h>
#include "ScrollingTextBox.h"
#include "DMA_IRQ_Handler.h"
#include "LcdDriver_ILI9320.h"
#include "ChangeOver.h"
#include "xprintf.h"

#define FIRSTROW		1
#define LASTROW			4
#define FIRSTCOL		1
#define LASTCOL			39
#define NUMROWS			(LASTROW - FIRSTROW + 1)
#define NUMCOLS			(LASTCOL - FIRSTCOL + 1)
#define NUMCHARS		NUMROWS*NUMCOLS

#define TEXT_LINE_HEIGHT    (17)
#define OFFSETX_ONAIR    0
//#define OFFSETY_ONAIR    (5 * TEXT_LINE_HEIGHT)
#define OFFSETY_ONAIR    97


//extern unsigned char NewChar;
unsigned char LCD_buffer[NUMCHARS];
const char BLANK = ' ';

static int col;
static int row;
static int charnum;
static int ptr;

int dx = 8;
int dy = 17;


void DisplayText ( char ch)
{
	int x;
	int y;

	//Set Text Color

	if(RxTx_InTxMode())
		GL_SetTextColor(LCD_COLOR_RED);
	else
		GL_SetTextColor(LCD_COLOR_BLACK);

	// handle backspace
	if (ch==8){
		charnum--;
		col--;
		if (row==FIRSTROW){
			if (charnum<0) charnum = 0;
			if (col<FIRSTCOL) col = FIRSTCOL;
		}
		else {
			if (charnum<0)  charnum += NUMCHARS;
			if (col<FIRSTCOL) {
				col += NUMCOLS;
				row--;
			}
			x = OFFSETX_ONAIR + (col-1)*dx;
			y = OFFSETY_ONAIR + (row-1)*dy;
			GL_PrintChar (x,y,BLANK,0);
		}
	}
	else {
		if ((ch<0x20) | (ch>0x7E)) ch=0x20;
		x = OFFSETX_ONAIR + (col-1)*dx;
		y = OFFSETY_ONAIR + (row-1)*dy;
		GL_PrintChar (x,y,ch,0);
		LCD_buffer[charnum] = ch;
		charnum++;
		charnum %= NUMCHARS;
		col++;
		if (col > LASTCOL) {
			row++;
			col = FIRSTCOL;
			if (row > LASTROW) {
				ScrollText();
			}
		}
	}

}


 void ScrollText ( void ) {

	int x;
	int y;

	for (row = FIRSTROW; row < LASTROW; row++){
		y = OFFSETY_ONAIR + (row-1)*dy;
		for (col = FIRSTCOL; col <= LASTCOL; col++){
			x = OFFSETX_ONAIR + (col-1)*dx;
			GL_PrintChar(x,y, LCD_buffer[col-1 + ptr],0);
		}
		ptr += NUMCOLS;
		ptr %= NUMCHARS;
	}
	ptr += (NUMCOLS<<1);
	ptr %= NUMCHARS;
	y = OFFSETY_ONAIR + (LASTROW-1)*dy;

	for (col = FIRSTCOL; col <= LASTCOL; col++) {
		x = OFFSETX_ONAIR + (col-1)*dx;
		GL_PrintChar(x,y,BLANK,0);
	}
	col = FIRSTCOL;
}


void InitTextDisplay (void){
	row = FIRSTROW;
	col = FIRSTCOL;
	ptr = NUMCOLS;
	charnum = 0;
}

void ClearTextDisplay(void) {
	int x;
	int y;
	GL_SetBackColor(LCD_COLOR_BLACK);
	for (row = FIRSTROW; row < LASTROW+1; row++)
	{
		y = OFFSETY_ONAIR + (row-1)*dy;

		for (col = FIRSTCOL; col <= LASTCOL; col++) {
			x = OFFSETX_ONAIR + (col-1)*dx;
			GL_PrintChar(x,y,BLANK,0);
		}
	}
	GL_SetBackColor(LCD_COLOR_WHITE);
	InitTextDisplay ();
}
