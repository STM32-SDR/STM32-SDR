/*
 * PSKMod.h
 *
 *  Created on: Apr 12, 2013
 *      Author: CharleyK
 */

#ifndef PSKMOD_H_
#define PSKMOD_H_

#endif /* PSKMOD_H_ */
/*==================================================================*/
/* PSKMod.h: interface for the CPSKMod class.			    		*/
/*==================================================================*/
/*  Modified by Milt Cram, W8NUE				    				*/
/*==================================================================*/
/* PSK31Core Library for transmission and reception of PSK31	    */
/* signals using a PC soundcard  or .wav files.			    		*/
/*	               Copyright 2000, Moe Wheatley, AE4JY	    		*/
/*								    								*/
/*This library is free software; you can redistribute it and/or	    */
/*modify it under the terms of the GNU Lesser General Public	    */
/*License as published by the Free Software Foundation; either	    */
/*version 2.1 of the License, or (at your option) any later version.*/
/*								    								*/
/*This library is distributed in the hope that it will be useful,   */
/*but WITHOUT ANY WARRANTY; without even the implied warranty of    */
/*MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU */
/*Lesser General Public License for more details.		    		*/
/*								    								*/
/*You should have received a copy of the GNU Lesser General Public  */
/*License along with this library; if not, write to the Free 	    */
/* Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,   */
/* MA  02111-1307  USA 						    					*/
/* http://www.gnu.org/copyleft/lesser.html			    			*/
/*==================================================================*/

int m_Ramp;
int m_pTail;
int m_pHead;
char GetTxChar(void);
char XmitBuffer[26];

/* PSK31  generator variables */
int m_AddEndingZero;
uint16_t m_TxShiftReg;
double m_SymbolRate;

float m_RMSConstant;
double m_2PI;
float S1, S2;

/* PSK31 modulator private functions */
char GetChar(void);
char GetNextBPSKSymbol(void);

/* PSK31 modulator */

void Update_PSK(void);
void InitPSKModulator(void);

