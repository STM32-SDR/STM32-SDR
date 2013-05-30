/*
 * ChangeOver.c
 *
 *  Created on: Apr 3, 2013
 *      Author: CharleyK
 */

#include	"ChangeOver.h"
#include	"Encoder_2.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "Codec_Gains.h"
#include "Init_I2C.h"  //Referenced for Delay(n);
#include "PSKMod.h"
#include "DMA_IRQ_Handler.h"

void Receive_Sequence(void)
{
	Mute_HP();
	Mute_LO();
	Tx_Flag = 0;
	GPIO_WriteBit(GPIOD, GPIO_Pin_3, Bit_SET);	//Make PTT_Out High, Remember FET Inversion
	Disconnect_PGA();
	Connect_IQ_Inputs();
	Set_DAC_DVC(IQData[0]);  //RxAudio
	Set_PGA_Gain(IQData[1]);  //RxRF
	Set_ADC_DVC(0);  //was -20 using Milt's AGC scheme
	Set_HP_Gain(6);
}

void Xmit_SSB_Sequence(void)
{
	Mute_HP();
	Mute_LO();
	Tx_Flag = 1;
	Disconnect_PGA();
	Set_DAC_DVC(0);
	Set_ADC_DVC(0);
	Connect_Microphone_Input();
	Set_PGA_Gain(IQData[2]);  //(SSB_Level)
	GPIO_WriteBit(GPIOD, GPIO_Pin_3, Bit_RESET);  //Make PTT_Out Low,Remember FET Inversion
	Delay(1000);
	Set_LO_Gain(24);
}

void Xmit_CW_Sequence(void)
{
	CW_Gain = 0.0;  //For Testing
	Mute_HP();
	Mute_LO();
	Tx_Flag = 1;
	Disconnect_PGA();
	Set_DAC_DVC(IQData[3]);  //(CW_Level)
	GPIO_WriteBit(GPIOD, GPIO_Pin_3, Bit_RESET);  //Make PTT_Out Low,Remember FET Inversion
	Delay(1000);
	Set_LO_Gain(20);
	Set_HP_Gain(IQData[5]);  //ST_Level
}

void Xmit_PSK_Sequence(void)
{
	Mute_HP();
	Mute_LO();
	Tx_Flag = 1;
	Disconnect_PGA();
	Set_DAC_DVC(IQData[4]);  //(PSK_Level)
	GPIO_WriteBit(GPIOD, GPIO_Pin_3, Bit_RESET);  //Make PTT_Out Low,Remember FET Inversion
	Delay(1000);
	Set_LO_Gain(20);
	Set_HP_Gain(IQData[5]);  //ST_Level
}

void Init_PTT_IO(void)
{

	GPIO_InitTypeDef GPIO_InitStruct;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2; // Pin 2 is PTT_In
	GPIO_Init(GPIOD, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3; //  Pin 3 controls the FET-Gate for PTT_Out
	GPIO_Init(GPIOD, &GPIO_InitStruct);

}
