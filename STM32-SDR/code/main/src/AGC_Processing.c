/*
 * AGC_Processing.c
 *
 *  Created on: Sep 6, 2013
 *      Author: CharleyK
 */

#include 	"AGC_Processing.h"
#include 	"DSP_Processing.h"
#include 	"Init_DMA.h"
#include	"arm_math.h"
#include	"stdlib.h"
#include	"Codec_Gains.h"
#include	"options.h"
#include	"DMA_IRQ_Handler.h"

float   AGC_Mag;
float   DAC_AGC_Mag;
int		RSL_Mag;

int PGAGAIN0;
int dac_gain;
int Delta_Gain;
int PGAGain;
int Old_PGAGain;
int AGC_Mode;
int AGC_On;

float   Sig_Thr;

double 	AGC_Coeff;
double 	R_AGC_Coeff;
double 	AGCF_Coeff;
double 	R_AGCF_Coeff;
double 	AGC;
double 	Old_AGC;
double  SigRatio;
double  Vmin;

float   DeltaPGA;
float   DDeltaPGA;
float   Old_DDeltaPGA;
float	PGA_Coeff;
float   FPGA_Coeff;
int		AGC_Signal;
int		Old_AGC_Signal;

// Original SDR2GO Various Time Constants
//const float PGA_TC_Slow = .5; 	//Time constant for slow PGA decay
//const float PGA_TC_Fast = .01; 	//Time constant for fast PGA attack
//const float AGC_TC_Slow = .5;	//Time constant for slow AGC decay
//const float AGC_TC_Fast = .01;	//Time constant for fast AGC attack

const float PGA_TC_Slow = 3.0;
//const float PGA_TC_Slow = 0.8;
const float PGA_TC_Fast = .1;//Time constant for fast PGA attack
const float AGC_TC_Slow = 3.0;
//const float AGC_TC_Slow = .8;	//Time constant for slow AGC decay
const float AGC_TC_Fast = .1;	//Time constant for fast AGC attack

//const double dt = .0000833;//12000 hz sample rate artifact from SDR2GO
const double dt = .000125;//8000 hz sample rate

void CalcPGA_Setpoint(void);
void Proc_AGC(void);
void Init_AGC (void);

void Proc_AGC(void) {
	if (AGC_Flag == 1) {

	CalcPGA_Setpoint();
	AGC_Flag =0;
	}
}

void CalcPGA_Setpoint(void) {


	//PGA AGC Section
	//calculate gain change needed to reduce peak signal
	//DeltaPGA = 10*log10f( Sig_Thr/ (AGC_Mag + 1.0));
	DeltaPGA = 20*log10f( Sig_Thr/ (AGC_Mag + 1.0));
		if (DeltaPGA>0){  //Slow Decay
		DDeltaPGA = PGA_Coeff*DeltaPGA + (1-PGA_Coeff)*Old_DDeltaPGA;
		}
		else { //Fast Attack
		DDeltaPGA = FPGA_Coeff*DeltaPGA + (1-FPGA_Coeff)*Old_DDeltaPGA;
		}

	if ((long)DDeltaPGA == 0x7FFFFFFF) DDeltaPGA = 0.; //Artifact from SDR2GO supsect causes lock up
		Old_DDeltaPGA = DDeltaPGA;

		PGAGain += (int)DDeltaPGA;
	if (PGAGain<0) PGAGain = 0;
	if (PGAGain > PGAGAIN0) PGAGain = PGAGAIN0;
		if (PGAGain != Old_PGAGain) {

			if(AGC_On == 1)
			{Set_PGA_Gain (PGAGain);}
			else
			{Set_PGA_Gain (PGAGAIN0);}

			Old_PGAGain = PGAGain;
		}

		//DAC AGC Section
			SigRatio = 32*log10f(Vmin/(double )DAC_AGC_Mag+1);

			if (SigRatio > 0){
			AGC = AGC_Coeff*SigRatio + R_AGC_Coeff*Old_AGC;

			}
			else {
			AGC = AGCF_Coeff*SigRatio + R_AGCF_Coeff*Old_AGC;
				}

			if ((long)Old_AGC==0x7FFFFFFF) Old_AGC = 0.; //test for error condition
			if ((long)AGC==0x7FFFFFFF) AGC = 0.; //test for error condition

			Old_AGC = AGC;
			AGC_Signal =dac_gain + (int)AGC;

			if (AGC_Signal != Old_AGC_Signal) {

				if(AGC_On == 1)
				{Set_DAC_DVC(AGC_Signal);}
				else
				{Set_DAC_DVC(dac_gain);}


			Old_AGC_Signal = AGC_Signal;
     }

     }


void Init_AGC (void)			{

		PGA_Coeff = dt*(BUFFERSIZE/2)/PGA_TC_Slow;
		FPGA_Coeff = dt*(BUFFERSIZE/2)/PGA_TC_Fast;

		AGC_Coeff = dt*BUFFERSIZE/2/AGC_TC_Slow;
		AGCF_Coeff = dt*BUFFERSIZE/2/AGC_TC_Fast;

		R_AGC_Coeff = 1 - AGC_Coeff;
		R_AGCF_Coeff = 1 - AGCF_Coeff;


		AGC_Mag = 1.0;
		DAC_AGC_Mag = 1.0;

		PGAGAIN0 = 2*Options_GetValue(OPTION_RX_RF);

		PGAGain = PGAGAIN0;
		Old_PGAGain = PGAGain;

		dac_gain = Options_GetValue(OPTION_RX_AUDIO);
		AGC = dac_gain;
		Old_AGC = AGC;

		//Vmin = 100.0;
		Vmin = (float)Options_GetValue(OPTION_AGC_THRSH);
		Sig_Thr = 250.0; //Set Sig_Thr above DAC AGC Threshold
		//Sig_Thr = (float)Options_GetValue(OPTION_AGC_THRSH);
		Set_PGA_Gain (PGAGain);
		AGC_On =1;
	}
