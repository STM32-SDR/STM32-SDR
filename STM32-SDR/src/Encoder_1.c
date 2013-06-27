/*
 * Encoder_1.c
 *
 *  Created on: Feb 25, 2013
 *      Author: CharleyK
 */


#include	"stm32f4xx_rcc.h"
#include	"stm32f4xx_exti.h"
#include	"stm32f4xx_syscfg.h"
#include 	"stm32f4xx_gpio.h"
#include	"arm_math.h"
#include	"TFT_Display.h"
#include	"SI570.h"
#include	"Init_I2C.h"
#include	"Init_Codec.h"
#include	"Encoder_1.h"
#include	"eeprom.h"

EXTI_InitTypeDef EXTI_InitStructure;

int16_t read_SS1;
uint32_t Freq_Set[16];

uint32_t Freq_Max;
uint32_t Freq_Min;

uint32_t Freq_Disp;

int16_t ENC_Sens1;
int8_t DIR1;
uint8_t F_Mult;
uint32_t F_Unit = 100;

uint32_t EEProm_Value;

void Encoder1_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable GPIOC clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	/* Enable SYSCFG clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	/* Configure PC5,6&8 pin as input with Pull Up */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_8 );
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	/* Configure PE3,4,5&6 pin as input with Pull Up */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5
			| GPIO_Pin_6 );
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	/* Enable SYSCFG clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	/* Connect EXTI Line8 to PA8 pin */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource8 );

	/* Configure EXTI Line8 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line8;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Enable and set EXTI Line9-5 Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}

void Set_Initial_SI570_Data(void)
{
	Freq_Set[0] = 14070000;
	Freq_Set[1] = 3580000;
	Freq_Set[2] = 7028000;
	Freq_Set[3] = 7040000;
	Freq_Set[4] = 7070000;
	Freq_Set[5] = 10000000;
	Freq_Set[6] = 10138000;
	Freq_Set[7] = 14200000;
	Freq_Set[8] = 15000000;
	Freq_Set[9] = 18100000;
	Freq_Set[10] = 21070000;
	Freq_Set[11] = 24920000;
	Freq_Set[12] = 28120000;
	Freq_Set[13] = 50250000;
	Freq_Set[14] = 56320000;
	Freq_Set[15] = 4;
}

void init_encoder1(void)
{

	EEProm_Value1 = Read_Long_EEProm(0); //Read the 0 address to see if SI570 data has been stored

	if (EEProm_Value1 != 1234) {
		Set_Initial_SI570_Data();
		Store_SI570_Data();
		Write_Long_EEProm(0, 1234);
	}

	Read_SI570_Data();

	ENC_Sens1 = 2;
	Freq_Min = 2000000;
	Freq_Max = 500000000;
	F0 = (double) Freq_Set[14];
	F_Mult = Freq_Set[15];

	Check_SI570();

	if (SI570_Chk == 3) {
		LCD_StringLine(234, 60, " SI570_?? ");
	}
	else {
		Compute_FXTAL();
		check_SS1();
		if (read_SS1 < 14) {
			process_SS1();
		}
		else {
			FOUT = Freq_Set[0] * F_Mult;
			Output_Frequency(FOUT);
			Freq_Disp = Freq_Set[0];
			LCD_StringLine(234, 20, " 20 M PSK ");
			redrawFrequencyOnScreen();
		}
	}

	F_Unit = 100;

}

void process_encoder1(void)
{
	// encoder motion has been detected--determine direction
	static uint8_t old = { 0 };
	static int8_t enc_states[] = { 0, 1, -1, 0, -1, 0, 0, 1, 1, 0, 0, -1, 0, -1, 1, 0 };
	int8_t tempDIR;
	static int8_t DIR0;

	Delay(100);
	old <<= 2;  //remember previous encoder state
	old |= (GPIO_ReadInputData(GPIOC ) >> 5 & 0x03);  //get the current state
	tempDIR = enc_states[(old & 0x0F)];
	DIR0 += tempDIR;
	if ((DIR0 < ENC_Sens1) && (DIR0 > -ENC_Sens1))
		DIR1 = 0;
	if (DIR0 >= ENC_Sens1) {
		DIR1 = 1;
		DIR0 = 0;
	}
	if (DIR0 <= -ENC_Sens1) {
		DIR1 = -1;
		DIR0 = 0;
	}

	if (DIR1 != 0) {
		check_SS1();
		switch (read_SS1) {

		// TODO: If frequency hits min or max it gets locked in.
		// TODO: Remove duplication.
		case 0:
			if ((Freq_Set[read_SS1] > Freq_Min)
					&& (Freq_Set[read_SS1] < Freq_Max)) {
				Freq_Set[read_SS1] += DIR1 * F_Unit;
				FOUT = Freq_Set[read_SS1] * F_Mult;
				if (SI570_Chk != 3)
					Output_Frequency(FOUT);
				Freq_Disp = Freq_Set[read_SS1];
			}
			break;

		case 1:
			if ((Freq_Set[read_SS1] > Freq_Min)
					&& (Freq_Set[read_SS1] < Freq_Max)) {
				Freq_Set[read_SS1] += DIR1 * F_Unit;
				FOUT = Freq_Set[read_SS1] * F_Mult;
				if (SI570_Chk != 3)
					Output_Frequency(FOUT);
				Freq_Disp = Freq_Set[read_SS1];
			}
			break;

		case 2:
			if ((Freq_Set[read_SS1] > Freq_Min)
					&& (Freq_Set[read_SS1] < Freq_Max)) {
				Freq_Set[read_SS1] += DIR1 * F_Unit;
				FOUT = Freq_Set[read_SS1] * F_Mult;
				if (SI570_Chk != 3)
					Output_Frequency(FOUT);
				Freq_Disp = Freq_Set[read_SS1];
			}
			break;

		case 3:
			if ((Freq_Set[read_SS1] > Freq_Min)
					&& (Freq_Set[read_SS1] < Freq_Max)) {
				Freq_Set[read_SS1] += DIR1 * F_Unit;
				FOUT = Freq_Set[read_SS1] * F_Mult;
				if (SI570_Chk != 3)
					Output_Frequency(FOUT);
				Freq_Disp = Freq_Set[read_SS1];
			}
			break;

		case 4:
			if ((Freq_Set[read_SS1] > Freq_Min)
					&& (Freq_Set[read_SS1] < Freq_Max)) {
				Freq_Set[read_SS1] += DIR1 * F_Unit;
				FOUT = Freq_Set[read_SS1] * F_Mult;
				if (SI570_Chk != 3)
					Output_Frequency(FOUT);
				Freq_Disp = Freq_Set[read_SS1];
			}
			break;

		case 5:
			if ((Freq_Set[read_SS1] > Freq_Min)
					&& (Freq_Set[read_SS1] < Freq_Max)) {
				Freq_Set[read_SS1] += DIR1 * F_Unit;
				FOUT = Freq_Set[read_SS1] * F_Mult;
				if (SI570_Chk != 3)
					Output_Frequency(FOUT);
				Freq_Disp = Freq_Set[read_SS1];
			}
			break;

		case 6:
			if ((Freq_Set[read_SS1] > Freq_Min)
					&& (Freq_Set[read_SS1] < Freq_Max)) {
				Freq_Set[read_SS1] += DIR1 * F_Unit;
				FOUT = Freq_Set[read_SS1] * F_Mult;
				if (SI570_Chk != 3)
					Output_Frequency(FOUT);
				Freq_Disp = Freq_Set[read_SS1];
			}
			break;

		case 7:
			if ((Freq_Set[read_SS1] > Freq_Min)
					&& (Freq_Set[read_SS1] < Freq_Max)) {
				Freq_Set[read_SS1] += DIR1 * F_Unit;
				FOUT = Freq_Set[read_SS1] * F_Mult;
				if (SI570_Chk != 3)
					Output_Frequency(FOUT);
				Freq_Disp = Freq_Set[read_SS1];
			}
			break;

		case 8:
			if ((Freq_Set[read_SS1] > Freq_Min)
					&& (Freq_Set[read_SS1] < Freq_Max)) {
				Freq_Set[read_SS1] += DIR1 * F_Unit;
				FOUT = Freq_Set[read_SS1] * F_Mult;
				if (SI570_Chk != 3)
					Output_Frequency(FOUT);
				Freq_Disp = Freq_Set[read_SS1];
			}
			break;

		case 9:
			if ((Freq_Set[read_SS1] > Freq_Min)
					&& (Freq_Set[read_SS1] < Freq_Max)) {
				Freq_Set[read_SS1] += DIR1 * F_Unit;
				FOUT = Freq_Set[read_SS1] * F_Mult;
				if (SI570_Chk != 3)
					Output_Frequency(FOUT);
				Freq_Disp = Freq_Set[read_SS1];
			}
			break;

		case 10:
			if ((Freq_Set[read_SS1] > Freq_Min)
					&& (Freq_Set[read_SS1] < Freq_Max)) {
				Freq_Set[read_SS1] += DIR1 * F_Unit;
				FOUT = Freq_Set[read_SS1] * F_Mult;
				if (SI570_Chk != 3)
					Output_Frequency(FOUT);
				Freq_Disp = Freq_Set[read_SS1];
			}
			break;

		case 11:
			if ((Freq_Set[read_SS1] > Freq_Min)
					&& (Freq_Set[read_SS1] < Freq_Max)) {
				Freq_Set[read_SS1] += DIR1 * F_Unit;
				FOUT = Freq_Set[read_SS1] * F_Mult;
				if (SI570_Chk != 3)
					Output_Frequency(FOUT);
				Freq_Disp = Freq_Set[read_SS1];
			}
			break;

		case 12:
			if ((Freq_Set[read_SS1] > Freq_Min)
					&& (Freq_Set[read_SS1] < Freq_Max)) {
				Freq_Set[read_SS1] += DIR1 * F_Unit;
				FOUT = Freq_Set[read_SS1] * F_Mult;
				if (SI570_Chk != 3)
					Output_Frequency(FOUT);
				Freq_Disp = Freq_Set[read_SS1];
			}
			break;

		case 13:
			if ((Freq_Set[read_SS1] > Freq_Min)
					&& (Freq_Set[read_SS1] < Freq_Max)) {
				Freq_Set[read_SS1] += DIR1 * F_Unit;
				FOUT = Freq_Set[read_SS1] * F_Mult;
				if (SI570_Chk != 3)
					Output_Frequency(FOUT);
				Freq_Disp = Freq_Set[read_SS1];
			}
			break;

		case 14:
			if (F0 == 56320000.0) {
				F0 = 10000000.0;
			}
			else {
				F0 = 56320000.0;
			}
			if (SI570_Chk != 3)
				Compute_FXTAL();
			Freq_Disp = (uint32_t) F0;
			Freq_Set[14] = (uint32_t) F0;
			break;

		case 15:
			if (F_Mult == 4) {
				F_Mult = 2;
			}
			else {
				F_Mult = 4;
			}
			Freq_Disp = (uint32_t) F_Mult;
			Freq_Set[15] = (uint32_t) F_Mult;
			break;

		} //end of switch read_input

		redrawFrequencyOnScreen();

	}

}   //end of process_encoder

void check_SS1(void)
{
	//Read PE3,4,5,6 as four bit word
	read_SS1 = ~GPIO_ReadInputData(GPIOE ) >> 3 & 0x000F;

}

void process_SS1(void)
{
	//Read PE3,4,5,6 as four bit word
	//read_SS1 = ~GPIO_ReadInputData(GPIOE)>>3 & 0x000F;

	switch (read_SS1) {

	case 0:
		FOUT = Freq_Set[read_SS1] * F_Mult;
		if (SI570_Chk != 3)
			Output_Frequency(FOUT);
		Freq_Disp = Freq_Set[read_SS1];
		LCD_StringLine(234, 20, " 20 M PSK ");
		break;

	case 1:
		FOUT = Freq_Set[read_SS1] * F_Mult;
		if (SI570_Chk != 3)
			Output_Frequency(FOUT);
		Freq_Disp = Freq_Set[read_SS1];
		LCD_StringLine(234, 20, " 80 M PSK ");
		break;

	case 2:
		FOUT = Freq_Set[read_SS1] * F_Mult;
		if (SI570_Chk != 3)
			Output_Frequency(FOUT);
		Freq_Disp = Freq_Set[read_SS1];
		LCD_StringLine(234, 20, " 40 M PSKj");
		break;

	case 3:
		FOUT = Freq_Set[read_SS1] * F_Mult;
		if (SI570_Chk != 3)
			Output_Frequency(FOUT);
		Freq_Disp = Freq_Set[read_SS1];
		LCD_StringLine(234, 20, " 40 M PSKe");

		break;

	case 4:
		FOUT = Freq_Set[read_SS1] * F_Mult;
		if (SI570_Chk != 3)
			Output_Frequency(FOUT);
		Freq_Disp = Freq_Set[read_SS1];
		LCD_StringLine(234, 20, " 40 M PSKu");

		break;

	case 5:
		FOUT = Freq_Set[read_SS1] * F_Mult;
		if (SI570_Chk != 3)
			Output_Frequency(FOUT);
		Freq_Disp = Freq_Set[read_SS1];
		LCD_StringLine(234, 20, "10 MHz WWV");
		break;

	case 6:
		FOUT = Freq_Set[read_SS1] * F_Mult;
		if (SI570_Chk != 3)
			Output_Frequency(FOUT);
		Freq_Disp = Freq_Set[read_SS1];
		LCD_StringLine(234, 20, " 30 M PSK ");
		break;

	case 7:
		FOUT = Freq_Set[read_SS1] * F_Mult;
		if (SI570_Chk != 3)
			Output_Frequency(FOUT);
		Freq_Disp = Freq_Set[read_SS1];
		LCD_StringLine(234, 20, " 20 M SSB ");
		break;

	case 8:
		FOUT = Freq_Set[read_SS1] * F_Mult;
		if (SI570_Chk != 3)
			Output_Frequency(FOUT);
		Freq_Disp = Freq_Set[read_SS1];
		LCD_StringLine(234, 20, "15 MHz WWV");

		break;

	case 9:
		FOUT = Freq_Set[read_SS1] * F_Mult;
		if (SI570_Chk != 3)
			Output_Frequency(FOUT);
		Freq_Disp = Freq_Set[read_SS1];
		LCD_StringLine(234, 20, " 17 M PSK ");

		break;

	case 10:
		FOUT = Freq_Set[read_SS1] * F_Mult;
		if (SI570_Chk != 3)
			Output_Frequency(FOUT);
		Freq_Disp = Freq_Set[read_SS1];
		LCD_StringLine(234, 20, " 15 M PSK ");

		break;

	case 11:
		FOUT = Freq_Set[read_SS1] * F_Mult;
		if (SI570_Chk != 3)
			Output_Frequency(FOUT);
		Freq_Disp = Freq_Set[read_SS1];
		LCD_StringLine(234, 20, " 12 M PSK ");

		break;
	case 12:
		FOUT = Freq_Set[read_SS1] * F_Mult;
		if (SI570_Chk != 3)
			Output_Frequency(FOUT);
		Freq_Disp = Freq_Set[read_SS1];
		LCD_StringLine(234, 20, " 10 M PSK ");

		break;

	case 13:
		FOUT = Freq_Set[read_SS1] * F_Mult;
		if (SI570_Chk != 3)
			Output_Frequency(FOUT);
		Freq_Disp = Freq_Set[read_SS1];
		LCD_StringLine(234, 20, "  6 M PSK ");
		break;

	case 14:
		Freq_Disp = (uint32_t) F0;
		LCD_StringLine(234, 20, " SI570 F0 ");
		break;

	case 15:
		Freq_Disp = (uint32_t) F_Mult;
		LCD_StringLine(234, 20, "SI570 Mult");
		break;

	}

	redrawFrequencyOnScreen();
}

void Increase_Step(void)
{

	switch (F_Unit) {
	case 100000:    //stop here, upper limit
		F_Unit = 100000;
		break;
	case 10000:
		F_Unit = 100000;
		break;
	case 1000:
		F_Unit = 10000;
		break;
	case 100:
		F_Unit = 1000;
		break;
	case 10:
		F_Unit = 100;
		break;
	case 1:
		F_Unit = 10;
		break;
	}  // end switch
} // end Increase_Step

void Decrease_Step(void)
{

	switch (F_Unit) {
	case 100000:
		F_Unit = 10000;
		break;
	case 10000:
		F_Unit = 1000;
		break;
	case 1000:
		F_Unit = 100;
		break;
	case 100:
		F_Unit = 10;
		break;
	case 10:
		F_Unit = 1;
		break;
	case 1:
		F_Unit = 1;  //Lower Limit
		break;
	}  // end switch
} // End Decrease_Step

void redrawFrequencyOnScreen(void)
{
	Plot_Freq(Freq_Disp, 234, 0, F_Unit);
}

