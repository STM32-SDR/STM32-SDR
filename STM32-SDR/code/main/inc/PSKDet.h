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

#ifndef PSKDET_H_
#define PSKDET_H_

#include	"arm_math.h"

//extern unsigned char LCD_buffer[80];

//Stuff Imported From NUE_PSK_Header.h
typedef struct
{
	unsigned F0_char :1;
	unsigned E0_char :1;
	unsigned Shift_key :1;
	unsigned Alt_key :1;
	unsigned Ctrl_key :1;
	unsigned Bailout :1;
	unsigned RXTune :1;
	unsigned DispFreq :1;
	unsigned E1_char :1;
	unsigned ProcFFT :1;
	unsigned TglRnT :1;
	unsigned ProcPSK :1;
	unsigned TglTune :1;
	unsigned DAV :1;
	unsigned Setup :1;
	unsigned MacRec :1;
} Flag_type;

typedef int fractional;

typedef struct
{
	fractional real;
	fractional imag;
} fractcomplex;

extern const int Sine_table[4096];

float Sample_Frequency;
Flag_type Flag;

double NCO_Frequency;

#define PI2 6.2831853071795864765
#define KCONV 10430.37835              // 		4096*16/PI2
#define SYMBOL_RATE31 	31.25	/* 31.25 Symbols per Second */
//barrier
//Complex number   //
struct Complex
{
	double x;	// Real part
	double y;	// Imaginary part
};

#define ADC_BUFFER_LENGTH 1024
#define BPSK_MODE 0
#define TRUE 1
#define FALSE 0

#define SQMODEFAST 0
#define SQMODESLOW 1

/* methods */

struct Complex Samp;
struct Complex newsamp;
struct Complex sample;
struct Complex IQ;
void CalcAGC(struct Complex Samp);
void DecodeSymb(struct Complex newsamp);
void CalcQuality(double angle);
int SymbSync(struct Complex sample);
void CalcFreqError(struct Complex IQ);
void CalcFFreqError(struct Complex IQ);

extern int m_SQOpen;

void InitPSK(int Fs);
void ProcPSKDet(void);
void ResetModem(int mode);
void ResetDetector(void);
void CPSKInitDet(void);
void SetRXFrequency(double freq);

void SetAFCLimit(int limit);
void SetSampleClkAdj(int ppm);

#endif /* PSK_DET_H_ */
