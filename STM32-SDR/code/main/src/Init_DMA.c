/*
 * Init_DMA.c
 *
 *  Created on: Oct 4, 2012
 *      Author: CharleyK
 */
#include	"stm32f4xx.h"
#include	"stm32f4xx_rcc.h"
#include 	"stm32f4xx_dma.h"
#include 	"stm32f4xx_spi.h"
#include 	"Init_DMA.h"
#include 	"Init_Codec.h"
#include 	"Init_I2C.h" // reference for Delay();
#include	"misc.h"

//void Audio_DMA_Init(void);

int16_t Rx0BufferDMA[BUFFERSIZE];
int16_t Tx0BufferDMA[BUFFERSIZE];
int16_t Rx1BufferDMA[BUFFERSIZE];
int16_t Tx1BufferDMA[BUFFERSIZE];

void Audio_DMA_Init(void)
{
	DMA_InitTypeDef DMA_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

	//Setup Input Audio DMA Stream
	DMA_DeInit(DMA1_Stream0 );

	DMA_InitStructure.DMA_Channel = DMA_Channel_3;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) (I2S3ext_BASE + 0x0C);
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t) Rx0BufferDMA;
	DMA_InitStructure.DMA_BufferSize = BUFFERSIZE;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;

	DMA_Init(DMA1_Stream0, &DMA_InitStructure);

	DMA_DoubleBufferModeConfig(DMA1_Stream0, (uint32_t) Rx1BufferDMA,
			DMA_Memory_0 );
	DMA_DoubleBufferModeCmd(DMA1_Stream0, ENABLE);

	//Turn On Receive I2S to DMA Connection
	SPI_I2S_DMACmd(I2S3ext, SPI_I2S_DMAReq_Rx, ENABLE);

	//Setup Output Audio DMA Stream
	DMA_DeInit(DMA1_Stream5 );
	DMA_InitStructure.DMA_Channel = DMA_Channel_0;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) (SPI3_BASE + 0x0C);
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t) Tx0BufferDMA;
	DMA_InitStructure.DMA_BufferSize = BUFFERSIZE;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;

	DMA_Init(DMA1_Stream5, &DMA_InitStructure);

	DMA_DoubleBufferModeConfig(DMA1_Stream5, (uint32_t) Tx1BufferDMA,
			DMA_Memory_0 );
	DMA_DoubleBufferModeCmd(DMA1_Stream5, ENABLE);

	//Turn On I2S to DMA Transmit Connection
	SPI_I2S_DMACmd(SPI3, SPI_I2S_DMAReq_Tx, ENABLE);

	Delay(10000);

	//Startup DMA Processing
	DMA_Cmd(DMA1_Stream0, ENABLE);
	DMA_Cmd(DMA1_Stream5, ENABLE);
	Delay(10000);

	//Setup & Turn On DMA General Interrupt
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	Delay(10000);

	//Turn On Stream0 Transaction Complete Interrupt )
	//DMA_ITConfig(DMA1_Stream0, DMA_IT_TC, ENABLE);
}



void Audio_DMA_Start(void)
{
	DMA_ITConfig(DMA1_Stream0, DMA_IT_TC, ENABLE);
}
