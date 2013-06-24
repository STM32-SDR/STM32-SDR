// Includes ------------------------------------------------------------------
#include	"arm_math.h"
#include	"ChangeOver.h"
#include	"CW_Mod.h"
#include	"DMA_IRQ_Handler.h"
#include	"DMA_Test_Pins.h"
#include	"DSP_Processing.h"
#include	"Encoder_1.h"
#include	"Encoder_2.h"
#include	"Init_Codec.h"
#include	"Init_DMA.h"
#include 	"Init_I2C.h"
#include	"Init_I2S.h"
#include	"PSKDet.h"
#include	"ModeSelect.h"
#include	"SI570.h"
#include	"TFT_Display.h"
#include	"touch_pad.h"
#include	"uart.h"
#include	"User_Button.h"
#include 	"usb_conf.h"
#include	"usbh_hid_core.h"
#include	"usbh_usr.h"

const uint32_t CODEC_FREQUENCY = 8000;

uint8_t FFT_Display[256];
uint8_t FFT_Output[128];
int16_t old_SS1_position;
int16_t old_SS2_position;

// USB structures (Must be 4-byte aligned if DMA active)
__ALIGN_BEGIN USB_OTG_CORE_HANDLE USB_OTG_Core_dev __ALIGN_END;
__ALIGN_BEGIN USBH_HOST USB_Host __ALIGN_END;


/*
 * PROTOTYPES
 */
void initializeHardware(void);
void displaySplashScreen(void);
void main_delay(int numLoops);

/*
 * FUNCTIONS
 */



int main(void)
{
	initializeHardware();


	while (1) {

		if (DSP_Flag == 1) {
			for (int16_t j = 0; j < 128; j++) {
				//Changed for getting right display with SR6.3
				FFT_Output[j] = (uint8_t) (6 * log((float32_t) (FFT_Magnitude[j] + 1)));

				if (FFT_Output[j] > 64)
					FFT_Output[j] = 64;
				FFT_Display[2 * j] = FFT_Output[j];
				FFT_Display[2 * j + 1] = (FFT_Output[j] + FFT_Output[j + 1]) / 2;
			}

			DSP_Flag = 0;



			// Redraw the screen
			LCD_DrawFFT(FFT_Display);
			LCD_StringLine(0, 130, (char*) &LCD_buffer[0]);

			//Check_BT_Flag();
			//if (BT_Flag == 1) {
			//	LCD_StringLine(0, 60, "BT  On");

				//uart_putc( Start);
				///for (j=0;j<128;j++) {
				//uart_putc( (unsigned char)FFT_Display [2*j]);
				///uart_putc( (unsigned char)FFT_Display [2*j+1]);
				//for (j=0;j<39;j++) 	uart_putc(LCD_buffer[j]);
				//uart_putc( Stop);
			//}
			//else
			//	LCD_StringLine(0, 60, "BT OFF");

			// Display text
			//LCD_StringLine(0, 110, (char*) &received_string[0]);
		}

		// Process selector switch 1 (has it moved?)
		check_SS1();
		if (read_SS1 != old_SS1_position) {
			process_SS1();
			old_SS1_position = read_SS1;
		}
		if (SI570_Chk != 3)
			process_encoder1();

		// Process selector switch 2 (has it moved?)
		check_SS2();
		if (read_SS2 != old_SS2_position) {
			process_SS2();
			old_SS2_position = read_SS2;
		}
		process_encoder2();

		// Process any pending USB events
		USBH_Process(&USB_OTG_Core_dev, &USB_Host);

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

	Codec_AudioInterface_Init(CODEC_FREQUENCY);
	main_delay(SETUP_DELAY);

	Audio_DMA_Init();
	main_delay(SETUP_DELAY);

	TEST_GPIO_Init();
	main_delay(SETUP_DELAY);

	User_Button_Config();
	main_delay(SETUP_DELAY);

	//BT_Flag_Config();
	//main_delay(SETUP_DELAY);

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

	// Init USB Host Library
	USBH_Init(
			&USB_OTG_Core_dev, // Core handle
		#ifdef USE_USB_OTG_FS
			USB_OTG_FS_CORE_ID,
		#else
			USB_OTG_HS_CORE_ID, // core ID
		#endif
			&USB_Host,         // defined in /main.c (above)
			&HID_cb,           // In /drv/src/usbh_hid.core.c (class callback)
			&USR_Callbacks     // In /usbh_usr.c (user callback)
	);
	main_delay(SETUP_DELAY);
}

void displaySplashScreen(void)
{
	LCD_StringLine(200, 100, "STM32 SDR V2.4");
	LCD_StringLine(200, 80, __DATE__);
	LCD_StringLine(200, 60, __TIME__);
	main_delay(5000000);
}

void main_delay(int numLoops)
{
	volatile int i, j;

	for (i = 0; i < numLoops; i++) {
		j++;
	}
}

