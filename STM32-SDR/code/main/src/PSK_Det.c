
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
#include "PSKDet.h"
#include "PSKMod.h"
#include "Psk_Tables.h"
#include "arm_math.h"
#include "DSP_Processing.h"
#include  "uart.h"

/* define some constants  */
#define PHZ_180_BMIN	(0.0)			/* 0         */
#define PHZ_180_BMAX	(PI2/2.0)		/* Pi        */
#define PHZ_0_BMIN	(PI2/2.0)		/* Pi        */
#define PHZ_0_BMAX	(PI2)			/* 2Pi       */
#define SYMBOL_RATE31 	31.25	/* 31.25 Symbols per Second */
#define Ts (.032)			/* Ts == symbol period */

#define AFC_OFF 0
#define AFC_ON 1
#define AFC_TIMELIMIT 40	/* 2.5 seconds	*/
//#define AFC_TIMELIMIT 20	/* 2.5 seconds	*/ chh sept
#define AFC_FTIMELIMIT 4	/* 0.5 seconds	*/

#define NLP_K (100.0)		/*narrow phase derived afc constans  */
#define FNLP_K (30.0)

/* variables */
int m_FastAFCMode;
int m_AFCCaptureOn;
int m_IMDValid;
int m_SQOpen;
int m_LastBitZero;

unsigned char m_VaricodeDecTbl[2048];
int m_BitAcc;
long m_IQPhaseArray[20];
long m_SyncArray[20];
int m_AFCTimer;
int m_AFCmode;
int m_PSKmode;
int m_SampleClkAdj;
int m_IQPhzIndex;
int m_SquelchSpeed;
int m_SQLevel;
int m_SQThresh;
int m_ClkErrTimer;
int m_ClkErrCounter;
int m_ClkError;
int m_LastPkPos;
int m_OnCount;
int m_OffCount;
int m_SampCnt;
double m_FreqError;
double m_DevAve;
double m_I0; /* 4 stage I/Q delay line variables */
double m_I1;
double m_Q0;
double m_Q1;
double m_BitPhaseInc;
double m_BitPhasePos;
double m_SyncAve[21];
double m_NCOphzinc;
double m_SampleFreq;
double m_AFClimit;
double m_AFCmax;
double m_AFCmin;
double m_NLPk;

struct Complex m_FreqSignal;
struct Complex m_BitSignal;

/* Local variables for various functions that need to be saved between calls */
int m_PkPos;
int m_NewPkPos;
int m_BitPos;
int m_Pcnt;
int m_Ncnt;
double m_AGCave;
double m_FperrAve;
double m_FferrAve;
double m_QFreqError;
double m_VcoPhz;
struct Complex m_z1;
struct Complex m_z2;

//unsigned char LCD_buffer[80];
unsigned char NewChar;
int count;
int char_count;

double m_PSKPeriodUpdate;
double m_SymbolRate;

extern double NCO_2;

/* //////////////////////////////////////////////////////////////////// */
/* Construction/Destruction                                             */
/* //////////////////////////////////////////////////////////////////// */

void ResetModem(int mode)
{
//	int i;

//	for (i = 0; i < 40; i++) {
//		LCD_buffer[i] = ' ';
//	}
//	LCD_buffer[39] = '\0';

	count = 0;
	char_count = 0;

	InitPSK(8000);
	CPSKInitDet();
	InitPSKModulator();

	}
/* //////////////////////////////////////////////////////////////////// */

void CPSKInitDet(void)
{
	double NCO_Frequency = 2000;  //chh temporary test value
	float Sample_Frequency = 8000.0;
	m_SampleClkAdj = 0;
	m_IQPhzIndex = 0;
	m_IMDValid = FALSE;
	m_FreqError = 0.0;
	m_SquelchSpeed = 75;
	m_SQThresh = 50;
	m_NCOphzinc = (PI2 * (double) NCO_Frequency / (double) Sample_Frequency);
	m_AFClimit = 50.0 * PI2 / (double) Sample_Frequency;
	m_AFCmax = m_NCOphzinc + m_AFClimit;
	m_AFCmin = m_NCOphzinc - m_AFClimit;
	SetSampleClkAdj(0);
}

