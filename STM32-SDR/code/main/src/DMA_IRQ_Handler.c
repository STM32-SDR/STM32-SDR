/*
 * DMA IRQ handler routines
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
#include 	"stm32f4xx_dma.h"
#include 	"DSP_Processing.h"
#include 	"DMA_IRQ_Handler.h"
#include 	"ModeSelect.h"
#include	"CW_Mod.h"
#include    "stm32f4xx_gpio.h"
#include 	"PSKMod.h"
#include 	"AGC_Processing.h"
#include 	"DMA_Test_Pins.h"
#include	"stm32f4xx_gpio.h"

uint32_t DMA_RX_Memory;
uint32_t DMA_TX_Memory;
int16_t DSP_Flag = 0;
int16_t AGC_Flag = 0;
int16_t Tx_Flag;
int16_t i;

float rgain;
float R_lgain;
float R_xgain;
float T_lgain;
float T_xgain;
float phase_adjust;

const float Amp0 = 32766;
static float temp;
static float temp_old;
float key;

const float A = 0.000125 / .005; // sample interval divided by keying time constant
const float B = 1.0 - 0.000125 / .005;

float PSK_Tx;
float PSK_Gain;

void DMA1_Stream0_IRQHandler(void)
{
	uint8_t Key_Down;

	//Check to see which set of buffers are currently in use
	DMA_RX_Memory = DMA_GetCurrentMemoryTarget(DMA1_Stream0 );
	DMA_TX_Memory = DMA_GetCurrentMemoryTarget(DMA1_Stream5 );

	if (Tx_Flag == 0) {
		Rcvr_DSP();
	}
	else {
		switch (Mode_GetCurrentMode()) {
		case MODE_SSB:
			Xmit_SSB();
			break;

		case MODE_CW:
			Key_Down = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_9 );
			if (Key_Down == 0)
				key = Amp0;
			else
				key = 0.0;
			Xmit_CW();
			break;

		case MODE_PSK:
			Xmit_PSK();
			break;
		}  //End of Mode Switch
	}

	DSP_Flag = 1;
	AGC_Flag = 1;

	//Clear the DMA buffer full interrupt flag
	DMA_ClearITPendingBit(DMA1_Stream0, DMA_IT_TCIF0 );

}

void Rcvr_DSP(void)
{
	GPIO_WriteBit(Test_GPIO, Test_0, Bit_SET);
	if (DMA_RX_Memory == 0) //Transfer I/Q data and fill FFT buffer on inactive buffer
	        {
		for (i = 0; i < BUFFERSIZE / 2; i++) {
			FIR_I_In[i] = (q15_t) ((float) Rx1BufferDMA[2 * i] * R_lgain);
			FFT_Input[i * 2] = FIR_I_In[i];
			phase_adjust = (float) Rx1BufferDMA[2 * i] * R_xgain;
			FIR_Q_In[i] = (q15_t) (((float) Rx1BufferDMA[2 * i + 1] + phase_adjust) * rgain);
			FFT_Input[i * 2 + 1] = FIR_Q_In[i];
		}

		Process_FIR_I();
		Process_FIR_Q();
		Sideband_Demod(); //PSK buffer is filled in this procedure with USB data stream
		ProcPSKDet();
		GPIO_WriteBit(Test_GPIO, Test_2, Bit_SET);
		Process_FFT();
		GPIO_WriteBit(Test_GPIO, Test_2, Bit_RESET);

		for (i = 0; i < BUFFERSIZE / 2; i++) {
			Tx1BufferDMA[2 * i] = (int16_t) USB_Out[i];
			//Tx1BufferDMA[2*i+1] = (int16_t)LSB_Out[i];  //left for future testing
			Tx1BufferDMA[2 * i + 1] = (int16_t) USB_Out[i];
		}

	}   //End of Buffer 0 Processing

	else  //Transfer I/Q data and fill FFT buffer on inactive buffer

	{
		for (i = 0; i < BUFFERSIZE / 2; i++) {
			FIR_I_In[i] = (q15_t) ((float) Rx0BufferDMA[2 * i] * R_lgain);
			FFT_Input[i * 2] = FIR_I_In[i];
			phase_adjust = (float) Rx0BufferDMA[2 * i] * R_xgain;
			FIR_Q_In[i] = (q15_t) (((float) Rx0BufferDMA[2 * i + 1] + phase_adjust) * rgain);
			FFT_Input[i * 2 + 1] = FIR_Q_In[i];
		}

		Process_FIR_I();
		Process_FIR_Q();
		Sideband_Demod(); //PSK Buffer is filled in this procedure
		ProcPSKDet();
		GPIO_WriteBit(Test_GPIO, Test_2, Bit_SET);
		Process_FFT();
		GPIO_WriteBit(Test_GPIO, Test_2, Bit_RESET);

		for (i = 0; i < BUFFERSIZE / 2; i++) {
			Tx0BufferDMA[2 * i] = (int16_t) USB_Out[i];
			//Tx0BufferDMA[2*i+1] = (int16_t)LSB_Out[i];  //left for future testing
			Tx0BufferDMA[2 * i + 1] = (int16_t) USB_Out[i];
		}

	}   //End of Buffer 1 Processing
	GPIO_WriteBit(Test_GPIO, Test_0, Bit_RESET);
}  // End of Rcvr_DSP( )

void Xmit_SSB(void)
{
	if (DMA_RX_Memory == 0) //Transfer I/Q data and fill FFT buffer on inactive buffer
	        {
		for (i = 0; i < BUFFERSIZE / 2; i++) {
			FIR_I_In[i] = (q15_t) ((float) Rx1BufferDMA[2 * i] * T_lgain);
			FFT_Input[i * 2] = FIR_I_In[i];
			phase_adjust = (float) Rx1BufferDMA[2 * i] * T_xgain;
			FIR_Q_In[i] = (q15_t) (((float) Rx1BufferDMA[2 * i+1] + phase_adjust) * rgain); //feed same audio in for SSB
			FFT_Input[i * 2 + 1] = FIR_Q_In[i];
		}

		Process_FIR_I();
		Process_FIR_Q();
		Process_FFT();
		//Output FIR filter results to codec
		for (i = 0; i < BUFFERSIZE / 2; i++) {
			Tx1BufferDMA[2 * i] = (int16_t) FIR_I_Out[i];
			Tx1BufferDMA[2 * i + 1] = (int16_t) FIR_Q_Out[i];
		}
	}		//End of Buffer 1 Processing

	//Transfer I/Q data and fill FFT buffer on inactive buffer
	else {
		for (i = 0; i < BUFFERSIZE / 2; i++) {
			FIR_I_In[i] = (q15_t) ((float) Rx0BufferDMA[2 * i] * T_lgain);
			FFT_Input[i * 2] = FIR_I_In[i];
			phase_adjust = (float) Rx0BufferDMA[2 * i] * T_xgain;
			FIR_Q_In[i] = (q15_t) (((float) Rx0BufferDMA[2 * i+1] + phase_adjust) * rgain); //feed same audio in for SSB
			FFT_Input[i * 2 + 1] = FIR_Q_In[i];
		}

		Process_FIR_I();
		Process_FIR_Q();
		Process_FFT();

		//Output FIR filter results to codec
		for (i = 0; i < BUFFERSIZE / 2; i++) {
			Tx0BufferDMA[2 * i] = (int16_t) FIR_I_Out[i];
			Tx0BufferDMA[2 * i + 1] = (int16_t) FIR_Q_Out[i];
		}

	}  //End of Buffer 0 Processing

}  //End of Xmit_SSB()

void Xmit_CW(void)
{
	static q15_t NCO_I;
	static long NCO_phz;
	float CW_Gain;

	x_NCOphzinc = (PI2 * (double) NCO_Frequency / (double) Sample_Frequency);

	if (DMA_RX_Memory == 0) //Transfer I/Q data and fill FFT buffer on inactive buffer
	        {
		for (i = 0; i < BUFFERSIZE / 2; i++) {
			NCO_phz += (long) (KCONV * (x_NCOphzinc));
			NCO_I = (Sine_table[((NCO_phz >> 4) + 0x400) & 0xFFF]);

			temp = A * (key - temp_old) + B * temp_old;
			temp_old = temp;
			CW_Gain = temp / Amp0;

			FIR_I_In[i] = (q15_t) ((float) NCO_I * T_lgain * CW_Gain);
			FFT_Input[i * 2] = FIR_I_In[i];
			phase_adjust = (float) NCO_I * T_xgain;
			FIR_Q_In[i] = (q15_t) (((float) NCO_I + phase_adjust) * (CW_Gain) * rgain); //
			FFT_Input[i * 2 + 1] = FIR_Q_In[i];
		}

		Process_FIR_I();
		Process_FIR_Q();
		Process_FFT();

		for (i = 0; i < BUFFERSIZE / 2; i++)				//Output FIR filter results to codec
		        {
			Tx1BufferDMA[2 * i] = (int16_t) FIR_I_Out[i];
			Tx1BufferDMA[2 * i + 1] = (int16_t) FIR_Q_Out[i];
		}

	}		//End of Buffer 1 Processing
	else  //Transfer I/Q data and fill FFT buffer on inactive buffer
	{
		for (i = 0; i < BUFFERSIZE / 2; i++) {
			NCO_phz += (long) (KCONV * (x_NCOphzinc));
			NCO_I = (Sine_table[((NCO_phz >> 4) + 0x400) & 0xFFF]);

			temp = A * (key - temp_old) + B * temp_old;
			temp_old = temp;
			CW_Gain = temp / Amp0;

			FIR_I_In[i] = (q15_t) ((float) NCO_I * T_lgain * CW_Gain);
			FFT_Input[i * 2] = FIR_I_In[i];
			phase_adjust = (float) NCO_I * T_xgain;
			FIR_Q_In[i] = (q15_t) (((float) NCO_I + phase_adjust) * rgain * CW_Gain); //
			FFT_Input[i * 2 + 1] = FIR_Q_In[i];
		}

		Process_FIR_I();
		Process_FIR_Q();
		Process_FFT();

		for (i = 0; i < BUFFERSIZE / 2; i++)  //Output FIR filter results to codec
		        {
			Tx0BufferDMA[2 * i] = (int16_t) FIR_I_Out[i];
			Tx0BufferDMA[2 * i + 1] = (int16_t) FIR_Q_Out[i];
		}

	}  //End of Buffer 0 Processing

}  //End of Xmit_CW()

void Xmit_PSK(void)
{
	static q15_t TX_I;
	static q15_t TX_Q;
	q15_t Product1;
	q15_t Product2;
	q15_t TXData;
	static long NCO_phz;
	float m_RMSConstant = 0.03;

	x_NCOphzinc = (PI2 * (double) NCO_Frequency / (double) Sample_Frequency);

	if (DMA_RX_Memory == 0) //Transfer I/Q data and fill FFT buffer on inactive buffer
	        {
		for (i = 0; i < BUFFERSIZE / 2; i++) {
			NCO_phz += (long) (KCONV * (x_NCOphzinc));
			TX_Q = (Sine_table[(NCO_phz >> 4) & 0xFFF]);
			TX_I = (Sine_table[((NCO_phz >> 4) + 0x400) & 0xFFF]);

			Update_PSK();  //S1 & S2 are updated to provide PSK modulation of IQ signals

			Product1 = (q15_t) (m_RMSConstant * S1 * (float) TX_I);
			Product2 = (q15_t) (m_RMSConstant * S2 * (float) TX_Q);

			TXData = Product1 + Product2;

			FIR_I_In[i] = (q15_t) ((float) TXData * T_lgain);
			FFT_Input[i * 2] = FIR_I_In[i];
			phase_adjust = (float) TXData * T_xgain;
			FIR_Q_In[i] = (q15_t) (((float) TXData + phase_adjust) * rgain); //
			FFT_Input[i * 2 + 1] = FIR_Q_In[i];
		}

		Process_FIR_I();
		Process_FIR_Q();
		Process_FFT();

		for (i = 0; i < BUFFERSIZE / 2; i++)				//Output FIR filter results to codec
		{
			Tx1BufferDMA[2 * i] = (int16_t) FIR_I_Out[i];
			Tx1BufferDMA[2 * i + 1] = (int16_t) FIR_Q_Out[i];
		}

	}		//End of Buffer 1 Processing

	else  //Transfer I/Q data and fill FFT buffer on inactive buffer

	{
		for (i = 0; i < BUFFERSIZE / 2; i++) {
			NCO_phz += (long) (KCONV * (x_NCOphzinc));
			TX_Q = (Sine_table[(NCO_phz >> 4) & 0xFFF]);
			TX_I = (Sine_table[((NCO_phz >> 4) + 0x400) & 0xFFF]);

			Update_PSK();  //S1 & S2 are updated to provide PSK modulation of IQ signals

			Product1 = (q15_t) (m_RMSConstant * S1 * (float) TX_I);
			Product2 = (q15_t) (m_RMSConstant * S2 * (float) TX_Q);

			TXData = Product1 + Product2;

			FIR_I_In[i] = (q15_t) ((float) TXData * T_lgain);
			FFT_Input[i * 2] = FIR_I_In[i];
			phase_adjust = (float) TXData * T_xgain;
			FIR_Q_In[i] = (q15_t) (((float) TXData + phase_adjust) * rgain); //
			FFT_Input[i * 2 + 1] = FIR_Q_In[i];
		}

		Process_FIR_I();
		Process_FIR_Q();
		Process_FFT();

		for (i = 0; i < BUFFERSIZE / 2; i++)  //Output FIR filter results to codec
		{
			Tx0BufferDMA[2 * i] = (int16_t) FIR_I_Out[i];
			Tx0BufferDMA[2 * i + 1] = (int16_t) FIR_Q_Out[i];
		}

	}  //End of Buffer 0 Processing

}  //End of Xmit_PSK()

