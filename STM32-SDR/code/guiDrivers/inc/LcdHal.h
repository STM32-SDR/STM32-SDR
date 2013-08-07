/**
  ******************************************************************************
  * @file    LcdHal.h
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    11-July-2011
  * @brief   This file contains all the LCD functions whose
  *          implementation depends on the LCD Type used in your Application.
  *          You only need to change these functions implementations
  *          in order to reuse this code with other LCD
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion ---------------------------------------*/
#ifndef __LCD_HAL_H
#define __LCD_HAL_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#if 0
#ifdef USE_STM32100B_EVAL
 #include "stm32HAL.h"
 #include "stm32100b_eval_lcd.h"
#elif defined USE_STM3210B_EVAL
 #include "stm32HAL.h"
 #include "stm3210b_eval_lcd.h" 
#elif defined USE_STM3210E_EVAL
 #include "stm32HAL.h"
 #include "stm3210e_eval_lcd.h"
#elif defined USE_STM3210C_EVAL
 #include "stm32HAL.h"
 #include "stm3210c_eval_lcd.h"
#elif defined USE_STM32100E_EVAL
 #include "stm32HAL.h"
 #include "stm32100e_eval_lcd.h"
#elif defined (USE_STM322xG_EVAL)
 #include "stm32HAL.h"
 #include "stm322xg_eval_lcd.h"
#elif defined (USE_STM32L152_EVAL)
 #include "stm32HAL.h"
 #include "stm32l152_eval_lcd.h"   
#endif
#endif

#include "stm32_eval.h"
#include "LcdDriver_ILI9320.h"

typedef enum {GL_SPI = 0, GL_FSMC = 1, GL_OTHER = 2} GL_BusType;

typedef enum {GL_RESET = 0, GL_SET = !GL_RESET} GL_FlagStatus, GL_ITStatus;

/** 
  * @brief  Signal state enumeration definition  
  */
typedef enum
{ GL_LOW   = Bit_RESET,
  GL_HIGH  = Bit_SET
}GL_SignalActionType;

// Supported fonts for SetFont
typedef enum
{
	GL_FONTOPTION_16x24,
	GL_FONTOPTION_12x12,
	GL_FONTOPTION_8x16,
	GL_FONTOPTION_8x12,
	GL_FONTOPTION_8x12Bold,
	GL_FONTOPTION_8x8,
} GL_FontOption;

typedef struct   
{ 
    GPIO_TypeDef * LCD_Ctrl_Port_NCS;
    GPIO_TypeDef * LCD_Gpio_Data_Port;
    uint16_t       LCD_Ctrl_Pin_NCS;
    uint16_t       LCD_Gpio_Pin_SCK;  
    uint16_t       LCD_Gpio_Pin_MISO; 
    uint16_t       LCD_Gpio_Pin_MOSI;
    uint32_t       LCD_Rcc_BusPeriph_GPIO;
    uint32_t       LCD_Rcc_BusPeriph_GPIO_Ncs;
    uint32_t       LCD_Gpio_RemapPort;
    uint32_t       LCD_Rcc_Bus_Periph;
    SPI_TypeDef *  LCD_Bus_Port;
    GL_BusType     LCD_Connection_Mode;
}LCD_HW_Parameters_TypeDef;

extern __IO uint16_t          GL_TextColor;
extern __IO uint16_t          GL_BackColor;

/*LcdHal_Exported_Constants */
#define GL_OFF                0x00
#define GL_ON                 0x01

/* LCD color */
#define GL_White              LCD_COLOR_WHITE
#define GL_Black              LCD_COLOR_BLACK
#define GL_Grey               LCD_COLOR_GREY
#define GL_Blue               LCD_COLOR_BLUE
#define GL_Blue2              LCD_COLOR_NAVY
#define GL_Red                LCD_COLOR_RED
#define GL_Magenta            LCD_COLOR_MAGENTA
#define GL_Green              LCD_COLOR_GREEN
#define GL_Cyan               LCD_COLOR_CYAN
#define GL_Yellow             LCD_COLOR_YELLOW

#define GL_Horizontal         0x00
#define GL_Vertical           0x01

#define GL_FONT_BIG           GL_FONTOPTION_16x24
#define GL_FONT_SMALL         GL_FONTOPTION_8x16

#define CursorColor           GL_Black



LCD_HW_Parameters_TypeDef* NewLcdHwParamObj (void);
void GL_SetTextColor(__IO uint16_t TextColor);
uint16_t GL_GetTextColor(void);
void GL_SetBackColor(__IO uint16_t BackColor);
uint16_t GL_GetBackColor(void);
void GL_Clear(uint16_t Color);
void GL_SetDisplayWindow(uint16_t Xpos, uint16_t Ypos, uint16_t Height, uint16_t Width);
void GL_DrawLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length, uint8_t Direction);
void GL_LCD_DrawRect(uint8_t Xpos, uint16_t Ypos, uint8_t Height, uint16_t Width);
void GL_LCD_DrawFilledRect(uint8_t Xpos, uint16_t Ypos, uint8_t Height, uint16_t Width);
void GL_LCD_DrawCircle(uint8_t Xpos, uint16_t Ypos, uint16_t Radius);
void GL_BackLightSwitch(uint8_t u8_State);
void GL_BUSConfig(GL_BusType busType);
void GL_LCD_Init(void);
void GL_LCD_WindowModeDisable(void);


void GL_LCD_CtrlLinesWrite(GPIO_TypeDef* GPIOx, uint16_t CtrlPins, GL_SignalActionType BitVal);
uint16_t GL_LCD_ReadRAM(void);


// TODO: Remove
void GL_DisplayAdjStringLine(uint16_t Line, uint16_t Column, uint8_t *ptr, _Bool isTransparent);
void LCD_StringLine(uint16_t PosX, uint16_t PosY, const char *str);

// NEW FUNCTIONS (by Brian)
void GL_SetFont(GL_FontOption uFont);
GL_FontOption GL_GetFont(void);
uint16_t GL_GetFontLetterWidth(GL_FontOption font);
uint16_t GL_GetFontLetterHeight(GL_FontOption font);
void GL_PrintString(uint16_t x, uint16_t y, const char *str, _Bool isTransparent);
void GL_PrintChar(uint16_t x, uint16_t y, char c, _Bool isTransparent);

void GL_TestDisplayScreen(void);


#ifdef __cplusplus
}
#endif

#endif /*__LCD_HAL_H */