/* ///////////////////////////////////////////////////////////////  */
/*       Initialize PskDet buffers and pointers                     */
/*/////////////////////////////////////////////////////////////     */
void InitPSK(int Fs)
{
	unsigned int wTemp;
	int i;
	extern float Sample_Frequency;
	int j;
	Sample_Frequency = Fs;
	for (j = 0; j < 2048; j++) /* init inverse varicode lookup decoder table */
	{
		m_VaricodeDecTbl[j] = 0;
		for (i = 0; i < 256; i++) {
			wTemp = VARICODE_TABLE[i];
			wTemp >>= 4;
			while (!(wTemp & 1))
				wTemp >>= 1;
			wTemp >>= 1;
			if (wTemp == j)
				m_VaricodeDecTbl[j] = (unsigned char) i;
		}
	}

	ResetDetector();
	SetSampleClkAdj(0);

	m_SQLevel = 50;
	m_BitPhasePos = 0.0;
	m_BitAcc = 0;
	m_LastBitZero = FALSE;
	m_SampCnt = 0;
	m_OnCount = 0;
	m_OffCount = 0;

	/* Init a bunch of "static" variables used in various member functions */
	m_AGCave = 0.0;
	m_VcoPhz = 0.0;
	m_PkPos = 0;
	m_NewPkPos = 5;
	m_BitPos = 0;
	m_I1 = 0.0;
	m_I0 = 0.0;
	m_Q1 = 0.0;
	m_Q0 = 0.0;
	m_DevAve = .4;
	m_z1.x = 0.0;
	m_z1.y = 0.0;
	m_z2.x = 0.0;
	m_z2.y = 0.0;
	m_FferrAve = 0.0;
	m_FferrAve = 0.0;
	m_QFreqError = 0.0;
	m_LastPkPos = 0;
	m_SQOpen = FALSE;
	m_AFCTimer = 0;
	//m_AFCCaptureOn = FALSE;
	m_AFCCaptureOn = TRUE;
	m_FastAFCMode = FALSE;
	m_NLPk = NLP_K;

}

/* ////////////////////////////////////////////////////////////////////  */
/* Called to adjust the sample clock                                     */
/* ////////////////////////////////////////////////////////////////////  */
void SetSampleClkAdj(int ppm)
{
	extern float Sample_Frequency;
	extern double NCO_Frequency;  //chh added extern
	m_SampleClkAdj = (Sample_Frequency * ppm) / 1000000;
	m_SampleFreq = (double) (Sample_Frequency + m_SampleClkAdj); /*adj sample rate */
	m_NCOphzinc = (double) (PI2 / m_SampleFreq) * (double) NCO_Frequency; /*new center freq inc */
	m_BitPhaseInc = 16.0 / m_SampleFreq; /*bit oversampling period */
}

/* ////////////////////////////////////////////////////////////////////  */
/* Called to change the Rx frequency                                     */
/* ////////////////////////////////////////////////////////////////////  */
void SetRXFrequency(double freq)
{
	extern double m_SampleFreq, m_NCOphzinc;

	m_AFCCaptureOn = TRUE;
	m_NCOphzinc = PI2 * freq / m_SampleFreq;
	m_FferrAve = 0.0;
	m_FperrAve = 0.0;
	if (m_FastAFCMode)
		m_AFCTimer = AFC_FTIMELIMIT;
	else
		m_AFCTimer = AFC_TIMELIMIT; /* 2.5 sec countdown timer for AFC */
	/* calculate new limits around new receive frequency                     */
	m_AFCmax = m_NCOphzinc + m_AFClimit;
	m_AFCmin = m_NCOphzinc - m_AFClimit;
	if (m_AFCmin <= 0.0)
		m_AFCmin = 0.0;
	m_Pcnt = 0;
	m_Ncnt = 0;
}

/* ////////////////////////////////////////////////////////////////////  */
/* Called to change the AFC limit                                        */
/* ////////////////////////////////////////////////////////////////////  */
void SetAFCLimit(int limit)
{
	if (limit == 0)
		m_AFCmode = AFC_OFF;
	else
		m_AFCmode = AFC_ON;
	if (limit == 3000)
		m_FastAFCMode = TRUE;
	else
		m_FastAFCMode = FALSE;
	m_AFClimit = (double) limit * PI2 / m_SampleFreq;
	/* calculate new limits around current receive frequency                 */
	m_AFCmax = m_NCOphzinc + m_FreqError + m_AFClimit;
	m_AFCmin = m_NCOphzinc + m_FreqError - m_AFClimit;
	if (m_AFCmin <= 0.0)
		m_AFCmin = 0.0;
	if (m_FastAFCMode) {
		m_NLPk = FNLP_K;
	}
	else {
		m_NLPk = NLP_K;
	}
}

