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
int		RSL_Mag;

int PGAGAIN0;
int dac_gain;
int Delta_Gain;
int PGAGain;
int Old_PGAGain;
int AGC_Mode;

float   Sig_Thr;

double 	AGC_Coeff;
double 	R_AGC_Coeff;
double 	AGCF_Coeff;
double 	R_AGCF_Coeff;
double 	AGC;
double 	Old_AGC;
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
const float PGA_TC_Slow = 0.8;
const float PGA_TC_Fast = .1; 	//Time constant for fast PGA attack
const float AGC_TC_Slow = .5;	//Time constant for slow AGC decay
const float AGC_TC_Fast = .1;	//Time constant for fast AGC attack

//const double dt = .0000833;//12000 hz sample rate artifact from SDR2GO
const double dt = .000125;//8000 hz sample rate

void CalcPGA_Setpoint(void);
void Proc_AGC(void);
void Init_AGC (void);

void Proc_AGC(void) {
	if (AGC_Flag == 1) {

	switch(AGC_Mode)	{

	case 0:
		{
		AGC_Mag = Point_Mag;
		RSL_Mag = Point_RSL;
		}
		break;

	case 1:
		{
		AGC_Mag = Peak_Mag;
		RSL_Mag = Peak_RSL;
		}
		break;

	case 2:
		{
		AGC_Mag = Avg_Mag;
		RSL_Mag = Avg_RSL;
		}
		break;

	}

	CalcPGA_Setpoint();
	AGC_Flag =0;
	}
}

void CalcPGA_Setpoint(void) {
	//calculate gain change needed to reduce peak signal
	DeltaPGA = 20*log10f( Sig_Thr/ (AGC_Mag + 1.0));
		if (DeltaPGA>0){  //Slow Decay
		DDeltaPGA = PGA_Coeff*DeltaPGA + (1-PGA_Coeff)*Old_DDeltaPGA;
		}
		else { //Fast Attack
		DDeltaPGA = FPGA_Coeff*DeltaPGA + (1-FPGA_Coeff)*Old_DDeltaPGA;
		}

	//if ((long)DDeltaPGA == 0x7FFFFFFF) DDeltaPGA = 0.; Artifact from SDR2GO supsect causes lock up
	if (DDeltaPGA > -10.0)Old_DDeltaPGA = DDeltaPGA; //Prevent wind up if strong signal present
	if (DDeltaPGA > 0.0 && DDeltaPGA > 1.0) DDeltaPGA = 1.0;//Limit rate of change on decay AGC
	PGAGain += (int)DDeltaPGA;
	if (PGAGain<0) PGAGain = 0;
	if (PGAGain > PGAGAIN0) PGAGain = PGAGAIN0;
		if (PGAGain != Old_PGAGain) {
			Set_PGA_Gain (PGAGain);
			Old_PGAGain = PGAGain;
		}
	}

void Init_AGC (void)			{

		PGA_Coeff = dt*(BUFFERSIZE/2)/PGA_TC_Slow;
		FPGA_Coeff = dt*(BUFFERSIZE/2)/PGA_TC_Fast;

		AGC_Mag = 1.0;
		PGAGAIN0 = 2*Options_GetValue(OPTION_RX_RF);
		PGAGain = PGAGAIN0;
		Old_PGAGain = PGAGain;
		Sig_Thr = (float)Options_GetValue(OPTION_AGC_THRSH);
		Set_PGA_Gain (PGAGain);
	}
