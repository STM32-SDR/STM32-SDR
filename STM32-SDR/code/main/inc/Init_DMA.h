/*
 * Init_DMA.h
 *
 *  Created on: Oct 4, 2012
 *      Author: CharleyK
 */

//This Buffer size is set to support a 256 bin FFT. The I/Q data is held in a single buffer
// and the data is interleaved in the form (real0, imag0, real1, imag1, real2, imag2....etc)
#define BUFFERSIZE		1024  // change for 512 sampling
#include	"arm_math.h"

int16_t Rx0BufferDMA[BUFFERSIZE];
int16_t Tx0BufferDMA[BUFFERSIZE];
int16_t Rx1BufferDMA[BUFFERSIZE];
int16_t Tx1BufferDMA[BUFFERSIZE];

void Audio_DMA_Init(void);

void Audio_DMA_Start(void);
