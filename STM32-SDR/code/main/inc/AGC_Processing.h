/*
 * AGC_Processing.h
 *
 *  Created on: Sep 6, 2013
 *      Author: CharleyK
 */
void Proc_AGC(void);
void Init_AGC (void);

int PGAGain;
int AGC_Mode;
float   DDeltaPGA;

float   AGC_Mag;
int		RSL_Mag;