/* ////////////////////////////////////////////////////////////////////  */
/* Called to reset the detector before going into xmit                   */
/* ////////////////////////////////////////////////////////////////////  */
void ResetDetector(void)
{
	int i;

	for (i = 0; i < 16; i++) {
		m_IQPhaseArray[i] = 1;
	}
	for (i = 0; i < 21; i++) {
		m_SyncAve[i] = 0.0; /* initialize the array */
	}
	m_SQLevel = 10;
	m_ClkErrCounter = 0;
	m_ClkErrTimer = 0;
	m_ClkError = 0;
	m_DevAve = 90.0;
}

/* ////////////////////////////////////////////////////////////////////  */
/* Main routine called to process the next block of data                 */
/*  We will use a block of length ADC_BUFFER_LENGTH						 */
/* ////////////////////////////////////////////////////////////////////  */
void ProcPSKDet()
{
	extern float Sample_Frequency;
	extern double NCO_Frequency;
	extern double m_NCOphzinc;
	int i;
	//int  j =0;
	static long NCO_phz;
	static long Product_I, Product_Q;

	uint32_t BlockSize = 4;

	static q15_t FIR_sample_I, FIR_sample_Q, FIR_Output_I, FIR_Output_Q, NCO_I,
	NCO_Q, ADC_I, temp_I, temp_Q, FIROut_I_bit, FIROut_Q_bit,
	FIROut_I_freq, FIROut_Q_freq;

	//I dont know why but I had to add the q15_t reference below to get it to compile
	q15_t Moe_35_sample_I_1[4], Moe_35_sample_I_2[4], Moe_35_sample_Q_1[4],
	Moe_35_sample_Q_2[4];

	extern double m_NCOphzinc, m_FreqError;
	static int Index_1, Index_2;

	/*  AFC timer initialized in SetRXFrequency, decremented here at */
	/*	.125ms * ADC_BUFFER_LENGTH intervals, (128msec interval) 	*/

	if (m_AFCTimer) {
		if (--m_AFCTimer <= 0) {
			m_AFCTimer = 0;
			m_AFCCaptureOn = FALSE;

			/* calculate new limits around latest receive frequency   */
			m_AFCmax = m_NCOphzinc + m_AFClimit;
			m_AFCmin = m_NCOphzinc - m_AFClimit;
			NCO_Frequency = m_NCOphzinc * (double) Sample_Frequency / PI2;
			NCO_2 = NCO_Frequency;
			if (m_AFCmin <= 0.0)
				m_AFCmin = 0.0;
		}
		else
			m_AFCCaptureOn = TRUE;
	}

	/* Process a block of input data */
	for (i = 0; i < BUFFERSIZE / 2; i++) {

		ADC_I = ADC_Buffer[i];

		NCO_phz += (long) (KCONV * (m_NCOphzinc + m_FreqError));

		NCO_Q = (Sine_table[(NCO_phz >> 4) & 0xFFF]);
		NCO_I = (Sine_table[((NCO_phz >> 4) + 0x400) & 0xFFF]);

		Product_I = (long) ADC_I * (long) NCO_I;
		Product_Q = (long) ADC_I * (long) NCO_Q;

		FIR_sample_I = (int) (Product_I >> 15);
		FIR_sample_Q = (int) (Product_Q >> 15);

		/*----------------------------------------------------*/

		Moe_35_sample_I_1[Index_1] = FIR_sample_I;
		Moe_35_sample_Q_1[Index_1] = FIR_sample_Q;

		Index_1 = (Index_1 + 1) % 4;

		if (!Index_1) {

			arm_fir_decimate_q15(&Filter_I1, &Moe_35_sample_I_1[0],
					&Moe_35_sample_I_2[Index_2], BlockSize);

			arm_fir_decimate_q15(&Filter_Q1, &Moe_35_sample_Q_1[0],
					&Moe_35_sample_Q_2[Index_2], BlockSize);

			Index_2 = (Index_2 + 1) % 4;

			if (!Index_2) {

				arm_fir_decimate_q15(&Filter_I2, &Moe_35_sample_I_2[0],
						&FIR_Output_I, BlockSize);

				arm_fir_decimate_q15(&Filter_Q2, &Moe_35_sample_Q_2[0],
						&FIR_Output_Q, BlockSize);

				temp_I = FIR_Output_I;
				temp_Q = FIR_Output_Q;

				arm_fir_q15(&Filter_I3, &FIR_Output_I, &FIROut_I_bit, 1);

				arm_fir_q15(&Filter_Q3, &FIR_Output_Q, &FIROut_Q_bit, 1);

				arm_fir_q15(&Filter_I4, &temp_I, &FIROut_I_freq, 1);

				arm_fir_q15(&Filter_Q4, &temp_Q, &FIROut_Q_freq, 1);

				/* here at Fs/16 == 500.0 Hz or 1000.0 Hz rate with latest sample in acc.  */
				/* Matched Filter the I and Q data and also a frequency error filter       */
				/*	filter outputs in variables m_FreqSignal and m_BitSignal.  */

				m_FreqSignal.x = FIROut_I_freq;
				m_FreqSignal.y = FIROut_Q_freq;
				m_BitSignal.x = FIROut_I_bit;
				m_BitSignal.y = FIROut_Q_bit;

				/* Perform AGC operation  */
				CalcAGC(m_FreqSignal);

				/* Calculate frequency error and tweak NCO */
				if (m_FastAFCMode)
					CalcFFreqError(m_FreqSignal);
				else
					CalcFreqError(m_FreqSignal);

				/* Bit Timing synchronization  */
				if (SymbSync(m_BitSignal))
					DecodeSymb(m_BitSignal);
			}
		}
	}

}

