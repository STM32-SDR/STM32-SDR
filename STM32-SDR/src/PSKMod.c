/*
 * PSKMod.c
 *
 *  Created on: Apr 19, 2013
 *      Author: CharleyK
 */
#include 	"PSKDet.h"
#include 	"PSKMod.h"
#include 	"PSK_TX_ShapeTable.h"
#include 	"TFT_Display.h"
#include	"User_Button.h"

//#define TXOFF_CODE 2	/* control code that can be placed in the input by Ctrl-Q */
//#define TXON_CODE 3		/* ditto for Ctrl-S */
#define TXTOG_CODE 1


#define SYM_NOCHANGE 0	/*Stay the same phase */
#define SYM_P180 2		/*Plus 180 deg */
#define SYM_OFF 4		/*No output */
#define SYM_ON 5		/*constant output */
#define PSK_TX_BUFFER_SIZE 40

float S1, S2;
extern const unsigned int VARICODE_TABLE[256];

int m_Ramp  = 0;
int m_pTail = 0;
int m_pHead = 0;
char XmitBuffer[PSK_TX_BUFFER_SIZE];



void Update_PSK(void)
{
	static uint8_t symbol;
	extern const int PSKPhaseLookupTable[6][5];
	static int Itbl_num, Qtbl_num;  //chhstatic
	static int m_PresentPhase; //chhstatic
	extern const float PSKShapeTbl[2049];

	int tempdata;

	S1 = PSKShapeTbl[Itbl_num * 256 + m_Ramp + 1];
	S2 = PSKShapeTbl[Qtbl_num * 256 + m_Ramp + 1];

	m_Ramp += 1;
	// time to update symbol
	if (m_Ramp == 256) {
		m_Ramp = 0;
		symbol = GetNextBPSKSymbol();
		tempdata = PSKPhaseLookupTable[symbol][m_PresentPhase];
		Itbl_num = (tempdata & 0x700) >> 8;
		Qtbl_num = (tempdata & 0x070) >> 4;
		m_PresentPhase = (tempdata & 0x7);
	}
}

void InitPSKModulator(void)
{
	m_Ramp = 0;
	m_pTail = 0;
	m_pHead = 0;

	//Clear Xmit Buffer
	for (int i = 0; i < PSK_TX_BUFFER_SIZE-2; i++) {
		XmitBuffer[i] = ' ';
	}
	XmitBuffer[PSK_TX_BUFFER_SIZE-1] = '\0';

	m_AddEndingZero = TRUE;
	m_SymbolRate = SYMBOL_RATE31;

	//	i = 0;
	//	while(i<32)		/*create post/preamble tables */
	//	{
	//		m_Preamble[i] = TXTOG_CODE;
	//		m_Postamble[i++] = TXON_CODE;
	//	}
	//	m_Preamble[i] = 0;		/* null terminate these tables */
	//	m_Postamble[i] = 0;

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
		for (int i = 0; i < m_pTail; i++) XmitBuffer[i] = XmitBuffer[i+1]; //shift contents left one character
		for(int i = m_pTail; i < PSK_TX_BUFFER_SIZE - 2; i++) XmitBuffer[i] = 32; //put spaces in remaining empty spaces
		m_pTail--;

		// LCD code is not thread safe, so trigger a delayed event to display the XMitBuffer to the LCD.
		DelayEvent_TriggerEvent(DelayEvent_DisplayStoreFreq);
	}
	else
		ch = TXTOG_CODE;		/* if que is empty return TXTOG_CODE */

	return ch;
}

// Queue a new character for transmission.
// Returns 1 if succeeded, 0 if failed (buffer full)
_Bool PSK_addCharToTx(char newChar)
{
	// Queue full?
	if (PSK_isQueueFull()) {
		return 0;
	}

	// Queue the character
	XmitBuffer[m_pTail++] = newChar;
	//m_pHead %= PSK_TX_BUFFER_SIZE;
	LCD_StringLine(0, 90, (char*) &XmitBuffer[0]);

	// Success
	return 1;
}




_Bool PSK_isQueueFull(void)
{
	//int nextHead = (m_pHead + 1) % PSK_TX_BUFFER_SIZE;
	return 38 == m_pTail;//if  38 == m_pTail then return 1 else return 0
}


_Bool PSK_isQueueEmpty(void)
{
	return 0 == m_pTail;
}
