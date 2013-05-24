/* Includes ------------------------------------------------------------------*/

#include	"User_Button.h"
#include	"main.h"
#include	"arm_math.h"
#include	"Init_I2S.h"
//#include 	"stm32f4xx_spi.h"
#include	"Init_DMA.h"
#include 	"Init_I2C.h"
#include	"Init_Codec.h"
//#include 	"stm32f4xx_dma.h"
#include	"DMA_Test_Pins.h"
//#include	"stm32f4xx_gpio.h"
#include	"DSP_Processing.h"
#include	"DMA_IRQ_Handler.h"
#include	"PSKDet.h"
#include	"TFT_Display.h"
#include	"touch_pad.h"
#include	"uart.h"
//#include	"xprintf.h"
#include	"SI570.h"
#include	"Encoder_1.h"
#include	"Encoder_2.h"
#include	"ModeSelect.h"
#include	"CW_Mod.h"
#include	"ChangeOver.h"

	uint8_t FFT_Display[256];
	uint8_t FFT_Output[128];
	uint32_t CodecFreq = 8000;
	int16_t Display_Flag;
	int16_t j;
	int16_t i;
	int16_t old_SS1_position;
	int16_t old_SS2_position;
	unsigned char Start = 241;
	unsigned char Stop = 242;

int main(void)	{

	LCD_Init();
	main_delay ( 100 );

	touch_init();
	main_delay ( 100 );

	touch_interrupt_init();
	main_delay ( 100 );

	I2C_GPIO_Init();
	main_delay ( 100 );

	I2C_Cntrl_Init();
	main_delay ( 100 );

	Codec_AudioInterface_Init(CodecFreq);
	main_delay ( 100 );

	Audio_DMA_Init();
	main_delay ( 100 );

	TEST_GPIO_Init ();
	main_delay ( 100 );

	User_Button_Config();
	main_delay ( 100 );

	BT_Flag_Config();
	main_delay ( 100 );

	ResetModem(BPSK_MODE);
	main_delay ( 100 );

	SetRXFrequency (1000);
	main_delay ( 100 );

	SetAFCLimit(1000);
	main_delay ( 100 );

	DSP_Flag = 0;

	Audio_DMA_Start();
	main_delay ( 100 );

	uart_init();
	main_delay ( 100 );

	LCD_StringLine(200,100,"STM32 SDR V1.0");
	LCD_Clear(0x0000);

	main_delay ( 100 );
	Encoder1_GPIO_Config();
	main_delay ( 100 );


	check_SS1();
	old_SS1_position =read_SS1;
	main_delay ( 100 );

	init_encoder1();
	main_delay ( 100 );

	Encoder2_GPIO_Config();
	main_delay ( 100 );

	check_SS2();
	old_SS2_position =read_SS2;
	main_delay ( 100 );

	init_encoder2 ();
	main_delay ( 100 );

	Init_PTT_IO();
	main_delay ( 100 );

	Init_Mode();
	main_delay ( 100 );

	Init_CW_GPIO ();
	main_delay ( 100 );

	while (1)	{

		if (DSP_Flag == 1)

			{for (j = 0; j<128; j++)
			{
			FFT_Output[j] = (uint8_t)(6 * log((float32_t)(FFT_Magnitude[j]+1)));  //Changed for getting right dispaly with SR6.3

			if (FFT_Output[j]>64) FFT_Output[j] = 64;
			FFT_Display [2*j] = FFT_Output[j];
			FFT_Display [2*j+1] = (FFT_Output[j] + FFT_Output[j+1])/2;
			}

			DSP_Flag = 0;
			Display_Flag = 1;
			}

		if (Display_Flag ==1)
			{
			LCD_DrawFFT();
			LCD_StringLine(0,130,(char*)&LCD_buffer[0]);


			Check_BT_Flag ();
			if (BT_Flag ==1)
			{
			LCD_StringLine(0,60,"BT  On");
			LCD_StringLine(0,110, (char*)&received_string[0]);

			//uart_putc( Start);
			///for (j=0;j<128;j++) {
			//uart_putc( (unsigned char)FFT_Display [2*j]);
			///uart_putc( (unsigned char)FFT_Display [2*j+1]);
			//for (j=0;j<39;j++) 	uart_putc(LCD_buffer[j]);
			//uart_putc( Stop);
			}
			else
			LCD_StringLine(0,60,"BT OFF");
			Display_Flag = 0;
			}

		check_SS1();
		if (read_SS1 != old_SS1_position) {
		process_SS1();
		old_SS1_position = read_SS1;
		}
		if (SI570_Chk !=3) process_encoder1();

		check_SS2();
		if (read_SS2 != old_SS2_position) {
		process_SS2();
		old_SS2_position = read_SS2;
		}
		process_encoder2();

		}//while

	} //main


void main_delay (int a)
{
	volatile int i,j;

	for (i=0 ; i < a ; i++)	{

		j++;

		}

	return;

}