/* ////////////////////////////////////////////////////////////////////  */
/*  Frequency error calculator for fast AFC satellite mode               */
/* returns frequency error for mainNCO.                                  */
/* ////////////////////////////////////////////////////////////////////  */
void CalcFFreqError(struct Complex IQ)
{
#define FP_GN 0.008
#define FI_GN 3.0E-5
#define FP_CGN 0.002
#define FI_CGN 1.50E-5
#define FWIDE_GN (1.0/.02)			/*gain to make error in Hz  */
#define FWLP_K (300.0)
	extern double m_FferrAve;
	double freqerr;

	if (m_AFCmode == AFC_OFF) {
		m_FferrAve = 0.0;
		m_FperrAve = 0.0;
		m_FreqError = 0.0;
		return;
	}

	freqerr = (IQ.x - m_z2.x) * m_z1.y - (IQ.y - m_z2.y) * m_z1.x;
	m_z2.x = m_z1.x;
	m_z2.y = m_z1.y;
	m_z1.x = IQ.x;
	m_z1.y = IQ.y;
	/* error at this point is abt .02 per Hz error  */
	if (freqerr > .30) /*clamp range  */
		freqerr = .30;
	if (freqerr < -.30)
		freqerr = -.30;
	m_FferrAve = (1.0 - 1.0 / FWLP_K) * m_FferrAve
			+ ((1.0 * FWIDE_GN)/FWLP_K) * freqerr;
	freqerr = m_FferrAve; /* error is now approximately in Hertz */
	if ((freqerr > 6.0) || (freqerr < -6.0)) {
		m_NCOphzinc = m_NCOphzinc + (freqerr * FI_CGN);
		/*		m_FreqError = freqerr*FP_CGN;   chh this looks suspicious  */
	}
	else {
		if ((m_FferrAve * m_FperrAve) > 0.0) /*make sure both errors agree */
			freqerr = m_FperrAve;
		else
			freqerr = 0.0;
		if ((freqerr > 0.3) || (freqerr < -0.3))
			m_NCOphzinc = m_NCOphzinc + (freqerr * FI_GN);
		/*		m_FreqError = freqerr*FP_GN; */
	}
	/*clamp frequency within range */
	if ((m_NCOphzinc + m_FreqError) > m_AFCmax) {
		m_NCOphzinc = m_AFCmax;
		m_FreqError = 0.0;
	}
	else if ((m_NCOphzinc + m_FreqError) < m_AFCmin) {
		m_NCOphzinc = m_AFCmin;
		m_FreqError = 0.0;
	}
}

