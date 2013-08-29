#include "widgets.h"
#include <assert.h>
#include "ScrollingTextBox.h"

#define FIRSTROW		1
//#define LASTROW			4
#define LASTROW			2
#define FIRSTCOL		1
#define LASTCOL			39
#define NUMROWS			(LASTROW - FIRSTROW + 1)
#define NUMCOLS			(LASTCOL - FIRSTCOL + 1)
#define NUMCHARS		NUMROWS*NUMCOLS

#define TEXT_LINE_HEIGHT    (17)
#define OFFSETX_ONAIR    0
//#define OFFSETY_ONAIR    (6 * TEXT_LINE_HEIGHT)
#define OFFSETY_ONAIR    (5 * TEXT_LINE_HEIGHT)

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
