/*
 * Header file for setting codec gains
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

// Min/Max gain constants


#ifndef CODEC_GAINS_H_
#define CODEC_GAINS_H_

#define PGA_GAIN_MIN 0
#define PGA_GAIN_MAX 95
#define DAC_GAIN_MIN -127
#define DAC_GAIN_MAX 48
#define ADC_GAIN_MIN -24
#define ADC_GAIN_MAX 40

void Set_HP_Gain(int HP_gain);
void Set_LO_Gain(int LO_gain);
void Set_PGA_Gain(int PGA_gain);
void Set_ADC_DVC(int ADC_gain);
void Set_DAC_DVC(int DAC_gain);
void Turn_On_Bias(void);
void Turn_Off_Bias(void);
void Disconnect_PGA(void);
void Mute_HP(void);
void Mute_LO(void);
void Connect_IQ_Inputs(void);
void Connect_Microphone_Input(void);
void Connect_Sidetone_Input(void);
void Disconnect_Sidetone_Input(void);
void Sidetone_Key_Up(void);
void Sidetone_Key_Down(void);

#endif /* CODEC_GAINS_H_ */