/* ////////////////////////////////////////////////////////////////////  */
/*  Frequency error calculator                                           */
/* calculates the derivative of the tan(I/Q).                            */
/* returns frequency error for mainNCO.                                  */
/* ////////////////////////////////////////////////////////////////////  */
void CalcFreqError(struct Complex IQ)
{
#define P_GN 0.001			/*AFC constants  */
#define I_GN 1.5E-6
#define P_CGN 0.0004
#define I_CGN 3.0E-6
#define WIDE_GN (1.0/.02)			/*gain to make error in Hz  */
#define WLP_K (200.0)
	extern double m_FferrAve;
	double freqerr;

	if (m_AFCmode == AFC_OFF) {
		m_FferrAve = 0.0;
		m_FperrAve = 0.0;
		m_FreqError = 0.0;
		return;
	}

	freqerr = (IQ.x - m_z2.x) * m_z1.y - (IQ.y - m_z2.y) * m_z1.x;
	m_z2.x = m_z1.x;
	m_z2.y = m_z1.y;
	m_z1.x = IQ.x;
	m_z1.y = IQ.y;
	/* error at this point is abt .02 per Hz error  */
	if (freqerr > .30) /*clamp range  */
		freqerr = .30;
	if (freqerr < -.30)
		freqerr = -.30;
	m_FferrAve = (1.0 - 1.0 / WLP_K) * m_FferrAve
			+ ((1.0 * WIDE_GN)/WLP_K) * freqerr;
	/* error is now approximately in Hz */

	if (m_AFCCaptureOn) {
		freqerr = m_FferrAve;
		if ((freqerr > 0.3) || (freqerr < -0.3))
			m_NCOphzinc = m_NCOphzinc + (freqerr * I_CGN);
		m_FreqError = freqerr * P_CGN;

	}
	else {
		if ((m_FferrAve * m_FperrAve) > 0.0)
			freqerr = m_FperrAve;
		else
			freqerr = 0.0;
		if ((freqerr > 0.3) || (freqerr < -0.3))
			m_NCOphzinc = m_NCOphzinc + (freqerr * I_GN);
		m_FreqError = freqerr * P_GN;

	}

	/*clamp frequency within range  */
	if ((m_NCOphzinc + m_FreqError) > m_AFCmax) {
		m_NCOphzinc = m_AFCmax;
		m_FreqError = 0.0;
	}
	else if ((m_NCOphzinc + m_FreqError) < m_AFCmin) {
		m_NCOphzinc = m_AFCmin;
		m_FreqError = 0.0;
	}

}

/* ////////////////////////////////////////////////////////////////////  */
/* Automatic gain control calculator                                     */
/* ////////////////////////////////////////////////////////////////////  */
void CalcAGC(struct Complex Samp)
{
	double mag;

	mag = sqrt(Samp.x * Samp.x + Samp.y * Samp.y);

	if (mag > m_AGCave)
		m_AGCave = (1.0 - 1.0 / 200.0) * m_AGCave + (1.0 / 200.0) * mag;
	else
		m_AGCave = (1.0 - 1.0 / 500.0) * m_AGCave + (1.0 / 500.0) * mag;

	if (m_AGCave >= 1.0) /* divide signal by ave if not almost zero */
	{
		m_BitSignal.x /= m_AGCave;
		m_BitSignal.y /= m_AGCave;
		m_FreqSignal.x /= m_AGCave;
		m_FreqSignal.y /= m_AGCave;
	}

}

