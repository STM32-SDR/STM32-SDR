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
extern float rgain;
extern float R_lgain;
extern float R_xgain;
extern float T_lgain;
extern float T_xgain;
extern float phase_adjust;

extern int16_t Tx_Flag;

double x_NCOphzinc;
