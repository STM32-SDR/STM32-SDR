// Includes ------------------------------------------------------------------
#include	"arm_math.h"
#include	"ChangeOver.h"
#include	"CW_Mod.h"
#include	"DMA_IRQ_Handler.h"
#include	"DMA_Test_Pins.h"
#include	"DSP_Processing.h"
#include	"Encoders.h"
#include	"Init_Codec.h"
#include	"Init_DMA.h"
#include 	"Init_I2C.h"
#include	"Init_I2S.h"
#include	"PSKDet.h"
#include	"ModeSelect.h"
#include	"SI570.h"
#include	"uart.h"
#include	"User_Button.h"
#include 	"usb_conf.h"
#include	"usbh_hid_core.h"
#include	"usbh_usr.h"

#include 	"LcdDriver_ILI9320.h"
#include	"LcdHal.h"
#include	"TSHal.h"

#include	"screen_all.h"
#include	"options.h"
#include	"FrequencyManager.h"
#include	"sdr_image.h"


#define VERSION_STRING "1.003"

const uint32_t CODEC_FREQUENCY = 8000;

// USB structures (Must be 4-byte aligned if DMA active)
__ALIGN_BEGIN USB_OTG_CORE_HANDLE USB_OTG_Core_dev __ALIGN_END;
__ALIGN_BEGIN USBH_HOST USB_Host __ALIGN_END;


/*
 * PROTOTYPES
 */
static void initializeHardware(void);
static void displaySplashScreen(void);
static void main_delay(uint32_t numLoops);

/*
 * FUNCTIONS
 */
int main(void)
{
	initializeHardware();

	/*
	 * Startup the GUI
	 */
	Screen_CreateAllScreens();
	if (!TS_IsCalibrated()) {
		Screen_ShowScreen(&g_screenCalibrate);
	} else {
		Screen_ShowScreen(&g_screenMain);
	}

	while (1) {
		/*
		 * Check if encoder-knobs have changed:
		 */
		Encoders_CalculateAndProcessChanges();

		/*
		 * USB
		 */
		// Process any pending USB events
		USBH_Process(&USB_OTG_Core_dev, &USB_Host);

		/*
		 * Handle general deferred events from the ISR.
		 */
		DelayEvent_ProcessDelayedEvents();

		/*
		 * Touch Events
		 */
		ProcessInputData();

		/*
		 * Redraw the screen (as needed)
		 */
		UpdateScreenWithChanges();
		// DSP_Flag = 0; // moved to widget_FFT_Display

	}
}

static void initializeHardware(void)
{
	const int SETUP_DELAY = 100;

	LCD_Init();
	main_delay(SETUP_DELAY);

	I2C_GPIO_Init();
	main_delay(SETUP_DELAY);

	I2C_Cntrl_Init();
	main_delay(SETUP_DELAY);

	TS_Initialize();
	main_delay(SETUP_DELAY);

	Codec_AudioInterface_Init(CODEC_FREQUENCY);
	main_delay(SETUP_DELAY);

	Audio_DMA_Init();
	main_delay(SETUP_DELAY);

	TEST_GPIO_Init();
	main_delay(SETUP_DELAY);

	User_Button_Config();
	main_delay(SETUP_DELAY);

	ResetModem(BPSK_MODE);
	main_delay(SETUP_DELAY);

	SetRXFrequency(1000);
	main_delay(SETUP_DELAY);

	SetAFCLimit(1000);
	main_delay(SETUP_DELAY);

	uart_init();
	main_delay(SETUP_DELAY);

	displaySplashScreen();
	main_delay(SETUP_DELAY);

	Options_Initialize();
	main_delay(SETUP_DELAY);

	Encoders_Init();
	main_delay(SETUP_DELAY);

	FrequencyManager_Initialize();
	main_delay(SETUP_DELAY);

	Init_PTT_IO();
	main_delay(SETUP_DELAY);

	Mode_Init();
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

	Audio_DMA_Start();			//Get everything up and running before starting DMA Interrupt
	main_delay(SETUP_DELAY);

}

static void displaySplashScreen(void)
{
	const int TEXT_LEFT = 180;
	LCD_DrawBMP16Bit(0,0, gimp_image.height, gimp_image.width, (uint16_t*) gimp_image.pixel_data, 0);

	GL_SetTextColor(LCD_COLOR_BLACK);
	GL_SetBackColor(LCD_COLOR_WHITE);

	GL_PrintString(TEXT_LEFT, 140, "STM32 SDR V" VERSION_STRING, 0);
	GL_PrintString(TEXT_LEFT, 160, __DATE__, 0);
	GL_PrintString(TEXT_LEFT, 180, __TIME__, 0);
	main_delay(10000000);
}

static void main_delay(uint32_t numLoops)
{
	volatile uint32_t i, j;

	for (i = 0; i < numLoops; i++) {
		j++;
	}
}

