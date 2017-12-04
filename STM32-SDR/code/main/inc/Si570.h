/*
 * header file for Si570 Subroutines 2/16/2008  John H F Fisher K4JHF
 *
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

//===================================================================

#define	twoto28th		268435456.0
#define	FOSC_MIN		4850000000.0
#define	FOSC_MIN_long	485000000
#define	FOSC_MAX		5670000000.0
#define	FOSC_MAX_long	567000000
#define SI570_Addr		0x55<<1

//===================================================================
void Read_Si570_registers(unsigned char read[]);
void Write_Si570_registers(unsigned char write[]);
void Recall_F0(void);
void Set_HS_DIV_N1(long Freq_long);
void Pack_Si570_registers(unsigned char reg[]);
void Unpack_Si570_registers(unsigned char reg[]);
void Compute_FXTAL(void);
void Output_Frequency(long Freq_Out);
char Large_RFREQ_Change(void);
void Check_SI570(void);
void Si570_Init(void);
_Bool Si570_isEnabled(void);

//===================================================================
//long		FOUT_multiplier, FOUT_divisor, Display_multiplier, Display_divisor;
extern unsigned char si570_read[6];
extern unsigned const char HS_DIV_VALUE_char[6];
extern unsigned long RFREQ_INT, RFREQ_FRAC;
extern float HS_DIV, N1;
extern double RFREQ, Old_RFREQ, FXTAL;
extern long FOUT;  //, delta_freq;
extern double F0;
extern unsigned char SI570_Chk;
//===================================================================

