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
#include    "ChangeOver.h"

float   AGC_Mag;
float   DAC_AGC_Mag;
int		RSL_Mag;

int PGAGAIN0;
int dac_gain;

int Old_PGAGAIN0;
int Old_dac_gain;

int Delta_Gain;
int PGAGain;
int Old_PGAGain;
int AGC_Mode;
int AGC_On;

int RSL_Cal;


double  Vmin;
double  SigRatio;
double 	AGC_Coeff;
double 	R_AGC_Coeff;
double 	AGCF_Coeff;
double 	R_AGCF_Coeff;
double 	AGC;
double 	Old_AGC;

float   Sig_Thr;
float   DeltaPGA;
float   DDeltaPGA;
float   Old_DDeltaPGA;
float	PGA_Coeff;
float   FPGA_Coeff;
int		AGC_Signal;
int		Old_AGC_Signal;
int 	shape = 3; 		// Mode 0 (CW)  time constant shaping:  decay * shape and attack / shape

// Original SDR2GO Various Time Constants
//const float PGA_TC_Slow = .5; 	//Time constant for slow PGA decay
//const float PGA_TC_Fast = .01; 	//Time constant for fast PGA attack
//const float AGC_TC_Slow = .5;	    //Time constant for slow AGC decay
//const float AGC_TC_Fast = .01;	//Time constant for fast AGC attack

const float PGA_TC_Slow = 3.0; 	//Time constant for slow PGA decay
const float PGA_TC_Fast = .1;	//Time constant for fast PGA attack
const float AGC_TC_Slow = .8;	//Time constant for slow AGC decay
const float AGC_TC_Fast = .1;	//Time constant for fast AGC attack

//const double dt = .0000833;//12000 hz sample rate artifact from SDR2GO
const double dt = .000125;//8000 hz sample rate

void Calc_AGC_Setpoints(void);
void Proc_AGC(void);
void Init_AGC (void);

void Proc_AGC(void) {
	Calc_AGC_Setpoints();
	AGC_Flag =0;
	}

void Calc_AGC_Setpoints(void) {
	//PGA AGC Section
	//calculate gain change needed to reduce peak signal
	DeltaPGA = 20*log10f( Sig_Thr/(AGC_Mag + 1.0));

		if (DeltaPGA>0){  //Slow Decay
		DDeltaPGA = PGA_Coeff*DeltaPGA + (1-PGA_Coeff)*Old_DDeltaPGA;
		}
		else { //Fast Attack
		DDeltaPGA = FPGA_Coeff*DeltaPGA + (1-FPGA_Coeff)*Old_DDeltaPGA;
		}

	// if ((long)DDeltaPGA == 0x7FFFFFFF) DDeltaPGA = 0.0;
		Old_DDeltaPGA = DDeltaPGA;

	if(AGC_On == 1)
			{		//AGC is On
			PGAGain += (int)DDeltaPGA;
			if (PGAGain<0) PGAGain = 0;
			if (PGAGain > PGAGAIN0) PGAGain = PGAGAIN0;

			if (PGAGain != Old_PGAGain)
			{Set_PGA_Gain (PGAGain);
			Old_PGAGain = PGAGain;
			}
		} //AGC is On
			else
		{				//AGC is Off
			if (PGAGAIN0 != Old_PGAGAIN0 && RxTx_InRxMode())
			{Set_PGA_Gain(PGAGAIN0);
			PGAGain = PGAGAIN0;
			Old_PGAGAIN0 = PGAGAIN0;
			}
		} //AGC is Of


	//DAC AGC Section
	SigRatio = 32*log10f(Vmin/((double )DAC_AGC_Mag+1));

		if (SigRatio > 0){
		AGC = AGC_Coeff*SigRatio + R_AGC_Coeff*Old_AGC;
		}
		else {
		AGC = AGCF_Coeff*SigRatio + R_AGCF_Coeff*Old_AGC;
			}

		// if ((long)Old_AGC==0x7FFFFFFF) Old_AGC = 0.; //test for error condition
		// if ((long)AGC==0x7FFFFFFF) AGC = 0.; //test for error condition
	//	if ((long)Old_AGC >= 0x7FFFFFFF) Old_AGC = 0.4999; //test for error condition
	//	if ((long)AGC >= 0x7FFFFFFF) AGC = 0.4999; //test for error condition

	if(AGC_On == 1)  //AGC ON
		{	//AGC is On
		Old_AGC = AGC;
		AGC_Signal =dac_gain + (int)AGC;

		if (AGC_Signal != Old_AGC_Signal)
				{Set_DAC_DVC(AGC_Signal);
				Old_AGC_Signal = AGC_Signal;
				}
			}   //AGC On
		else
		{	//AGC is Off
		if (dac_gain != Old_dac_gain && RxTx_InRxMode())
				{Set_DAC_DVC (dac_gain);
				AGC_Signal = dac_gain;
				Old_dac_gain = dac_gain;
				}
			}   //AGC is Off

       }  //End of Calc_AGC_Setpoints()




void Init_AGC (void)			{

		PGA_Coeff =dt*(BUFFERSIZE/2)/PGA_TC_Slow;
		FPGA_Coeff = dt*(BUFFERSIZE/2)/PGA_TC_Fast;

		AGC_Coeff = dt*(BUFFERSIZE/2)/AGC_TC_Slow;
		AGCF_Coeff = dt*(BUFFERSIZE/2)/AGC_TC_Fast;

		// Mode 0 CW shaping  JDG
		if (AGC_Mode == 0)
			{PGA_Coeff *= shape;
			FPGA_Coeff /= (shape/2);
			AGC_Coeff *= shape;
			AGCF_Coeff /= (shape/2);
			}

		R_AGC_Coeff = 1 - AGC_Coeff;
		R_AGCF_Coeff = 1 - AGCF_Coeff;

		AGC_Mag = 1.0;
		DAC_AGC_Mag = 1.0;

		PGAGAIN0 = Options_GetValue(OPTION_RX_RF);
		PGAGain = PGAGAIN0;
		Old_PGAGain = PGAGain;
		Set_PGA_Gain (PGAGain);

		Old_PGAGAIN0 = -200; //Set to an outrageous value for change testing in AGC_Processsing.c
		Old_dac_gain = -200; //Set to an outrageous value for change testing in AGC_Processsing.c

		dac_gain = Options_GetValue(OPTION_RX_AUDIO);
		AGC_Signal = dac_gain;
		Old_AGC_Signal = AGC_Signal;
		Set_DAC_DVC(AGC_Signal);

		Vmin = 100.0;
		Sig_Thr = (float)Options_GetValue(OPTION_AGC_THRSH);
		RSL_Cal = Options_GetValue(OPTION__RSL_CAL);

		if (AGC_Mode != 3)
		AGC_On =1;
		else
		AGC_On =0;

	}



