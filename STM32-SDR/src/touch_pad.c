/*======================================================================*/

/*======================================================================*/

#include "touch_pad.h"
#include "TFT_Display.h"
#include "stm32f4xx_it.h"
#include "stm32f4xx_spi.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_rcc.h"
#include "misc.h"
#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32f4xx_syscfg.h"
#include "arm_math.h"
#include <stdio.h>
#include "PSKDet.h"
#include "Encoder_1.h"
#include "Encoder_2.h"
#include "eeprom.h"
#include "DMA_IRQ_Handler.h"
#include "ChangeOver.h"
#include "ModeSelect.h"

uint16_t TouchFlag;
int16_t TouchCount = 0;
int16_t DebounceCount = 0;

uint16_t X_Point;
uint16_t Y_Point;

uint16_t Mode_Test = 0;

//Coordinate * Ptr;
/*======================================================================*/

/* Private variables ---------------------------------------------------------*/
//Matrix matrix ;
//Coordinate  display ;
/* LCD coordinates corresponding sampling AD value. for example£ºLCD coordinates 45, 45 X Y sampling ADC value 3388, 920 */
//Coordinate ScreenSample[3];
/* LCD Coordinate */
//Coordinate DisplaySample[3] =   {
// { 45, 45 },
//{ 45, 270},
//  { 190,190}
//	                            } ;
/* Private define ------------------------------------------------------------*/
//#define THRESHOLD 2   /* threshold */
#define THRESHOLD 20

/*******************************************************************************
 * Function Name  : ADS7843_SPI_Init
 * Description    : ADS7843 SPI Initialization
 * Input          : None
 * Output         : None
 * Return         : None
 * Attention		 : None
 *******************************************************************************/

/*******************************************************************************
 * Function Name  : TP_Init
 * Description    : ADS7843 Port initialization
 * Input          : None
 * Output         : None
 * Return         : None
 * Attention		 : None
 *******************************************************************************/
void touch_init(void)
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

}