/* ////////////////////////////////////////////////////////////////////  */
/* Called at Fs/16 rate to calculate the symbol sync position            */
/* Returns TRUE if at center of symbol.                                  */
/* Sums up the energy at each sample time, averages it, and picks the    */
/*   sample time with the highest energy content.                        */
/* ////////////////////////////////////////////////////////////////////  */
int SymbSync(struct Complex sample)
{
	int Trigger = FALSE;
	double max;
	double energy;
	int BitPos = m_BitPos;
	int i;
	if (BitPos < 16) {
		energy = (sample.x * sample.x) + (sample.y * sample.y);
		if (energy > 4.0) /*wait for AGC to settle down */
			energy = 1.0;
		m_SyncAve[BitPos] = (1.0 - 1.0 / 82.0) * m_SyncAve[BitPos]
		                                                   + (1.0 / 82.0) * energy;
		if (BitPos == m_PkPos) /* see if at middle of symbol */
		{
			Trigger = TRUE;
			m_SyncArray[m_PkPos] = (int) (900.0 * m_SyncAve[m_PkPos]);
		}
		else {
			Trigger = FALSE;
			m_SyncArray[BitPos] = (int) (750.0 * m_SyncAve[BitPos]);
		}
		if (BitPos == HALF_TBL[m_NewPkPos]) /*don't change pk pos until */
			m_PkPos = m_NewPkPos; /* halfway into next bit. */
		BitPos++;
	}

	m_BitPhasePos += (m_BitPhaseInc);
	if (m_BitPhasePos >= Ts) { /* here every symbol time */
		m_BitPhasePos = fmod(m_BitPhasePos, Ts); /*keep phase bounded */
		if ((BitPos == 15) && (m_PkPos == 15)) /*if missed the 15 bin before rollover */
			Trigger = TRUE;
		BitPos = 0;
		max = -1e10;

		for (i = 0; i < 16; i++) /*find maximum energy pk */
		{
			energy = m_SyncAve[i];
			if (energy > max) {
				m_NewPkPos = i;
				max = energy;
			}
		}
		if (m_SQOpen) {
			if (m_PkPos == m_LastPkPos + 1) /*calculate clock error */
				m_ClkErrCounter++;
			else if (m_PkPos == m_LastPkPos - 1)
				m_ClkErrCounter--;
			if (m_ClkErrTimer++ > 313) /* every 10 seconds sample clk drift */
			{
				m_ClkError = m_ClkErrCounter * 200; /*each count is 200ppm */
				m_ClkErrCounter = 0;
				m_ClkErrTimer = 0;
			}
		}
		else {
			m_ClkError = 0;
			m_ClkErrCounter = 0;
			m_ClkErrTimer = 0;
		}
		m_LastPkPos = m_PkPos;
	}
	m_BitPos = BitPos;
	return Trigger;
}

/* ////////////////////////////////////////////////////////////////////  */
/*  Decode the new symbol sample                                         */
/* ////////////////////////////////////////////////////////////////////  */

struct Complex vect;
void DecodeSymb(struct Complex newsamp)
{

	double angle;
	double energy;
	struct Complex vect;
	unsigned char ch = 0;

	int bit;
	//static int i;

	int GotChar = FALSE;

	m_I1 = m_I0; /*form the multi delayed symbol samples  */
	m_Q1 = m_Q0;
	m_I0 = newsamp.x;
	m_Q0 = newsamp.y;

	/* calculate difference angle for QPSK, BPSK, and IQPSK decoding  */
	/* create vector whose angle is the difference angle by multiplying the */
	/* current sample by the complex conjugate of the previous sample. */
	/* swap I and Q axis to keep away from  the +/-Pi discontinuity and */
	/*  add Pi to make make range from 0 to 2Pi. */
	/* 180 deg phase changes center at Pi/4 */
	/* 0 deg phase changes center at 3Pi/2 */
	/* +90 deg phase changes center at 2Pi or 0 */
	/* -90 deg phase changes center at Pi */
	/*  if using lower sideband must flip sign of difference angle. */
	/*                                                              */
	/* first calculate normalized vectors for vector display        */
	vect.y = (m_I1 * m_I0 + m_Q1 * m_Q0);
	vect.x = (m_I1 * m_Q0 - m_I0 * m_Q1);
	energy = sqrt(vect.x * vect.x + vect.y * vect.y) / 1.0E3;

	if (m_AGCave > 10.0) {
		m_IQPhaseArray[m_IQPhzIndex++] = (long) (vect.x / energy);
		m_IQPhaseArray[m_IQPhzIndex++] = (long) (vect.y / energy);
	}
	else {
		m_IQPhaseArray[m_IQPhzIndex++] = 2;
		m_IQPhaseArray[m_IQPhzIndex++] = 2;
	}
	m_IQPhzIndex &= 0x000F; /*mod 16 index */

	angle = (PI2 / 2) + atan2(vect.y, vect.x); /*QPSK upper sideband or BPSK */
	CalcQuality(angle);

	/*calc BPSK symbol over 2 chips */
	vect.y = m_I1 * m_I0 + m_Q1 * m_Q0;
	bit = (int) (vect.y > 0.0);

	if ((bit == 0) && m_LastBitZero) /*if character delimiter */
	{
		if (m_BitAcc != 0) {
			m_BitAcc >>= 2; /*get rid of last zero and one */
			m_BitAcc &= 0x07FF;
			ch = m_VaricodeDecTbl[m_BitAcc];
			m_BitAcc = 0;
			GotChar = TRUE;
		}
	}
	else {
		m_BitAcc <<= 1;
		m_BitAcc |= bit;
		if (bit == 0)
			m_LastBitZero = TRUE;
		else
			m_LastBitZero = FALSE;
	}

	if (GotChar && (ch != 0) && m_SQOpen) {
		NewChar = ch;
		GotChar = FALSE;
	}

}

