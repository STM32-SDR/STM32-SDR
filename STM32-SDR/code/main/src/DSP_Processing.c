/*
 * Core DSP processing code
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

#include	"Init_DMA.h"
#include	"arm_math.h"
#include	"FIR_Coefficients.h"
#include	"DSP_Processing.h"

float 	Point_Mag;
float 	Peak_Mag;
float	Total_Mag;
float	Avg_Mag;
float	RMS_Sig;
float	RMS_Sig2;
float	dB_Sig;
float	dB_Sig2;
float	RMS_Sig_Sum;
float	dB_Sig_Sum;
float	RMS_Mag;
float	RMS_Mag2;
float	dB_RSL;
float	dB_RSL2;
float	Sig_Mag;
float	Sig_RSL;

int 	Point_RSL;
int 	Peak_RSL;
int		Total_RSL;
int		Avg_RSL;
int		Sig;
int		Sig_Max;
int		Sig2;
int		Sig_Sum;

int		AGC_Scale;
int 	number_bins;
float 	AGC_Multiplier;

float 	FFT_Coeff = 0.2;
extern  int NCO_Bin;

float32_t FFT_Output[256];
float32_t FFT_Filter[256];

q15_t FFT_Input[1024];
q15_t FFT_Scale[1024]; //512 sampling
q15_t FFT_Magnitude[512]; //512 sampling

int FFT_Mag_10[256];

uint16_t FFT_Size = 512;  // change for 512 sampling
uint8_t FFT_status;

q15_t FIR_State_I[NUM_FIR_COEF + (BUFFERSIZE / 2) - 1];
q15_t FIR_State_Q[NUM_FIR_COEF + (BUFFERSIZE / 2) - 1];
q15_t FIR_I_In[BUFFERSIZE / 2];
q15_t FIR_Q_In[BUFFERSIZE / 2];
q15_t FIR_I_Out[BUFFERSIZE / 2];
q15_t FIR_Q_Out[BUFFERSIZE / 2];
q15_t USB_Out[BUFFERSIZE / 2];
q15_t LSB_Out[BUFFERSIZE / 2];

//chh Filter Definitions imported from NUE PSK
q15_t Filter_delay_I1[38]; //38 = 35 + 4 -1
q15_t Filter_delay_I2[38];
q15_t Filter_delay_Q1[38];
q15_t Filter_delay_Q2[38];
q15_t Filter_delay_I3[65]; //65 = 65 + 1 -1
q15_t Filter_delay_Q3[65];
q15_t Filter_delay_I4[65];
q15_t Filter_delay_Q4[65];
//chh Filter Definitions imported from NUE PSK



q15_t ADC_Buffer[BUFFERSIZE / 2];  //for 1024 sampling

arm_cfft_radix2_instance_q15 S_CFFT;

void init_DSP(void) {
	FFT_status = arm_cfft_radix2_init_q15(&S_CFFT, FFT_Size, 0, 1);
}

arm_fir_instance_q15 S_FIR_I = { NUM_FIR_COEF, &FIR_State_I[0], &coeff_fir_I[0] };
arm_fir_instance_q15 S_FIR_Q = { NUM_FIR_COEF, &FIR_State_Q[0], &coeff_fir_Q[0] };

//chh PSK Filters
arm_fir_decimate_instance_q15 Filter_I1 = { 4, 35, &Dec4LPCoef[0], &Filter_delay_I1[0] };
arm_fir_decimate_instance_q15 Filter_I2 = { 4, 35, &Dec4LPCoef[0], &Filter_delay_I2[0] };
arm_fir_decimate_instance_q15 Filter_Q1 = { 4, 35, &Dec4LPCoef[0], &Filter_delay_Q1[0] };
arm_fir_decimate_instance_q15 Filter_Q2 = { 4, 35, &Dec4LPCoef[0], &Filter_delay_Q2[0] };
arm_fir_instance_q15 Filter_I3 = { 65, &Filter_delay_I3[0], &FIRBitCoef[0] };
arm_fir_instance_q15 Filter_Q3 = { 65, &Filter_delay_Q3[0], &FIRBitCoef[0] };
arm_fir_instance_q15 Filter_I4 = { 65, &Filter_delay_I4[0], &FIRFreqCoef[0] };
arm_fir_instance_q15 Filter_Q4 = { 65, &Filter_delay_Q4[0], &FIRFreqCoef[0] };
//chh PSK Filters

void Process_FIR_I(void)
{
	arm_fir_q15(&S_FIR_I, &FIR_I_In[0], &FIR_I_Out[0], BUFFERSIZE / 2);
}

void Process_FIR_Q(void)
{
	arm_fir_q15(&S_FIR_Q, &FIR_Q_In[0], &FIR_Q_Out[0], BUFFERSIZE / 2);
}

void Sideband_Demod(void)
{
	int16_t j;
	for (j = 0; j < BUFFERSIZE / 2; j++) {
		USB_Out[j] = FIR_I_Out[j] - FIR_Q_Out[j];
		LSB_Out[j] = FIR_I_Out[j] + FIR_Q_Out[j];
		//Fill the PSK Buffer
		ADC_Buffer[j] = USB_Out[j]; //made change to get display to work for psk
	}
}


void Process_FFT(void)
{
	//Execute complex FFT
	arm_cfft_radix2_q15(&S_CFFT, &FFT_Input[0]);
	//Shift FFT data to compensate for FFT scaling
	arm_shift_q15(&FFT_Input[0], 6, &FFT_Scale[0], 512 );
	//Calculate the magnitude squared of FFT results ( i.e., power level)
	arm_cmplx_mag_squared_q15(&FFT_Scale[0], &FFT_Magnitude[0], FFT_Size);
	//********************************************************
	Point_Mag = 0;
	Peak_Mag = 0;
	Total_Mag =0;
	Avg_Mag =0;

	Point_RSL = 0;
	Peak_RSL =0;
	Total_RSL = 0;
	Avg_RSL = 0;

	Sig_Max = 0;
	Sig2 = 0;
	Sig_Sum = 0;

	number_bins =0;
	AGC_Multiplier = (float)AGC_Scale/100.0;

if(0)
{

	for (int j = 0; j<256;j++) FFT_Mag_10[j] = (int) FFT_Magnitude[j] * 10; //add in 10 db gain
	//Sift Thru the Bins
	for (int16_t j = 8; j < 252; j++)
	{  //Changed for 512 FFT
		//This detects bins which are saturated and ignores them
		if (FFT_Mag_10[j]> 0)
		{
			FFT_Output[j] =  (10.0 * log((float32_t)FFT_Mag_10[j] + 1.0));
		}
			else
		{
			FFT_Output[j] = 0;
		}
		//First Order Filter for FFT
		FFT_Filter[j] =  FFT_Coeff * FFT_Output[j] + (1.0-FFT_Coeff) * FFT_Filter[j];


		//Find Point Values
		if(j== NCO_Bin)
		{
			for (int k = NCO_Bin-2; k < NCO_Bin+3; k++)
			{
				if ((int)FFT_Filter[k]> Point_RSL)
				{
					Point_RSL = 	(int)FFT_Filter[k];
					Point_Mag = 10*sqrt((float32_t)FFT_Magnitude[k]);
				}
			}
		}

		//Find Peak Values
		if (j>16 && (uint16_t)FFT_Filter[j] >  Peak_RSL)
		{
			Peak_RSL = (uint16_t)FFT_Filter[j];
			Peak_Mag = 10*sqrt((float32_t)FFT_Magnitude[j]);
		}

		//Calculate Average Values and Scale them
		if (j>8 && j<64 && (uint16_t)FFT_Filter[j]>2 )
		{
			Total_RSL = Total_RSL + (uint16_t)FFT_Filter[j];
			Total_Mag = Total_Mag + 10*sqrt((float32_t)FFT_Magnitude[j]);
			number_bins++;
		}
		if (number_bins>0)
		{
			Avg_RSL = (Total_RSL*AGC_Scale)/(number_bins*100);//Scaling is done with integer values
			Avg_Mag = Total_Mag*AGC_Multiplier/(float)number_bins;
		}
	}//End of loop over FFT bins

}// End of if(0) block

if(1)
{

	for (int j = 0; j<256;j++) FFT_Mag_10[j] = (int) FFT_Magnitude[j] * 10; //add in 10 db gain
	for (int16_t j = 8; j < 252; j++)
	{  //Changed for 512 FFT
		//This detects bins which are saturated and ignores them
		if (FFT_Mag_10[j]> 0)
		{
			FFT_Output[j] =  (10.0 * log((float32_t)FFT_Mag_10[j] + 1.0));
		}
			else
		{
			FFT_Output[j] = 0;
		}
		//First Order Filter for FFT
		FFT_Filter[j] =  FFT_Coeff * FFT_Output[j] + (1.0-FFT_Coeff) * FFT_Filter[j];


		//Point AGC
		if (j == NCO_Bin)
		{
			for (int k = NCO_Bin-2; k < NCO_Bin+3; k++)
			{
				Sig = (int)FFT_Magnitude[k];
				//if ( Sig > Sig_Max) Sig_Max = Sig;
				Sig2 += Sig;
			}
		}


		//Peak AGC
		if((int) FFT_Magnitude[j] > Sig_Max) Sig_Max = FFT_Magnitude[j];

		//Total or SSB or Average AGC
		if ( j > 8 && j < 128 )
		{
			Sig_Sum += (int)FFT_Magnitude[j];
			number_bins++;
		}
	}
	RMS_Sig = 10*sqrt((float32_t)Sig_Max); // Peak
	RMS_Sig2 = 10*sqrt((float32_t)Sig2);  // Point


	dB_Sig = 23. + 10*log((float32_t)Sig_Max + .1); //Peak
	dB_Sig2 = 23. + 10*log((float32_t)Sig2 + .1); //Point
	RMS_Sig_Sum = 10*sqrt((float32_t)Sig_Sum); //Average
	dB_Sig_Sum = 23. + 10*log((float32_t)Sig_Sum + .1); //Average

	RMS_Mag = FFT_Coeff*RMS_Sig + (1.-FFT_Coeff)*RMS_Mag; //Peak
	RMS_Mag2 = FFT_Coeff*RMS_Sig2 + (1.-FFT_Coeff)*RMS_Mag2; //Point
	dB_RSL = FFT_Coeff*dB_Sig + (1.-FFT_Coeff)*dB_RSL; //Peak
	dB_RSL2 = FFT_Coeff*dB_Sig2 + (1.-FFT_Coeff)*dB_RSL2; //Point
	Sig_Mag = FFT_Coeff*RMS_Sig_Sum + (1.-FFT_Coeff)*Sig_Mag;//Average
	Sig_RSL = FFT_Coeff*dB_Sig_Sum + (1.-FFT_Coeff)*Sig_RSL; //Average

	//Point_RSL = (int)dB_RSL;
	//Point_Mag = RMS_Mag;
	//Peak_RSL = (int)dB_RSL2;
	//Peak_Mag = RMS_Mag2;

	Point_RSL = (int)dB_RSL2;
	Point_Mag = RMS_Mag2;
	Peak_RSL = (int)dB_RSL;
	Peak_Mag = RMS_Mag;

	//Avg_RSL = (int)Sig_RSL*AGC_Scale/100;
	Avg_RSL = (int)Sig_RSL;
	//Avg_Mag = Sig_Mag*AGC_Multiplier/(float)number_bins;
	Avg_Mag = Sig_Mag;
}// End of if(1) block (new code by MEC W8NUE



}//End of Process_FFT

