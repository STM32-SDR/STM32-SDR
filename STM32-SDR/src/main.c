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

/*
 * PROTOTYPES
 */
void initializeHardware(void);
void displaySplashScreen(void);


/*
 * FUNCTIONS
 */
int main(void)
{

	initializeHardware();

	while (1) {

		if (DSP_Flag == 1) {
			for (j = 0; j < 128; j++) {
				//Changed for getting right display with SR6.3
				FFT_Output[j] = (uint8_t) (6 * log((float32_t) (FFT_Magnitude[j] + 1)));

				if (FFT_Output[j] > 64)
					FFT_Output[j] = 64;
				FFT_Display[2 * j] = FFT_Output[j];
				FFT_Display[2 * j + 1] = (FFT_Output[j] + FFT_Output[j + 1]) / 2;
			}

			DSP_Flag = 0;
			Display_Flag = 1;
		}

		if (Display_Flag == 1) {
			LCD_DrawFFT();
			LCD_StringLine(0, 130, (char*) &LCD_buffer[0]);

			Check_BT_Flag();
			if (BT_Flag == 1) {
				LCD_StringLine(0, 60, "BT  On");
				LCD_StringLine(0, 110, (char*) &received_string[0]);

				//uart_putc( Start);
				///for (j=0;j<128;j++) {
				//uart_putc( (unsigned char)FFT_Display [2*j]);
				///uart_putc( (unsigned char)FFT_Display [2*j+1]);
				//for (j=0;j<39;j++) 	uart_putc(LCD_buffer[j]);
				//uart_putc( Stop);
			}
			else
				LCD_StringLine(0, 60, "BT OFF");
			Display_Flag = 0;
		}

		check_SS1();
		if (read_SS1 != old_SS1_position) {
			process_SS1();
			old_SS1_position = read_SS1;
		}
		if (SI570_Chk != 3)
			process_encoder1();

		check_SS2();
		if (read_SS2 != old_SS2_position) {
			process_SS2();
			old_SS2_position = read_SS2;
		}
		process_encoder2();

	} //while

} //main

void initializeHardware(void)
{
	const int SETUP_DELAY = 100;

	LCD_Init();
	main_delay(SETUP_DELAY);

	touch_init();
	main_delay(SETUP_DELAY);

	touch_interrupt_init();
	main_delay(SETUP_DELAY);

	I2C_GPIO_Init();
	main_delay(SETUP_DELAY);

	I2C_Cntrl_Init();
	main_delay(SETUP_DELAY);

	Codec_AudioInterface_Init(CodecFreq);
	main_delay(SETUP_DELAY);

	Audio_DMA_Init();
	main_delay(SETUP_DELAY);

	TEST_GPIO_Init();
	main_delay(SETUP_DELAY);

	User_Button_Config();
	main_delay(SETUP_DELAY);

	BT_Flag_Config();
	main_delay(SETUP_DELAY);

	ResetModem(BPSK_MODE);
	main_delay(SETUP_DELAY);

	SetRXFrequency(1000);
	main_delay(SETUP_DELAY);

	SetAFCLimit(1000);
	main_delay(SETUP_DELAY);

	DSP_Flag = 0;

	Audio_DMA_Start();
	main_delay(SETUP_DELAY);

	uart_init();
	main_delay(SETUP_DELAY);

	displaySplashScreen();
	LCD_Clear(0x0000);

	main_delay(SETUP_DELAY);
	Encoder1_GPIO_Config();
	main_delay(SETUP_DELAY);

	check_SS1();
	old_SS1_position = read_SS1;
	main_delay(SETUP_DELAY);

	init_encoder1();
	main_delay(SETUP_DELAY);

	Encoder2_GPIO_Config();
	main_delay(SETUP_DELAY);

	check_SS2();
	old_SS2_position = read_SS2;
	main_delay(SETUP_DELAY);

	init_encoder2();
	main_delay(SETUP_DELAY);

	Init_PTT_IO();
	main_delay(SETUP_DELAY);

	Init_Mode();
	main_delay(SETUP_DELAY);

	Init_CW_GPIO();
	main_delay(SETUP_DELAY);
}

void displaySplashScreen(void)
{
	LCD_StringLine(200, 100, "STM32 SDR V2.4");
	LCD_StringLine(200, 80, __DATE__);
	LCD_StringLine(200, 60, __TIME__);
	main_delay(5000000);
}

void main_delay(int a)
{
	volatile int i, j;

	for (i = 0; i < a; i++) {
		j++;
	}
}


