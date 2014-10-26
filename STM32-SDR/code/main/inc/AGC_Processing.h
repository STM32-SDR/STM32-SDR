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


extern float   AGC_Mag;
extern int		RSL_Mag;

extern int PGAGAIN0;
extern int dac_gain;
extern int	AGC_Signal;

extern int AGC_On;
extern int Old_PGAGAIN0;
extern int Old_dac_gain;

extern int RSL_Cal;
