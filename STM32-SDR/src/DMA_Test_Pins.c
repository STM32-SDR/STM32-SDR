/*
 * DMA_Test_Pins.c
 *
 *  Created on: Oct 10, 2012
 *      Author: CharleyK
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

void TEST_GPIO_Init ( void  )	{


	//Structure Assignment
	GPIO_InitTypeDef        GPIO_InitStructure;

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
