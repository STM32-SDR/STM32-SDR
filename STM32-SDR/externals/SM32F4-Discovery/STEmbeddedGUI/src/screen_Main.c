// Code for the main (first) screen
#include "screen_All.h"

// Used in this file to refer to the correct screen (helps to keep code copy-paste friendly.
static GL_Page_TypeDef *s_pThisScreen = &g_screenMain;

/**
 * Call-back prototypes
 */
static void calibrate_Click(void);
static void testSwitchButton_Click(void);

uint16_t WidgetFFT_GetWidth(void);
uint16_t WidgetFFT_GetHeight(void);
void WidgetFFT_EventHandler(void);
void WidgetFFT_DrawHandler(void);


/**
 * Create the screen
 */
void ScreenMain_Create(void)
{
	Create_PageObj(s_pThisScreen);

	// Create the UI widgets
	GL_PageControls_TypeDef* lblTitle = NewLabel(1, "Main Screen Example", GL_HORIZONTAL, GL_FONTOPTION_16x24, LCD_COLOR_NAVY);
	GL_PageControls_TypeDef* btnCalibrate = NewButton(2, "Calibrate Touch Screen", calibrate_Click);
	GL_PageControls_TypeDef* lblText1 = NewLabel(3, "This is just the first label", GL_HORIZONTAL, GL_FONTOPTION_8x16, LCD_COLOR_BLACK);
	GL_PageControls_TypeDef* lblText2 = NewLabel(4, "of two lines of text on the LCD.", GL_HORIZONTAL, GL_FONTOPTION_8x16, LCD_COLOR_BLACK);
	GL_PageControls_TypeDef* swtchTest = NewSwitch(5, "State 1", "Other State", testSwitchButton_Click);
	GL_PageControls_TypeDef* fft = NewCustomWidget(6, WidgetFFT_GetWidth, WidgetFFT_GetHeight, WidgetFFT_EventHandler, WidgetFFT_DrawHandler);

	// Place the widgets onto the screen
	AddPageControlObj(0,   100, lblTitle, s_pThisScreen);
	AddPageControlObj(0,   130, lblText1, s_pThisScreen);
	AddPageControlObj(0,   150, lblText2, s_pThisScreen);
	AddPageControlObj(0,   200, swtchTest, s_pThisScreen);
	AddPageControlObj(150, 200, btnCalibrate, s_pThisScreen);
}


/**
 * Button callbacks
 */
static void calibrate_Click(void)
{
	Screen_ShowScreen(&g_screenCalibrate);
}

static void testSwitchButton_Click(void)
{
}




/**
 * FFT Widget (Move to own file?!?)
 */
#include "arm_math.h"
#include "DSP_Processing.h"

const int FFT_WIDTH   = 240;
const int FFT_HEIGHT  =  64;
const int OFFSET_X    =  40;
const int OFFSET_Y    =   0;
const int SELFREQ_ADJ =   4;

uint16_t WidgetFFT_GetWidth(void)
{
	return FFT_WIDTH;
}
uint16_t WidgetFFT_GetHeight(void)
{
	return FFT_HEIGHT;
}

void WidgetFFT_EventHandler(void)
{

}

void WidgetFFT_DrawHandler(void)
{
	uint8_t FFT_Display[256];
	uint8_t FFT_Output[128];


	/*
	 * Calculate the data to draw:
	 */
	// TODO: Where are all these FFT constants from?
	for (int16_t j = 0; j < 128; j++) {
		//Changed for getting right display with SR6.3
		FFT_Output[j] = (uint8_t) (6 * log((float32_t) (FFT_Magnitude[j] + 1)));

		if (FFT_Output[j] > 64)
			FFT_Output[j] = 64;
		FFT_Display[2 * j] = FFT_Output[j];
		// TODO: FFT Display calculation seems to use old data in buffer.
		FFT_Display[2 * j + 1] = (FFT_Output[j] + FFT_Output[j + 1]) / 2;
	}


	/*
	 * Display the FFT:
	 */
	float selectedFreqX = (float) (NCO_Frequency - 125) / 15.625;
	if (selectedFreqX < 0) {
		selectedFreqX = 0;
	}

	// Draw the FFT using direct memory writes (fast).
	LCD_SetDisplayWindow(OFFSET_X, OFFSET_Y, FFT_HEIGHT, FFT_WIDTH);
	LCD_WriteRAM_PrepareDir(LCD_WriteRAMDir_Down);

	for (int x = 0; x < FFT_WIDTH; x++) {
		// Plot this column of the FFT.
		for (int y = 0; y < FFT_HEIGHT; y++) {

			// Draw red line for selected frequency
			if (x == (int) selectedFreqX) {
				// Leave some white at the top
				if (y <= SELFREQ_ADJ) {
					LCD_WriteRAM(LCD_COLOR_WHITE);
				} else {
					LCD_WriteRAM(LCD_COLOR_RED);
				}
			}

			// Draw data
			else if (FFT_HEIGHT - y < FFT_Display[x + 8]) {
				LCD_WriteRAM(LCD_COLOR_BLUE);
			}

			// Draw background
			else {
				LCD_WriteRAM(LCD_COLOR_WHITE);
			}
		}
	}
}
