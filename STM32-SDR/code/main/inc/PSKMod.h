/* PSK code
 Based on code by Moe Weatley; changes may have been made
 for the STM32-SDR project. Original license below:

     ////////////////////////////////////////////////////////////////////
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
	////////////////////////////////////////////////////////////////////
*/

#ifndef PSKMOD_H_
#define PSKMOD_H_

#include <stdint.h>

// Exported for display in the ISR.
extern char XmitBuffer[];

void ClearXmitBuffer(void);
char GetTxChar(void);

extern float S1, S2;

/* PSK31 modulator private functions */
char GetChar(void);
char GetNextBPSKSymbol(void);

/* PSK31 modulator */

void Update_PSK(void);
void InitPSKModulator(void);

// Support for transmitting characters and the Tx buffer
_Bool PSK_addCharToTx(char newTxChar);
_Bool PSK_isQueueFull(void);
_Bool PSK_isQueueEmpty(void);


#endif /* PSKMOD_H_ */
