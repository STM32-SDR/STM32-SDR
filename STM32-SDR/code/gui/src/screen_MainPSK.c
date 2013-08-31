// Code for the PSK main screen
#include "screen_All.h"
#include "widgets.h"
#include "ModeSelect.h"
#include "options.h"
#include "Text_Enter.h"
#include "Keyboard_Input.h"
#include <assert.h>

// Used in this file to refer to the correct screen (helps to keep code copy-paste friendly.
static GL_Page_TypeDef *s_pThisScreen = &g_screenMainPSK;

/**
 * Call-back prototypes
 */
static void tx_Click(GL_PageControls_TypeDef* pThis);
static void rx_Click(GL_PageControls_TypeDef* pThis);
static void N_Click(GL_PageControls_TypeDef* pThis);
static void C_Click(GL_PageControls_TypeDef* pThis);
static void T_Click(GL_PageControls_TypeDef* pThis);
/**
 * Create the screen
 */
void ScreenMainPSK_Create(void)
{
	Create_PageObj(s_pThisScreen);

	// PSK
	GL_PageControls_TypeDef* ctrlPskText = Widget_NewPSKTextDisplay();
	//AddPageControlObj(0,  80, ctrlPskText, s_pThisScreen);
	AddPageControlObj(0,  60, ctrlPskText, s_pThisScreen);

	// FFT
	Widget_AddToPage_NewFFTDisplay(80, 0, s_pThisScreen);


	// "Standard" complement of main screen buttons:
	// .. Mode
	GL_PageControls_TypeDef* btnMode = Widget_NewBigButtonMode();
	AddPageControlObj(0, 0, btnMode, s_pThisScreen);
	// .. Options
	GL_PageControls_TypeDef* btnOptions = Widget_NewBigButtonOptions();
	AddPageControlObj(0, LCD_HEIGHT - 42, btnOptions, s_pThisScreen);
	// .. Frequency
	GL_PageControls_TypeDef* btnFreq = Widget_NewBigButtonFrequency();
	AddPageControlObj(
			LCD_WIDTH - ((GL_Custom_TypeDef*)(btnFreq->objPTR))->GetWidth(btnFreq),
			LCD_HEIGHT - ((GL_Custom_TypeDef*)(btnFreq->objPTR))->GetHeight(btnFreq),
			btnFreq, s_pThisScreen);
	// .. Rx & Tx buttons (Remove when code can automatically switch)
	GL_PageControls_TypeDef* btnRx  = NewButton(10, " Rx ", rx_Click);
	GL_PageControls_TypeDef* btnTx  = NewButton(9,  " Tx ", tx_Click);
	GL_PageControls_TypeDef* btnN  = NewButton(13,  "N ", N_Click);
	GL_PageControls_TypeDef* btnC  = NewButton(14,  "C ", C_Click);
	GL_PageControls_TypeDef* btnT  = NewButton(15,  "T ", T_Click);

	AddPageControlObj(120, LCD_HEIGHT - 42, btnRx, s_pThisScreen);
	AddPageControlObj(165, LCD_HEIGHT - 42, btnTx, s_pThisScreen);
	AddPageControlObj(150,   165, btnN, s_pThisScreen);
	AddPageControlObj(0, 165, btnC, s_pThisScreen);
	AddPageControlObj(280, 165, btnT, s_pThisScreen);

}


/**
 * Button callbacks
 */
#include "ChangeOver.h"
static void tx_Click(GL_PageControls_TypeDef* pThis)
{
	assert(Mode_GetCurrentMode() == MODE_PSK);
	Xmit_PSK_Sequence();
}
static void rx_Click(GL_PageControls_TypeDef* pThis)
{
	Receive_Sequence();
}

static void N_Click(GL_PageControls_TypeDef* pThis)
{
	set_kybd_mode(2);
	Contact_Clear(1);
	text_cnt = 0;
}

static void C_Click(GL_PageControls_TypeDef* pThis)
{
	set_kybd_mode(1);
	Contact_Clear(0);
	text_cnt = 0;
}

static void T_Click(GL_PageControls_TypeDef* pThis)
{
	Screen_ShowScreen(&g_screenEditText);

}
