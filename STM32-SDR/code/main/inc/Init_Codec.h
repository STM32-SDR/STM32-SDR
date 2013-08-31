/*
 * Header file for CODEC routines used in initialisation
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

/*
 *
 *
 *  Created on: Sep 27, 2012
 *      Author: CharleyK
 */
#define CODEC_ADDRESS                   0x18<<1
#define AUDIO_RESET_GPIO_CLK           RCC_AHB1Periph_GPIOB
#define AUDIO_RESET_PIN                GPIO_Pin_0
#define AUDIO_RESET_GPIO               GPIOB
#define Codec_Pause						1

#define CODEC_RESET_DELAY               0x4FF //Delay for the Codec to be correctly reset
void Codec_GPIO_Init(void);
void Codec_Reset(void);
void Codec_Init(void);