void touch_interrupt_init(void)
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
int Read_X(void)
{
	int i;

	TP_CS(0);

	DelayUS(1);

	WR_CMD(CHX);

	DelayUS(1);

	i = RD_AD();

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
int Read_Y(void)
{
	int i;

	TP_CS(0);

	DelayUS(1);

	WR_CMD(CHY);

	DelayUS(1);

	i = RD_AD();

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
void TP_GetAdXY(int *x, int *y)
{
	int adx, ady;

	adx = Read_X();

	DelayUS(1);

	ady = Read_Y();

	*x = adx;
	*y = ady;
}

/*******************************************************************************
 * Function Name  : TP_DrawPoint
 * Description    : Draw point Must have a LCD driver
 * Input          : - Xpos: Row Coordinate
 *                  - Ypos: Line Coordinate
 * Output         : None
 * Return         : None
 * Attention		 : None
 *******************************************************************************/
//void TP_DrawPoint(uint16_t Xpos,uint16_t Ypos)
//{
//  LCD_SetPoint(Xpos,Ypos,0xf800);     /* Center point */
//  LCD_SetPoint(Xpos+1,Ypos,0xf800);
//  LCD_SetPoint(Xpos,Ypos+1,0xf800);
//  LCD_SetPoint(Xpos+1,Ypos+1,0xf800);
//}
/*******************************************************************************
 * Function Name  : DrawCross
 * Description    : specified coordinates painting crosshairs
 * Input          : - Xpos: Row Coordinate
 *                  - Ypos: Line Coordinate
 * Output         : None
 * Return         : None
 * Attention		 : None
 *******************************************************************************/

Coordinate screen;
/*******************************************************************************
 * Function Name  : Read_Ads7846
 * Description    : X Y obtained after filtering
 * Input          : None
 * Output         : None
 * Return         : Coordinate Structure address
 * Attention		 : None
 *******************************************************************************/
Coordinate *Read_Ads7846(void)
{
	//static Coordinate  screen;
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
		if (m0 > THRESHOLD && m1 > THRESHOLD && m2 > THRESHOLD) {
			return 0;
		}
		/* Calculating their average value */
		if (m0 < m1) {
			if (m2 < m0) {
				screen.x = (temp[0] + temp[2]) / 2;
			}
			else {
				screen.x = (temp[0] + temp[1]) / 2;
			}
		}
		else if (m2 < m1) {
			screen.x = (temp[0] + temp[2]) / 2;
		}
		else {
			screen.x = (temp[1] + temp[2]) / 2;
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
		if (m0 > THRESHOLD && m1 > THRESHOLD && m2 > THRESHOLD) {
			return 0;
		}

		if (m0 < m1) {
			if (m2 < m0) {
				screen.y = (temp[0] + temp[2]) / 2;
			}
			else {
				screen.y = (temp[0] + temp[1]) / 2;
			}
		}
		else if (m2 < m1) {
			screen.y = (temp[0] + temp[2]) / 2;
		}
		else {
			screen.y = (temp[1] + temp[2]) / 2;
		}
		return &screen;
	}

	return 0;
}

void Draw_Touch_Point(uint16_t x, uint16_t y)
{
	LCD_DrawUniLine(x - 12, y, x + 13, y);
	LCD_DrawUniLine(x, y - 12, x, y + 13);
	Pixel(x + 1, y + 1, RED);
	Pixel(x - 1, y + 1, RED);
	Pixel(x + 1, y - 1, RED);
	Pixel(x - 1, y - 1, RED);
	LCD_DrawCircle(x, y, 6);
}

void GetTouchPoint(void)
{

	uint16_t X_Raw;
	uint16_t Y_Raw;

	Read_Ads7846();

	X_Raw = screen.x;
	Y_Raw = screen.y;

	X_Point = (int) ((float) (3873 - X_Raw) / 11.11);
	Y_Point = (int) (((float) (Y_Raw - 212) / 15.13));

	//Update PSK NCO Frequency
	if ((Y_Point > 176) && (X_Point > 39) && (X_Point < 280)) {
		NCO_Frequency = (double) ((float) ((X_Point - 40) + 8) * 15.625);
		Plot_Integer((int) NCO_Frequency, 150, 150);
		SetRXFrequency(NCO_Frequency);
	}

	//Update Frequency Step
	if ((X_Point > 234) && (Y_Point < 20)) {
		if (X_Point < 277)
			TouchCount++;
		if (X_Point > 277)
			TouchCount--;

		if (TouchCount == 2) {  //Increase Step
			Increase_Step();
			TouchCount = 0;
		} //end Increase step

		if (TouchCount == -2) { //Decrease Step
			Decrease_Step();
			TouchCount = 0;
		} // end Decrease Step
	}  //End of Frequency Step

	if ((X_Point > 320) && (Y_Point < 48)) {  //Store Default Freq & IQ
		Store_Defaults();  //Both Encoder 1 & 2 Defaults Stored, see Encoder_2.c
	}  //End of Store Defaults

	if ((X_Point > 320) && (Y_Point > 48) && (Y_Point < 96)) {  //USB Selection
		rgain = -0.5;
		LCD_StringLine(0, 220, "LSB");
	}

	if ((X_Point > 320) && (Y_Point > 96) && (Y_Point < 144)) {  //LSB Selection
		rgain = 0.5;
		LCD_StringLine(0, 220, "USB");
	}

	if ((X_Point > 320) && (Y_Point > 192)) {  //TX Selection

		switch (Mode) {
		case 0:
			Xmit_SSB_Sequence();
			break;
		case 1:
			Xmit_CW_Sequence();
			break;
		case 2:
			Xmit_PSK_Sequence();
			break;
		}  // End of switch

		LCD_StringLine(296, 220, "TX");
	}

	if ((X_Point > 320) && (Y_Point > 144) && (Y_Point < 192)) {  //RX Selection
		Receive_Sequence();
		//Tx_Flag=0;
		LCD_StringLine(296, 220, "RX");
	}

	if ((X_Point > 88) && (X_Point < 128) && (Y_Point < 20) && (Tx_Flag == 0)) { //SSB Mode Selection
		Mode = 0;
		Set_Mode_Display();
	}

	if ((X_Point > 144) && (X_Point < 176) && (Y_Point < 20)
			&& (Tx_Flag == 0)) {  //CW Mode Selection
		Mode = 1;
		Set_Mode_Display();
	}

	if ((X_Point > 188) && (X_Point < 228) && (Y_Point < 20)
			&& (Tx_Flag == 0)) {  //PSK Mode Selection
		Mode = 2;
		Set_Mode_Display();
	}

}  //End of Get TouchPoint

void EXTI9_5_IRQHandler(void)
{

	//Handle Touch Screen Interrupts
	if (EXTI_GetITStatus(EXTI_Line6 ) != RESET) {
		GetTouchPoint();
		EXTI_ClearITPendingBit(EXTI_Line6 );
	}

	//Handle Encoder #2 PB interrupt
	if (EXTI_GetITStatus(EXTI_Line7 ) != RESET) {
		LCD_StringLine(0, 40, "Store   IQ");
		DelayUS(10);
		Store_IQ_Data();
		DelayUS(10);
		EXTI_ClearITPendingBit(EXTI_Line7 );
	}

	//Handle Encoder #1 PB interrupt
	if (EXTI_GetITStatus(EXTI_Line8 ) != RESET) {
		LCD_StringLine(234, 40, "Store Freq");
		DelayUS(10);
		Store_SI570_Data();
		DelayUS(10);
		EXTI_ClearITPendingBit(EXTI_Line8 );
	}
}

void TouchPanel_Calibrate(void)
{
	//Need to create calibration routine based on workbook method

}

