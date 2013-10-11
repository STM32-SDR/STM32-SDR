/*
 * AGC_Processing.h
 *
 *  Created on: Sep 6, 2013
 *      Author: CharleyK
 */

#ifndef _AGC_PROCESSING_H_
#define _AGC_PROCESSING_H_

void Proc_AGC(void);
void Init_AGC (void);

// Global variables exposed by AGC module
extern int   PGAGain;
extern int   AGC_Mode;
extern float DDeltaPGA;

extern float AGC_Mag;
extern int   RSL_Mag;

#endif
