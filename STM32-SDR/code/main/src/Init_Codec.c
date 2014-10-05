/*
 * Code for Initialisation of CODEC
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

#include	"stm32f4xx_gpio.h"
#include	"stm32f4xx_rcc.h"
#include 	"stm32f4xx_i2c.h"
#include 	"Init_Codec.h"
#include 	"Init_I2C.h"

void Codec_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable Audio Reset GPIO clock */
	RCC_AHB1PeriphClockCmd(AUDIO_RESET_GPIO_CLK, ENABLE);

	/* Audio reset pin configuration -------------------------------------------------*/
	GPIO_InitStructure.GPIO_Pin = AUDIO_RESET_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(AUDIO_RESET_GPIO, &GPIO_InitStructure);
}

void Codec_Reset(void)
{
	/* Power Down the codec */
	GPIO_WriteBit(AUDIO_RESET_GPIO, AUDIO_RESET_PIN, Bit_RESET);

	/* wait for a delay to insure registers erasing */
	Delay(CODEC_RESET_DELAY);

	/* Power on the codec */
	GPIO_WriteBit(AUDIO_RESET_GPIO, AUDIO_RESET_PIN, Bit_SET);
}

//=====================================================================*/

void Codec_Init(void)
{
	Codec_Reset();

	// Point to page 0
	I2C_WriteRegister(CODEC_ADDRESS, 0x00, 0x00);
	Delay(Codec_Pause);

	//software reset codec
	I2C_WriteRegister(CODEC_ADDRESS, 0x01, 0x01);
	Delay(Codec_Pause);

	//***********************************************************************
	// Power System  Config and Power Up
	//***********************************************************************

	// Point to page 1
	I2C_WriteRegister(CODEC_ADDRESS, 0x00, 0x01);
	Delay(Codec_Pause);

	//Disable crude AVDD generation from DVDD
	I2C_WriteRegister(CODEC_ADDRESS, 0x01, 0x08);
	Delay(Codec_Pause);

	//Enable Analog Blocks and LDO
	I2C_WriteRegister(CODEC_ADDRESS, 0x02, 0x01);
	Delay(Codec_Pause);

	//***********************************************************************
	// PLL and Clocks config and Power Up
	//***********************************************************************
	// Point to page 0
	I2C_WriteRegister(CODEC_ADDRESS, 0x00, 0x00);
	Delay(Codec_Pause);

	//For AIC Master Running of 12 Mhz Clock
	I2C_WriteRegister(CODEC_ADDRESS, 0x1b, 0xcd); //Left Justified MSB
	Delay(Codec_Pause);

	//PLL setting: PLLCLK <- MCLK and CODEC_CLKIN <-PLL CLK
	I2C_WriteRegister(CODEC_ADDRESS, 0x04, 0x03);
	Delay(Codec_Pause);

	//PLL setting: J=8
	I2C_WriteRegister(CODEC_ADDRESS, 0x06, 0x08);
	Delay(Codec_Pause);

	//PLL setting: HI_BYTE(D) for D=0x0780 (1920 decimal)
	I2C_WriteRegister(CODEC_ADDRESS, 0x07, 0x07);
	Delay(Codec_Pause);

	//PLL setting: LO_BYTE(D) for D=0x0780 (1920 decimal)
	I2C_WriteRegister(CODEC_ADDRESS, 0x08, 0x80);
	Delay(Codec_Pause);

	//for 8 KHz sampling
	//For 32 bit clocks per frame in Master mode ONLY
	//BCLK=DAC_CLK/N =(12288000/48) = 256 kHz = 32*fs
	I2C_WriteRegister(CODEC_ADDRESS, 0x1e, 0xb0);
	Delay(Codec_Pause);

	//8 KHz sampling //PLL setting: Power up PLL, P=1 and R=1
	I2C_WriteRegister(CODEC_ADDRESS, 0x05, 0x91);
	Delay(Codec_Pause);

	//8 KHz sampling //Hi_Byte(DOSR) for DOSR = 768 decimal or 0x0300 DAC oversamppling
	I2C_WriteRegister(CODEC_ADDRESS, 0x0d, 0x03);
	Delay(Codec_Pause);

	//Lo_Byte(DOSR) for DOSR = 768 decimal or 0x0300
	I2C_WriteRegister(CODEC_ADDRESS, 0x0e, 0x00);
	Delay(Codec_Pause);

	//AOSR for AOSR = 128 decimal or 0x0080 for decimation filters 1 to 6
	I2C_WriteRegister(CODEC_ADDRESS, 0x14, 0x80);
	Delay(Codec_Pause);

	//Power up NDAC and set NDAC value to 8
	I2C_WriteRegister(CODEC_ADDRESS, 0x0b, 0x88);
	Delay(Codec_Pause);

	//Power up MDAC and set MDAC value to 2
	I2C_WriteRegister(CODEC_ADDRESS, 0x0c, 0x82);
	Delay(Codec_Pause);

	//Power up NADC and set NADC value to 8
	I2C_WriteRegister(CODEC_ADDRESS, 0x12, 0x88);
	Delay(Codec_Pause);

	//for 8 KHz sampling
	//Power up MADC and set MADC value to 12
	I2C_WriteRegister(CODEC_ADDRESS, 0x13, 0x8c);
	Delay(Codec_Pause);

	//********************************************************************

	//******************************************************************/
	//* DAC ROUTING and Power Up
	//**********************************************************
	//Select page 1
	I2C_WriteRegister(CODEC_ADDRESS, 0x00, 0x01);
	Delay(Codec_Pause);

	//LDAC AFIR routed to HPL
	I2C_WriteRegister(CODEC_ADDRESS, 0x0c, 0x08);
	Delay(Codec_Pause);

	//RDAC AFIR routed to HPR
	I2C_WriteRegister(CODEC_ADDRESS, 0x0d, 0x08);
	Delay(Codec_Pause);

	//LDAC AFIR routed to LOL
	I2C_WriteRegister(CODEC_ADDRESS, 0x0e, 0x08);
	Delay(Codec_Pause);

	//RDAC AFIR routed to LOR
	I2C_WriteRegister(CODEC_ADDRESS, 0x0f, 0x08);
	Delay(Codec_Pause);

	//Select page 0
	I2C_WriteRegister(CODEC_ADDRESS, 0x00, 0x00);
	Delay(Codec_Pause);

	//Left vol=right vol
	I2C_WriteRegister(CODEC_ADDRESS, 0x40, 0x02);
	Delay(Codec_Pause);

	//Left DAC gain ; Right tracks Left, Zero db gain
	I2C_WriteRegister(CODEC_ADDRESS, 0x41, 0x00);
	Delay(Codec_Pause);

	//Power up left,right data paths and set channel
	I2C_WriteRegister(CODEC_ADDRESS, 0x3f, 0xd4);
	Delay(Codec_Pause);

	//Select page 1
	I2C_WriteRegister(CODEC_ADDRESS, 0x00, 0x01);
	Delay(Codec_Pause);

	//Unmute HPL , set gain = 15 db
	I2C_WriteRegister(CODEC_ADDRESS, 0x10, 0x0f);
	Delay(Codec_Pause);

	//Unmute HPR , set gain = 15 db
	I2C_WriteRegister(CODEC_ADDRESS, 0x11, 0x0f);
	Delay(Codec_Pause);

	//Unmute LOL , set gain
	I2C_WriteRegister(CODEC_ADDRESS, 0x12, 0x00);
	Delay(Codec_Pause);

	//Unmute LOR , set gain
	I2C_WriteRegister(CODEC_ADDRESS, 0x13, 0x00);
	Delay(Codec_Pause);

	I2C_WriteRegister(CODEC_ADDRESS, 0x3c, 0x80);  //PGAr 0 db Gain
	Delay(Codec_Pause);

	I2C_WriteRegister(CODEC_ADDRESS, 0x19, 0x27);  //MAR -30 db Gain
	Delay(Codec_Pause);

	//Power up HPL,HPR,LOL,LOR
	I2C_WriteRegister(CODEC_ADDRESS, 0x09, 0x3d); // added MAR
	Delay(Codec_Pause);

	//***********************************************************************
	//* ADC ROUTING and Power Up
	//***********************************************************************
	//Select page 1
	I2C_WriteRegister(CODEC_ADDRESS, 0x00, 0x01);
	Delay(Codec_Pause);

	// IN2_L to LADC_P through 40 kohm
	I2C_WriteRegister(CODEC_ADDRESS, 0x34, 0x30);
	Delay(Codec_Pause);

	//IN2_R to RADC_P through 40 kohmm
	I2C_WriteRegister(CODEC_ADDRESS, 0x37, 0x30);
	Delay(Codec_Pause);

	//CM_1 (common mode) to LADC_M through 40 kohm
	I2C_WriteRegister(CODEC_ADDRESS, 0x36, 0x03);
	Delay(Codec_Pause);

	//CM_1 (common mode) to RADC_M through 40 kohm
	I2C_WriteRegister(CODEC_ADDRESS, 0x39, 0x03);
	Delay(Codec_Pause);

	//MIC_PGA_L unmute
	I2C_WriteRegister(CODEC_ADDRESS, 0x3b, 0x3c);  //30 db Gain
	Delay(Codec_Pause);

	//MIC_PGA_R unmute, 20 db Gain
	I2C_WriteRegister(CODEC_ADDRESS, 0x3c, 0x3c);  //30 db Gain
	Delay(Codec_Pause);

	//Select page 0
	I2C_WriteRegister(CODEC_ADDRESS, 0x00, 0x00);
	Delay(Codec_Pause);

	//Powerup Left and Right ADC
	I2C_WriteRegister(CODEC_ADDRESS, 0x51, 0xc0);
	Delay(Codec_Pause);

	//Unmute Left and Right ADC
	I2C_WriteRegister(CODEC_ADDRESS, 0x52, 0x00);
	Delay(Codec_Pause);

} //End of Codec Init Procedure