/* ////////////////////////////////////////////////////////////////////  */
/* Calculate signal quality based on the statistics of the phase         */
/*	difference angle.  The more dispersion of the "0" and "180" degree   */
/*  phase shifts, the worse the signal quality.  This information is used  */
/*  to activate the squelch control.  If 20 consecutive "180" degree shifts */
/*  occur, the squelch is forced on, and if 20 consecutive "0" degree    */
/*  shifts occur, the squelch is forced off quickly.                     */
/* ////////////////////////////////////////////////////////////////////  */
void CalcQuality(double angle)
{
#define ELIMIT 5
#define PHZDERIVED_GN (1.0/.2)		/*gain to make error in Hz */
	double temp;
	double SqTimeK;
	SqTimeK = (double) m_SquelchSpeed;
	if (!m_PSKmode && ((angle >= PHZ_180_BMIN)&& (angle <= PHZ_180_BMAX))) {
		temp = angle - PI2/4.0;
		m_QFreqError = temp;

		temp = 150.0*fabs(temp);
		if(temp < m_DevAve)
			m_DevAve= (1.0-1.0/SqTimeK)*m_DevAve + (1.0/SqTimeK)*temp;
		else
			m_DevAve= (1.0-1.0/(SqTimeK*2.0))*m_DevAve + (1.0/(SqTimeK*2.0))*temp;

		if(m_OnCount > 20 ) /* fast squelch counter */
			m_DevAve = 100.0-75.0; /*set to 75% */
		else
			m_OnCount++;
		m_OffCount = 0;

		if( m_QFreqError >= 0.0 ) {
			m_Pcnt++;
			m_Ncnt = 0;
		}
		else {
			m_Ncnt++;
			m_Pcnt = 0;
		}

		if( (m_Pcnt<ELIMIT) && (m_Ncnt<ELIMIT) )
			m_QFreqError = 0.0;
	}
	else
	{
		if(!m_PSKmode && ((angle >= PHZ_0_BMIN) && (angle <= PHZ_0_BMAX) ) )
		{
			temp = angle - 3*PI2/4.0;
			m_QFreqError = temp;

			temp = 150.0*fabs(temp);

			if( temp < m_DevAve)
				m_DevAve=  (1.0-1.0/SqTimeK)*m_DevAve + (1.0/SqTimeK)*temp;
			else
				m_DevAve=  (1.0-1.0/(SqTimeK*2.0))*m_DevAve + (1.0/(SqTimeK*2.0))*temp;

			if((m_OffCount > 20) ) {	/* fast squelch counter */
				m_DevAve = 100.0 - 0.0;		/*set to 0% */
			}
			else m_OffCount++;
			m_OnCount = 0;
			if( m_QFreqError >= 0.0 )
			{
				m_Pcnt++;
				m_Ncnt = 0;
			}
			else
			{
				m_Ncnt++;
				m_Pcnt = 0;
			}
			if( (m_Pcnt<ELIMIT) && (m_Ncnt<ELIMIT) )
				m_QFreqError = 0.0;
		}
	}

	if (m_OnCount > 2)
		m_IMDValid = TRUE;
	else
		m_IMDValid = FALSE;

	if (m_AGCave > 10.0) {
		if (m_PSKmode) /*if QPSK */
			m_SQLevel = 100 - (int) m_DevAve;
		else
			m_SQLevel = 100 - (int) m_DevAve;

		if (m_SQLevel >= m_SQThresh)
			m_SQOpen = TRUE;
		else
			m_SQOpen = FALSE;
	}
	else {
		m_SQLevel = 0;
		m_SQOpen = FALSE;
	}
	if (m_PSKmode) {
		if (m_QFreqError > .6)/*  clamp range to +/- 3 Hz */
			m_QFreqError = .6;
		if (m_QFreqError < -.6)
			m_QFreqError = -.6;
	}
	else {
		if (m_QFreqError > 1.0)/*  clamp range to +/- 5 Hz */
			m_QFreqError = 1.0;
		if (m_QFreqError < -1.0)
			m_QFreqError = -1.0;
	}
	m_FperrAve = (1.0 - 1.0 / m_NLPk) * m_FperrAve
			+ ((1.0 * PHZDERIVED_GN)/m_NLPk) * m_QFreqError;

}

