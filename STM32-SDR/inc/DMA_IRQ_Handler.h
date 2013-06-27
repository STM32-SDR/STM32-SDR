/*
 * DMA_IRQ_Handler.h
 *
 *  Created on: Nov 8, 2012
 *      Author: CharleyK
 */

void DMA1_Stream0_IRQHandler(void);
void Rcvr_DSP(void);
void Xmit_SSB(void);
void Xmit_CW(void);
void Xmit_PSK(void);

extern int16_t DSP_Flag;
float rgain;
float R_lgain;
float R_xgain;
float T_lgain;
float T_xgain;
float phase_adjust;
float CW_Gain;

int16_t Tx_Flag;

double x_NCOphzinc;
uint8_t Key_Down;
