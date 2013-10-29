/*
 * AGC_Processing.h
 *
 *  Created on: Sep 6, 2013
 *      Author: CharleyK
 */



void Proc_AGC(void);
void Init_AGC (void);

// Global variables exposed by AGC module
extern int   PGAGain;
extern int   AGC_Mode;
extern float DDeltaPGA;


float   AGC_Mag;
int		RSL_Mag;

int PGAGAIN0;
int dac_gain;
int	AGC_Signal;

int AGC_On;
int Old_PGAGAIN0;
int Old_dac_gain;

int RSL_Cal;
