/*
 * Driver for teh ASS7843 touchscreen controller
 * should be used with TSHal.c for calibration
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


#include "TSDriver_ADS7843.h"
#include "stm32f4xx.h"
//example error correction
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_exti.h"
#include "misc.h"
#include "stm32f4xx_spi.h"
#include	"stm32f4xx_syscfg.h"
/* Private define ------------------------------------------------------------*/
/* AD channel selection command and register */
#define	CHX 	0x90 	/* channel Y+ selection command */
#define	CHY 	0xd0	/* channel X+ selection command*/

#define TP_CS(x)	x ? GPIO_SetBits(GPIOB,GPIO_Pin_12): GPIO_ResetBits(GPIOB,GPIO_Pin_12)
#define TP_INT_IN   GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_6)


/* Store event flag, and data. -----------------------------------------------*/
volatile static _Bool s_isTouchEventTriggered = 0;
static uint16_t s_touchEventX = 0;
static uint16_t s_touchEventY = 0;


/* Private function prototypes -----------------------------------------------*/
static _Bool Read_Ads7846(uint16_t *px, uint16_t *py);
static void touch_interrupt_init(void);
static void DelayUS(vu32 cnt);


/* Private define ------------------------------------------------------------*/
#define TOUCH_CLOSENESS_THRESHOLD 80


/*******************************************************************************
 * Function Name  : TP_Init
 * Description    : ADS7843 Port initialization
 * Input          : None
 * Output         : None
 * Return         : None
 * Attention		 : None
 *******************************************************************************/
void TSDriver_Initialize(void)
{
	SPI_InitTypeDef SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStruct;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOD, ENABLE);

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_SPI2 );    //sclk	10	 13
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_SPI2 );	//mýso	11	 14
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2 );	//mosý	12	 15

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

	SPI_I2S_DeInit(SPI2 );
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;

	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;	//SPI_CPOL_Low 	 SPI_CPOL_High
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;

	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;   //SPI_NSS_Hard	 //SPI_NSS_Soft
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256; 	//16
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI2, &SPI_InitStructure);
	SPI_Cmd(SPI2, ENABLE);

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12; // 3
	GPIO_Init(GPIOB, &GPIO_InitStruct);    // d

	TP_CS(1);

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6; //Pin 6 is IRQ
	GPIO_Init(GPIOD, &GPIO_InitStruct);

	touch_interrupt_init();
}

static void touch_interrupt_init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource6 );
	EXTI_InitStructure.EXTI_Line = EXTI_Line6;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;

	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}

/* ****************************************************************
 *   Public access routines
 * ****************************************************************/
_Bool TSDriver_HasNewTouchEvent(void)
{
	return s_isTouchEventTriggered;
}
void  TSDriver_GetRawTouchEventCoords(uint16_t *pX, uint16_t *pY)
{
	*pX = s_touchEventX;
	*pY = s_touchEventY;
}
void  TSDriver_ClearRawTouchEvent(void)
{
	s_isTouchEventTriggered = 0;
}

void TSDriver_HandleTouchInterrupt(void)
{
	uint16_t x, y;
	if (Read_Ads7846(&x, &y)) {
		// It was a good event, store the values
		s_touchEventX = x;
		s_touchEventY = y;
		s_isTouchEventTriggered = 1;
	}
}




/*******************************************************************************
 * Access Routines for the touchscreen A2D
 *******************************************************************************/



/*******************************************************************************
 * Function Name  : DelayUS
 * Description    : delay 1us
 * Input          : - cnt: delay value
 * Output         : None
 * Return         : None
 * Attention		 : None
 *******************************************************************************/
static void DelayUS(vu32 cnt)
{
	uint16_t i;
	for (i = 0; i < cnt; i++) {
		uint8_t us = 12;
		while (us--) {
			;
		}
	}
}

/*******************************************************************************
 * Function Name  : WR_CMD
 * Description    : write data to ADS7843
 * Input          : - cmd: data
 * Output         : None
 * Return         : None
 * Attention		 : None
 *******************************************************************************/
static void WR_CMD(uint8_t cmd)
{
	/* Wait for SPI2 Tx buffer empty */
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE ) == RESET)
		;
	/* Send SPI2 data */
	SPI_I2S_SendData(SPI2, cmd);
	/* Wait for SPI2 data reception */
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE ) == RESET)
		;
	/* Read SPI2 received data */
	SPI_I2S_ReceiveData(SPI2 );
}

/*******************************************************************************
 * Function Name  : RD_AD
 * Description    : read adc value
 * Input          : None
 * Output         : None
 * Return         : ADS7843 Return two byte data
 * Attention		 : None
 *******************************************************************************/
static int RD_AD(void)
{
	unsigned short buf, temp;
	/* Wait for SPI2 Tx buffer empty */
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE ) == RESET)
		;
	/* Send SPI2 data */
	SPI_I2S_SendData(SPI2, 0x0000);
	/* Wait for SPI2 data reception */
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE ) == RESET)
		;
	/* Read SPI2 received data */
	temp = SPI_I2S_ReceiveData(SPI2 );

	buf = temp << 8;

	DelayUS(1);

	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE ) == RESET)
		;
	/* Send SPI2 data */
	SPI_I2S_SendData(SPI2, 0x0000);
	/* Wait for SPI2 data reception */
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE ) == RESET)
		;
	/* Read SPI2 received data */

	temp = SPI_I2S_ReceiveData(SPI2 );
	buf |= temp;
	buf >>= 3;
	buf &= 0xfff;

	return buf;
}

