/*
 * DSC_Processing.h
 *
 *  Created on: Nov 8, 2012
 *      Author: CharleyK
 */

#include	"Init_DMA.h"

#include	"PSKDet.h"

void Process_FIR_I(void);

void Process_FIR_Q(void);

void Process_FFT(void);

void Sideband_Demod(void);

//q15_t FFT_Magnitude[BUFFERSIZE/8];  //1024 sampling

q15_t FFT_Magnitude[BUFFERSIZE/4];  //512 sampling

q15_t FIR_I_In[BUFFERSIZE/2];

q15_t FIR_Q_In[BUFFERSIZE/2];

q15_t FIR_I_Out[BUFFERSIZE/2];

q15_t FIR_Q_Out[BUFFERSIZE/2];

q15_t USB_Out[BUFFERSIZE/2];

q15_t LSB_Out[BUFFERSIZE/2];

//q15_t   FFT_Input[BUFFERSIZE/4];  //1024 sampling

q15_t   FFT_Input[BUFFERSIZE/2];  //512sampling

//chh PSK Filters Arrays

arm_fir_decimate_instance_q15 Filter_I1;

arm_fir_decimate_instance_q15 Filter_I2;

arm_fir_instance_q15 Filter_I3;

arm_fir_instance_q15 Filter_I4;

arm_fir_decimate_instance_q15 Filter_Q1;

arm_fir_decimate_instance_q15 Filter_Q2;

arm_fir_instance_q15 Filter_Q3;

arm_fir_instance_q15 Filter_Q4;

q15_t Filter_delay_I1 [38];

q15_t Filter_delay_I2 [38];

q15_t Filter_delay_Q1[38];

q15_t Filter_delay_Q2[38];

q15_t Filter_delay_I3[65];

q15_t Filter_delay_Q3[65];

q15_t Filter_delay_I4[65];

q15_t Filter_delay_Q4[65];

q15_t ADC_Buffer[BUFFERSIZE/2];




