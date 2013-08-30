 /*
 * PSKMod.c
 *
 *  Created on: Apr 19, 2013
 *      Author: CharleyK
 */
#include 	"PSKDet.h"
#include 	"PSKMod.h"
#include 	"PSK_TX_ShapeTable.h"

#define TXTOG_CODE 1

#define SYM_NOCHANGE 0	/*Stay the same phase */
#define SYM_P180 2		/*Plus 180 deg */
#define SYM_OFF 4		/*No output */
#define SYM_ON 5		/*constant output */
//#define PSK_TX_BUFFER_SIZE 41	// 40 characters on screen + space for null
#define PSK_TX_BUFFER_SIZE 161

float S1, S2;
extern const unsigned int VARICODE_TABLE[256];

int m_Ramp  = 0;
int m_pTail = 0;
char XmitBuffer[PSK_TX_BUFFER_SIZE];
extern unsigned char NewChar;


void Update_PSK(void)
{
	extern const int PSKPhaseLookupTable[6][5];
	extern const float PSKShapeTbl[2049];

	static int Itbl_num, Qtbl_num;  //chhstatic
	static int m_PresentPhase; //chhstatic

	S1 = PSKShapeTbl[Itbl_num * 256 + m_Ramp + 1];
	S2 = PSKShapeTbl[Qtbl_num * 256 + m_Ramp + 1];

	m_Ramp += 1;
	// time to update symbol
	if (m_Ramp == 256) {
		m_Ramp = 0;
		uint8_t symbol = GetNextBPSKSymbol();
		int tempdata = PSKPhaseLookupTable[symbol][m_PresentPhase];
		Itbl_num = (tempdata & 0x700) >> 8;
		Qtbl_num = (tempdata & 0x070) >> 4;
		m_PresentPhase = (tempdata & 0x7);
	}
}

void InitPSKModulator(void)
{
	m_Ramp = 0;
	m_pTail = 0;

	//Clear Xmit Buffer
	for (int i = 0; i < PSK_TX_BUFFER_SIZE-2; i++) {
		XmitBuffer[i] = ' ';
	}
	XmitBuffer[PSK_TX_BUFFER_SIZE-1] = '\0';

	m_AddEndingZero = TRUE;
	m_SymbolRate = SYMBOL_RATE31;

	m_TxShiftReg = 0;
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
		ch = XmitBuffer[0] & 0x00FF;
		// Shift contents left one character
		for (int i = 0; i < m_pTail; i++)
			XmitBuffer[i] = XmitBuffer[i+1];

		// Put spaces in remaining empty spaces
		for(int i = m_pTail; i < PSK_TX_BUFFER_SIZE - 2; i++)
			XmitBuffer[i] = ' ';
		m_pTail--;
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