/*******************************************************************************
 * Function Name  : Read_X
 * Description    : Read ADS7843 ADC value of X + channel
 * Input          : None
 * Output         : None
 * Return         : return X + channel ADC value
 * Attention		 : None
 *******************************************************************************/
static int Read_X(void)
{

	TP_CS(0);
	DelayUS(1);

	WR_CMD(CHX);
	DelayUS(1);

	int i = RD_AD();

	TP_CS(1);
	return i;
}

/*******************************************************************************
 * Function Name  : Read_Y
 * Description    : Read ADS7843 ADC value of Y + channel
 * Input          : None
 * Output         : None
 * Return         : return Y + channel ADC value
 * Attention		 : None
 *******************************************************************************/
static int Read_Y(void)
{
	TP_CS(0);
	DelayUS(1);

	WR_CMD(CHY);
	DelayUS(1);

	int i = RD_AD();
	TP_CS(1);

	return i;
}

/*******************************************************************************
 * Function Name  : TP_GetAdXY
 * Description    : Read ADS7843 ADC value of X + Y + channel
 * Input          : None
 * Output         : None
 * Return         : return X + Y + channel ADC value
 * Attention		 : None
 *******************************************************************************/
static void TP_GetAdXY(int *x, int *y)
{
	int adx = Read_X();
	DelayUS(1);
	int ady = Read_Y();

	*x = adx;
	*y = ady;
}



/*******************************************************************************
 * Function Name  : Read_Ads7846
 * Description    : X Y obtained after filtering
 * Input          : Pointers to variables to store X and Y values in.
 * Return         : 1 for valid touch, 0 for invalid touch.
 *******************************************************************************/
static _Bool Read_Ads7846(uint16_t *px, uint16_t *py)
{
	int m0, m1, m2, TP_X[1], TP_Y[1], temp[3];
	uint8_t count = 0;
	int buffer[2][9] = { { 0 }, { 0 } }; /* Multiple sampling coordinates X and Y */

	do /* Loop sampling 9 times */
	{
		TP_GetAdXY(TP_X, TP_Y);
		buffer[0][count] = TP_X[0];
		buffer[1][count] = TP_Y[0];
		count++;
	} while (!TP_INT_IN && count < 9); /* when user clicks on the touch screen, TP_INT_IN touchscreen interrupt pin will be set to low. */

	if (count == 9) /* Successful sampling 9, filtering */
	{
		/* In order to reduce the amount of computation, were divided into three groups averaged */
		temp[0] = (buffer[0][0] + buffer[0][1] + buffer[0][2]) / 3;
		temp[1] = (buffer[0][3] + buffer[0][4] + buffer[0][5]) / 3;
		temp[2] = (buffer[0][6] + buffer[0][7] + buffer[0][8]) / 3;
		/* Calculate the three groups of data */
		m0 = temp[0] - temp[1];
		m1 = temp[1] - temp[2];
		m2 = temp[2] - temp[0];
		/* Absolute value of the above difference */
		m0 = m0 > 0 ? m0 : (-m0);
		m1 = m1 > 0 ? m1 : (-m1);
		m2 = m2 > 0 ? m2 : (-m2);
		/* Judge whether the absolute difference exceeds the difference between the threshold, If these three absolute difference exceeds the threshold,
		 The sampling point is judged as outliers, Discard sampling points */
		if (m0 > TOUCH_CLOSENESS_THRESHOLD && m1 > TOUCH_CLOSENESS_THRESHOLD && m2 > TOUCH_CLOSENESS_THRESHOLD) {
			return 0;
		}
		/* Calculating their average value */
		if (m0 < m1) {
			if (m2 < m0) {
				*px = (temp[0] + temp[2]) / 2;
			}
			else {
				*px = (temp[0] + temp[1]) / 2;
			}
		}
		else if (m2 < m1) {
			*px = (temp[0] + temp[2]) / 2;
		}
		else {
			*px = (temp[1] + temp[2]) / 2;
		}

		/* calculate the average value of Y */
		temp[0] = (buffer[1][0] + buffer[1][1] + buffer[1][2]) / 3;
		temp[1] = (buffer[1][3] + buffer[1][4] + buffer[1][5]) / 3;
		temp[2] = (buffer[1][6] + buffer[1][7] + buffer[1][8]) / 3;

		m0 = temp[0] - temp[1];
		m1 = temp[1] - temp[2];
		m2 = temp[2] - temp[0];

		m0 = m0 > 0 ? m0 : (-m0);
		m1 = m1 > 0 ? m1 : (-m1);
		m2 = m2 > 0 ? m2 : (-m2);
		if (m0 > TOUCH_CLOSENESS_THRESHOLD && m1 > TOUCH_CLOSENESS_THRESHOLD && m2 > TOUCH_CLOSENESS_THRESHOLD) {
			return 0;
		}

		if (m0 < m1) {
			if (m2 < m0) {
				*py = (temp[0] + temp[2]) / 2;
			}
			else {
				*py = (temp[0] + temp[1]) / 2;
			}
		}
		else if (m2 < m1) {
			*py = (temp[0] + temp[2]) / 2;
		}
		else {
			*py = (temp[1] + temp[2]) / 2;
		}
		return 1;
	}

	return 0;
}


