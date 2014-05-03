/*
 * Header file for DSP processing
 *  *
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
#include	"PSKDet.h"

void Process_FIR_I(void);
void Process_FIR_Q(void);
void Process_FFT(void);
void Sideband_Demod(void);
void init_DSP(void);
void Process_post_FILT (void);


extern float32_t FFT_Output[256];
extern float32_t FFT_Filter[256];
extern q15_t FFT_Input[1024];  //512sampling
extern q15_t FFT_Magnitude[512];  //512 sampling

q15_t FIR_I_In[BUFFERSIZE / 2];
q15_t FIR_Q_In[BUFFERSIZE / 2];
q15_t FIR_I_Out[BUFFERSIZE / 2];
q15_t FIR_Q_Out[BUFFERSIZE / 2];
q15_t USB_Out[BUFFERSIZE / 2];
q15_t LSB_Out[BUFFERSIZE / 2];

//chh PSK Filters Arrays

arm_fir_decimate_instance_q15 Filter_I1;
arm_fir_decimate_instance_q15 Filter_I2;

arm_fir_instance_q15 Filter_I3;
arm_fir_instance_q15 Filter_I4;

arm_fir_decimate_instance_q15 Filter_Q1;
arm_fir_decimate_instance_q15 Filter_Q2;

arm_fir_instance_q15 Filter_Q3;
arm_fir_instance_q15 Filter_Q4;

q15_t Filter_delay_I1[38];
q15_t Filter_delay_I2[38];
q15_t Filter_delay_I3[65];
q15_t Filter_delay_I4[65];

q15_t Filter_delay_Q1[38];
q15_t Filter_delay_Q2[38];
q15_t Filter_delay_Q3[65];
q15_t Filter_delay_Q4[65];

q15_t ADC_Buffer[BUFFERSIZE / 2];

