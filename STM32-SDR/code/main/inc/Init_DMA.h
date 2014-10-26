/*
 * Header for DMa code
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

//This Buffer size is set to support a 256 bin FFT. The I/Q data is held in a single buffer
// and the data is interleaved in the form (real0, imag0, real1, imag1, real2, imag2....etc)
#define BUFFERSIZE		1024  // change for 512 sampling
#include	"arm_math.h"

extern int16_t Rx0BufferDMA[BUFFERSIZE];
extern int16_t Tx0BufferDMA[BUFFERSIZE];
extern int16_t Rx1BufferDMA[BUFFERSIZE];
extern int16_t Tx1BufferDMA[BUFFERSIZE];

void Audio_DMA_Init(void);

void Audio_DMA_Start(void);
