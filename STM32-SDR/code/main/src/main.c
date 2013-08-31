// Includes ------------------------------------------------------------------

#include	"ChangeOver.h"
#include	"CW_Mod.h"
#include	"Encoders.h"
#include	"Init_Codec.h"
#include	"Init_DMA.h"
#include 	"Init_I2C.h"
#include	"Init_I2S.h"
#include	"PSKDet.h"
#include	"ModeSelect.h"
#include	"SI570.h"
#include	"uart.h"
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
#include	"ScrollingTextBox.h"
#include	"Text_Enter.h"

#define VERSION_STRING "1.009"

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
static void displayLoadStationData(void);

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
		Screen_ShowMainScreen();
	}
	InitTextDisplay();

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

		 //* Touch Events
		 //*/
		ProcessInputData();

		/*
		 * Redraw the screen (as needed)
		 */
		UpdateScreenWithChanges();

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

	FrequencyManager_Initialize();
	main_delay(SETUP_DELAY);

	Encoders_Init();
	main_delay(SETUP_DELAY);

	//Load stored macro data
	displayLoadStationData();
	Text_Initialize();
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
static void displayLoadStationData(void)
	{
	GL_SetTextColor(LCD_COLOR_RED);
	GL_SetBackColor(LCD_COLOR_WHITE);
	GL_PrintString(0, 200, "Loading Station Info", 0);

	}


static void main_delay(uint32_t numLoops)
{
	volatile uint32_t i, j;

	for (i = 0; i < numLoops; i++) {
		j++;
	}
}

