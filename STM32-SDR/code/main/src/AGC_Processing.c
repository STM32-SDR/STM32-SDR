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

const double Vmin = 200.;  //This is the threshold for DAC AGC

extern float   AGC_Mag;

int		Sig2;
int		Sig2F;
int		Old_Sig2F;

int PGAGAIN0;
int dac_gain;
int Delta_Gain;
int PGAGain;
int Old_PGAGain;

float   Sig_Thr;
int		Sig2F_Coeff_INV;

double 	AGC_Coeff;
double 	R_AGC_Coeff;
double 	AGCF_Coeff;
double 	R_AGCF_Coeff;
double 	AGC;
double 	Old_AGC;
double	SigRatio;
float   DeltaPGA;
float   DDeltaPGA;
float   Old_DDeltaPGA;
float	PGA_Coeff;
float   FPGA_Coeff;
int		AGC_Signal;
int		Old_AGC_Signal;

// Various Time Constants
//const float PGA_TC_Slow = .5; 	//Time constant for slow PGA decay
//const float PGA_TC_Fast = .01; 	//Time constant for fast PGA attack
//const float AGC_TC_Slow = .5;	//Time constant for slow AGC decay
//const float AGC_TC_Fast = .01;	//Time constant for fast AGC attack
const float PGA_TC_Slow = 0.4; 	//Time constant for slow PGA decay
const float PGA_TC_Fast = .1; 	//Time constant for fast PGA attack
const float AGC_TC_Slow = .5;	//Time constant for slow AGC decay
const float AGC_TC_Fast = .1;	//Time constant for fast AGC attack

const float Sig2F_TC 	= .004;	//Time constant for Sig2F filter
//const float Sig2F_TC 	= .0004;	//Time constant for Sig2F filter
const float Envelope_TC = .004;	//Time constant for Envelope filter

//const double dt = .0000833;//12000 hz sample rate
const double dt = .000125;//8000 hz sample rate

void Calc_Sig2F(void);
void CalcPGA_Setpoint(void);

void Proc_AGC(void);
void Init_AGC (void);

void Proc_AGC(void) {
	if (AGC_Flag == 1) {
	//Calc_Sig2F();
	CalcPGA_Setpoint();
	AGC_Flag =0;
	}
}

void Calc_Sig2F(void) {
	for(int i =0; i<BUFFERSIZE/2;i++)
	{
	Sig2 = abs(Rx1BufferDMA[2 * i]);
	Sig2F = Old_Sig2F + (Sig2-Old_Sig2F)/Sig2F_Coeff_INV ;
	Old_Sig2F = Sig2F;
	}
}

void CalcPGA_Setpoint(void) {
	//calculate gain change needed to reduce peak signal
	//DeltaPGA = 20*log10f( Sig_Thr/ (float)Sig2F );
	DeltaPGA = 20*log10f( Sig_Thr/ (AGC_Mag + 1.0));

		if (DeltaPGA>0){  //Slow Decay
		DDeltaPGA = PGA_Coeff*DeltaPGA + (1-PGA_Coeff)*Old_DDeltaPGA;
		}
		else { //Fast Attack
		DDeltaPGA = FPGA_Coeff*DeltaPGA + (1-FPGA_Coeff)*Old_DDeltaPGA;
		}

	if ((long)DDeltaPGA == 0x7FFFFFFF) DDeltaPGA = 0.;
	Old_DDeltaPGA = DDeltaPGA;
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
		Sig2F_Coeff_INV = (int)(Sig2F_TC/dt);

		Sig2F = 0;
		Old_Sig2F = 0;

		AGC_Mag = 1.0;
		PGAGAIN0 = Options_GetValue(OPTION_RX_RF);
		PGAGain = PGAGAIN0;
		Old_PGAGain = PGAGain;
		Sig_Thr = (float)Options_GetValue(OPTION_AGC_THRSH);
		Set_PGA_Gain (PGAGain);
	}
