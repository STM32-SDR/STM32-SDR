/*
 * Code to handle Test Pint for performance testing
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
#include	"DMA_Test_Pins.h"

//This code initializes four outputs to enable testing of the DMA / DSP operation
//These pins are:

//PD-12    Used for observing the DMA Interrupt
//PD-13   Used for observing the DMA I2S Input Stream Buffer Select
//PD-14   Used for observing the DMA I2S Output Stream Buffer Select
//PD-15    Used for observing the processing outside the Interrupt handler

void TEST_GPIO_Init(void)
{
	//Structure Assignment
	GPIO_InitTypeDef GPIO_InitStructure;

	//Turn On GPIO Clock
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	//Define the GPIO Structure
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13; //Pins 14 and 15 removed for TFT

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

	//Init the GPIO Test Pins
	GPIO_Init(GPIOD, &GPIO_InitStructure);
}
