// Options screen
#include "screen_All.h"
#include "widgets.h"
#include "options.h"
#include <assert.h>

// Used in this file to refer to the correct screen (helps to keep code copy-paste friendly.
static GL_Page_TypeDef *s_pThisScreen = &g_screenOptions;

static GL_PageControls_TypeDef* s_lblStatus;


/**
 * Call-back prototypes
 */
static void optionButton_Click(GL_PageControls_TypeDef* pThis);
static void store_Click(GL_PageControls_TypeDef* pThis);
static void done_Click(GL_PageControls_TypeDef* pThis);
static void defaults_Click(GL_PageControls_TypeDef* pThis);
static void calibrate_Click(GL_PageControls_TypeDef* pThis);

#define ID_OPTION_START      100
#define FIRST_BUTTON_Y       40
#define SPACE_PER_BUTTON_Y   30
#define LEFT_COL_X           0
#define COLUMN_WIDTH         110
#define BUTTONS_PER_COLUMN   5

/**
 * Create the screen
 */
void ScreenOptions_Create(void)
{
	Create_PageObj(s_pThisScreen);

	/*
	 * Create the UI widgets
	 */
	GL_PageControls_TypeDef* lblTitle = Widget_NewLabel("Options", LCD_COLOR_WHITE, LCD_COLOR_BLACK, 0, GL_FONTOPTION_16x24, 0);
	GL_PageControls_TypeDef* lblDirections1 = Widget_NewLabel("Tap option to select;", LCD_COLOR_YELLOW, LCD_COLOR_BLACK, 0, GL_FONTOPTION_8x12Bold, 0);
	GL_PageControls_TypeDef* lblDirections2 = Widget_NewLabel("left-knob changes value.", LCD_COLOR_YELLOW, LCD_COLOR_BLACK, 0, GL_FONTOPTION_8x12Bold, 0);
	s_lblStatus = Widget_NewLabel("Changes applied instantly.  ", LCD_COLOR_WHITE, LCD_COLOR_BLACK, 0, GL_FONTOPTION_8x12Bold, 0);


	GL_PageControls_TypeDef* btnOptionsBigButton = Widget_NewBigButtonOptions();
	GL_PageControls_TypeDef* btnStore = NewButton(0, " Store ", store_Click);
	GL_PageControls_TypeDef* btnDefaults = NewButton(0, "Defaults", defaults_Click);
	GL_PageControls_TypeDef* btnDone = NewButton(0, "  Done  ", done_Click);

	GL_PageControls_TypeDef* btnCalibrate = NewButton(13, "Cal. TS", calibrate_Click);

	/*
	 * Place the widgets onto the screen
	 */
	// Title
	AddPageControlObj(  0,   0, lblTitle,        s_pThisScreen);
	AddPageControlObj(120,   0, lblDirections1,  s_pThisScreen);
	AddPageControlObj(120,  10, lblDirections2,  s_pThisScreen);

	// Populate the options buttons:
	for (int i = 0; i < NUM_OPTIONS; i++) {
		GL_PageControls_TypeDef* btnOption = NewButton(ID_OPTION_START + i, Options_GetName(i), optionButton_Click);

		int x = (i / BUTTONS_PER_COLUMN) * COLUMN_WIDTH;
		int y = (i % BUTTONS_PER_COLUMN) * SPACE_PER_BUTTON_Y + FIRST_BUTTON_Y;
		AddPageControlObj(x, y, btnOption, s_pThisScreen);
	}

	// Button Row
	AddPageControlObj(0, LCD_HEIGHT - 42, btnOptionsBigButton, s_pThisScreen);
	AddPageControlObj(90, LCD_HEIGHT - 42, s_lblStatus,  s_pThisScreen);
	AddPageControlObj(90 , LCD_HEIGHT - 30, btnStore, s_pThisScreen);
	AddPageControlObj(165, LCD_HEIGHT - 30, btnDefaults, s_pThisScreen);
	AddPageControlObj(LCD_WIDTH - 75, LCD_HEIGHT - 30, btnDone, s_pThisScreen);
	AddPageControlObj(220,   160, btnCalibrate, s_pThisScreen);

}


static void displayNormalFeedback(void)
{
	Widget_ChangeLabelColour(s_lblStatus, LCD_COLOR_WHITE);
	Widget_ChangeLabelText(s_lblStatus, "Changes applied instantly.  ");
}
static void displayStoreFeedback(void)
{
	Widget_ChangeLabelColour(s_lblStatus, LCD_COLOR_YELLOW);
	Widget_ChangeLabelText(s_lblStatus, "Values stored to EEPROM.    ");
}
static void displayDefaultsFeedback(void)
{
	Widget_ChangeLabelColour(s_lblStatus, LCD_COLOR_YELLOW);
	Widget_ChangeLabelText(s_lblStatus, "All values reset to default.");
}


/**
 * Callbacks
 */
static void optionButton_Click(GL_PageControls_TypeDef* pThis)
{
	uint16_t id = pThis->ID - ID_OPTION_START;
	assert(id >= 0 && id <= NUM_OPTIONS);
	Options_SetSelectedOption(id);

	// Reset message to clear any saved/defaults message.
	displayNormalFeedback();
}

static void store_Click(GL_PageControls_TypeDef* pThis)
{
	displayStoreFeedback();
	Options_WriteToEEPROM();
}

static void defaults_Click(GL_PageControls_TypeDef* pThis)
{
	displayDefaultsFeedback();
	Options_ResetToDefaults();
}

static void done_Click(GL_PageControls_TypeDef* pThis) {
	// Change text back to normal for next time.
	displayNormalFeedback();

	Screen_ShowScreen(&g_screenMain);
}

static void calibrate_Click(GL_PageControls_TypeDef* pThis) {
	Screen_ShowScreen(&g_screenCalibrate);
}


