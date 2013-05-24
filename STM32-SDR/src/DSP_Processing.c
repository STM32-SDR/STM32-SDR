/*
 * DSP_Processing.c
 *
 *  Created on: Nov 8, 2012
 *      Author: CharleyK
 */

#include	"Init_DMA.h"
#include	"arm_math.h"
#include	"FIR_Coefficients.h"
#include	"DSP_Processing.h"
//#include	"PSKDet.h"

	//q15_t   FFT_Input[BUFFERSIZE/4]; //1024 sampling
	q15_t   FFT_Input[BUFFERSIZE/2]; //512 sampling

	//q15_t   FFT_Test[BUFFERSIZE/4]; //1024 sampling
	q15_t   FFT_Test[BUFFERSIZE/2]; //512 sampling

	//q15_t   FFT_Scale[BUFFERSIZE/4]; //1024 sampling
	q15_t   FFT_Scale[BUFFERSIZE/2]; //512 sampling

	//q15_t   FFT_Magnitude[BUFFERSIZE/8]; //1024 sampling
	q15_t   FFT_Magnitude[BUFFERSIZE/4]; //512 sampling

	q15_t 	FIR_State_I[NUM_FIR_COEF+(BUFFERSIZE/2)-1];

	q15_t 	FIR_State_Q[NUM_FIR_COEF+(BUFFERSIZE/2)-1];

	q15_t FIR_I_In[BUFFERSIZE/2];

	q15_t FIR_Q_In[BUFFERSIZE/2];

	q15_t FIR_I_Out[BUFFERSIZE/2];

	q15_t FIR_Q_Out[BUFFERSIZE/2];

	q15_t USB_Out[BUFFERSIZE/2];

	q15_t LSB_Out[BUFFERSIZE/2];

	//uint16_t FFT_Size = BUFFERSIZE/8;  // change for 1024 sampling
	uint16_t FFT_Size = BUFFERSIZE/4;  // change for 512 sampling

	uint8_t FFT_status;

	//chh Filter Definitions imported from NUE PSK
	q15_t Filter_delay_I1 [38]; //38 = 35 + 4 -1

	q15_t Filter_delay_I2 [38];

	q15_t Filter_delay_Q1[38];

	q15_t Filter_delay_Q2[38];

	q15_t Filter_delay_I3[65]; //65 = 65 + 1 -1

	q15_t Filter_delay_Q3[65];

	q15_t Filter_delay_I4[65];

	q15_t Filter_delay_Q4[65];

	//chh Filter Definitions imported from NUE PSK

	//chh stuff for filling PSK  Buffer


	q15_t ADC_Buffer[BUFFERSIZE/2];  //for 1024 sampling

	arm_cfft_radix4_instance_q15  S_CFFT;

	arm_fir_instance_q15 S_FIR_I = {NUM_FIR_COEF, &FIR_State_I[0], &coeff_fir_I[0]};

	arm_fir_instance_q15 S_FIR_Q = {NUM_FIR_COEF,  &FIR_State_Q[0], &coeff_fir_Q[0] };

	//chh PSK Filters

	arm_fir_decimate_instance_q15 Filter_I1 = {4, 35, &Dec4LPCoef[0], &Filter_delay_I1[0] };

	arm_fir_decimate_instance_q15 Filter_I2 = {4, 35, &Dec4LPCoef[0], &Filter_delay_I2[0] };

	arm_fir_decimate_instance_q15 Filter_Q1 = {4, 35, &Dec4LPCoef[0], &Filter_delay_Q1[0] };

	arm_fir_decimate_instance_q15 Filter_Q2 = {4, 35, &Dec4LPCoef[0], &Filter_delay_Q2[0] };

	arm_fir_instance_q15 Filter_I3 = {65,  &Filter_delay_I3[0], &FIRBitCoef[0] };

	arm_fir_instance_q15 Filter_Q3 = {65,  &Filter_delay_Q3[0], &FIRBitCoef[0] };

	arm_fir_instance_q15 Filter_I4 = {65,  &Filter_delay_I4[0], &FIRFreqCoef[0] };

	arm_fir_instance_q15 Filter_Q4 = {65,  &Filter_delay_Q4[0], &FIRFreqCoef[0] };

	//chh PSK Filters

void Process_FIR_I(void){

			arm_fir_q15	(
			&S_FIR_I,
			&FIR_I_In[0],
			&FIR_I_Out[0],
			BUFFERSIZE/2);
			}

void Process_FIR_Q(void) {

			arm_fir_q15	(
			&S_FIR_Q,
			&FIR_Q_In[0],
			&FIR_Q_Out[0],
			BUFFERSIZE/2);
			}

void Sideband_Demod(void) {
	int16_t j;
	for (j=0; j<BUFFERSIZE/2; j++)
	{
	USB_Out[j] = FIR_I_Out[j] - FIR_Q_Out[j];
	LSB_Out[j] = FIR_I_Out[j] + FIR_Q_Out[j];
	//Fill the PSK Buffer
	ADC_Buffer[j] = USB_Out[j]; //made change to get display to work for psk
	}
	}


void Process_FFT(void) {
	//Set up structure for complex FFT processing
	FFT_status = arm_cfft_radix4_init_q15(
	&S_CFFT,
	FFT_Size,
	0,
	1
	);


	//Execute complex FFT
	arm_cfft_radix4_q15(&S_CFFT,&FFT_Input[0]);

	//Shift FFT data to compensate for FFT scaling
	arm_shift_q15(	&FFT_Input[0],
					6,
					&FFT_Scale[0],
					BUFFERSIZE/4 //1024 sampling
					);

	//Calculate the magnitude squared of FFT results ( i.e., power level)
	arm_cmplx_mag_squared_q15(	&FFT_Scale[0],
								&FFT_Magnitude[0],
								FFT_Size
								);
					}


