/*
 * Header file for FIR Coefficients use to do all the DSP filtering
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


#define NUM_FIR_COEF	128
q15_t coeff_fir_I[NUM_FIR_COEF];

q15_t coeff_fir_Q[NUM_FIR_COEF];

//chh PSK Filter Coefficients
q15_t Dec4LPCoef[35];

q15_t FIRBitCoef[65];

q15_t FIRFreqCoef[65];
//chh PSK Filter Coefficients
