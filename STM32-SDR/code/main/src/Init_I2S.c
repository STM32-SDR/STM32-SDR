/*
 * Code to initialise the I2S streams
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

#include	"stm32f4xx.h"
#include	"stm32f4xx_gpio.h"
#include	"stm32f4xx_rcc.h"
#include 	"stm32f4xx_spi.h"
#include 	"stm32f4xx_dma.h"
#include 	"Init_I2S.h"
#include 	"Init_Codec.h"
#include 	"Init_I2C.h" // reference for Delay();

void Codec_AudioInterface_Init(uint32_t AudioFreq);
void I2S_GPIO_Init(void);

void Codec_AudioInterface_Init(uint32_t AudioFreq)
{
	//Setup I2C comms to codec
	Codec_GPIO_Init();
	I2C_Cntrl_Init();

	//Bring Up Codec via the I2C comms
	Codec_Init();

	//Invoke I2S Pin connections
	I2S_GPIO_Init();

	//Now, get down to defining I2S, Full Duplex, Codec Master Configuration
	I2S_InitTypeDef I2S_InitStructure;

	/* Enable the CODEC_I2S peripheral clock */
	RCC_APB1PeriphClockCmd(CODEC_I2S_CLK, ENABLE);

	/* CODEC_I2S peripheral configuration */
	I2S_InitStructure.I2S_AudioFreq = AudioFreq;
	I2S_InitStructure.I2S_Standard = I2S_Standard_MSB;
	I2S_InitStructure.I2S_DataFormat = I2S_DataFormat_16b;
	I2S_InitStructure.I2S_CPOL = I2S_CPOL_Low;
	I2S_InitStructure.I2S_Mode = I2S_Mode_SlaveTx;

	I2S_InitStructure.I2S_MCLKOutput = I2S_MCLKOutput_Disable;

	/* Initialize the I2S peripheral with the structure above */
	I2S_Init(CODEC_I2S, &I2S_InitStructure);
	I2S_FullDuplexConfig(I2S3ext, &I2S_InitStructure);

	//Turn on the I2S interface
	I2S_Cmd(CODEC_I2S, ENABLE);
	I2S_Cmd(I2S3ext, ENABLE);

}

void I2S_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(CODEC_I2S_GPIO_CLOCK, ENABLE);

	/* CODEC_I2S pins configuration: WS, SCK and SD pins -----------------------------*/
	GPIO_InitStructure.GPIO_Pin = CODEC_I2S_SCK_PIN | CODEC_I2S_SD_PIN | CODEC_I2Sext_SD_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(CODEC_I2S_GPIO, &GPIO_InitStructure);

	/* Connect pins to I2S peripheral  */

	GPIO_PinAFConfig(CODEC_I2S_GPIO, CODEC_I2S_SCK_PINSRC, CODEC_I2S_GPIO_AF );
	GPIO_PinAFConfig(CODEC_I2S_GPIO, CODEC_I2S_SD_PINSRC, CODEC_I2S_GPIO_AF );
	GPIO_PinAFConfig(CODEC_I2S_GPIO, CODEC_I2Sext_SD_PINSRC, CODEC_I2S_GPIO_AF );

	GPIO_InitStructure.GPIO_Pin = CODEC_I2S_WS_PIN ;
	GPIO_Init(CODEC_I2S_WS_GPIO, &GPIO_InitStructure);
	GPIO_PinAFConfig(CODEC_I2S_WS_GPIO, CODEC_I2S_WS_PINSRC, CODEC_I2S_GPIO_AF );

}

