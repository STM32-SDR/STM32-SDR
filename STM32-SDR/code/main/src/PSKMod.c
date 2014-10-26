// PSK code
//
// Based on code by Moe Weatley; changes may have been made
// for the STM32-SDR project. Original license below:

/* ////////////////////////////////////////////////////////////////////
	 PSK31Core Library for transmission and reception of PSK31 signals
		using a PC soundcard  or .wav files.
					   Copyright 2000, Moe Wheatley, AE4JY

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
	 http://www.gnu.org/copyleft/lesser.html
	////////////////////////////////////////////////////////////////////*/

#include 	"PSKDet.h"
#include 	"PSKMod.h"
#include 	"PSK_TX_ShapeTable.h"
#include	"STM32-SDR-Subroutines.h"
#include	"xprintf.h"
#include	"ModeSelect.h"

#define TXTOG_CODE 1

#define SYM_NOCHANGE 0	/*Stay the same phase */
#define SYM_P180 2		/*Plus 180 deg */
#define SYM_OFF 4		/*No output */
#define SYM_ON 5		/*constant output */
//#define PSK_TX_BUFFER_SIZE 41	// 40 characters on screen + space for null
#define PSK_TX_BUFFER_SIZE 161

/* PSK31  generator variables */
int m_AddEndingZero;
uint16_t m_TxShiftReg;
double m_SymbolRate;

float m_RMSConstant;
double m_2PI;
float S1, S2;

float S1, S2;
extern const unsigned int VARICODE_TABLE[256];

int m_Ramp  = 0;
int m_pTail = 0;
char XmitBuffer[PSK_TX_BUFFER_SIZE];
extern unsigned char NewChar;

void ClearXmitBuffer(void);


void Update_PSK(void)
{
	extern const int PSKPhaseLookupTable[6][5];
	extern const float PSKShapeTbl[2049];

	static int Itbl_num, Qtbl_num;  //chhstatic
	static int m_PresentPhase; //chhstatic

	int tempdata;

	S1 = PSKShapeTbl[Itbl_num * 256 + m_Ramp + 1];
	S2 = PSKShapeTbl[Qtbl_num * 256 + m_Ramp + 1];

	m_Ramp += 1;
	// time to update symbol
	if (m_Ramp == 256) {
		m_Ramp = 0;
		uint8_t symbol = GetNextBPSKSymbol();
//		xprintf("mode = %d, %c", Mode_GetCurrentUserMode(), Mode_GetCurrentUserModeName());
		if (Mode_GetCurrentUserMode() == USERMODE_TUNE)
			tempdata = 0x110;
		else
			tempdata = PSKPhaseLookupTable[symbol][m_PresentPhase];

		Itbl_num = (tempdata & 0x700) >> 8;
		Qtbl_num = (tempdata & 0x070) >> 4;
		m_PresentPhase = (tempdata & 0x7);
	}
}

void InitPSKModulator(void)
{
	m_Ramp = 0;
	//m_pTail = 0;

	//Clear Xmit Buffer
	ClearXmitBuffer();
	m_AddEndingZero = TRUE;
	m_SymbolRate = SYMBOL_RATE31;

	m_TxShiftReg = 0;
}


void ClearXmitBuffer(void)

{
	m_pTail = 0;
	for (int i = 0; i < PSK_TX_BUFFER_SIZE-2; i++) {
		XmitBuffer[i] = ' ';
	}
	XmitBuffer[PSK_TX_BUFFER_SIZE-1] = '\0';
}

char GetNextBPSKSymbol(void)
{
	char symb;
	char ch;

	if (m_TxShiftReg == 0) {
		/* if is end of code */
		if (m_AddEndingZero) {
			symb = SYM_P180; /* end with a zero */
			m_AddEndingZero = FALSE;
		}
		else {
			ch = GetTxChar(); /*get next character to xmit */
			if (ch > 31 && ch<123)  NewChar = (unsigned char)ch;
			switch( ch ) {

				case TXTOG_CODE:
					symb = SYM_P180;
					break;

				default: /*get next VARICODE codeword to send */
					m_TxShiftReg = VARICODE_TABLE[ ch&0xFF ];
					symb = SYM_P180;	/*Start with a zero */
					break;
			}

		}
	}
	else /* is not end of code word so send next bit */
	{
		if (m_TxShiftReg & 0x8000) {
			symb = SYM_NOCHANGE;
		}
		else {
			symb = SYM_P180;
		}

		m_TxShiftReg = m_TxShiftReg << 1; /*point to next bit */
		if (m_TxShiftReg == 0) /* if at end of codeword */
			m_AddEndingZero = TRUE; /* need to send a zero nextime */
	}
	return symb;
}

/*==================================================================*/
/*get next character/symbol depending on TX state.		    		*/
/*==================================================================*/

/*==================================================================*/
/*		TX Queing routines				    					*/
/*==================================================================*/

/*==================================================================*/
char GetTxChar(void)
{
	char ch;

	if(!PSK_isQueueEmpty())	{
		if (XmitBuffer[0] != 0x1B) { // check for macro escape sequence
		ch = XmitBuffer[0] & 0x00FF;
		// Shift contents left one character
		for (int i = 0; i < m_pTail; i++)
			XmitBuffer[i] = XmitBuffer[i+1];

		// Put spaces in remaining empty spaces
		for(int i = m_pTail; i < PSK_TX_BUFFER_SIZE - 2; i++)
			XmitBuffer[i] = ' ';
		m_pTail--;

		} else {

			if (XmitBuffer[1] == 0x08){ // Receive mode macro
				debug(KEYBOARD, "Rx macro detected during PSK transmission\n");
				// Shift contents left two characters
				for (int i = 0; i < m_pTail; i++)
					XmitBuffer[i] = XmitBuffer[i+2];
				// Put spaces in remaining empty spaces
				for(int i = m_pTail-1; i < PSK_TX_BUFFER_SIZE - 2; i++)
					XmitBuffer[i] = ' ';
				m_pTail-=2;

				PlayMacro (0x08);
				ch = TXTOG_CODE;
			} else {
				ch = ' ';
			}
		}
	}
	else
		ch = TXTOG_CODE;		/* if que is empty return TXTOG_CODE */

	return ch;
}

// Queue a new character for transmission.
// Returns 1 if succeeded, 0 if failed (buffer full)
_Bool PSK_addCharToTx(char newTxChar)
{
	// Queue full?
	if (PSK_isQueueFull()) {
		return 0;
	}

	// Queue the character
	XmitBuffer[m_pTail++] = newTxChar;

	// Success
	return 1;
}

_Bool PSK_isQueueFull(void)
{
	// Full if data is in last position in the array
	// (0-indexd and space for the null)
	return m_pTail >= (PSK_TX_BUFFER_SIZE - 2);
}

_Bool PSK_isQueueEmpty(void)
{
	return 0 == m_pTail;
}
