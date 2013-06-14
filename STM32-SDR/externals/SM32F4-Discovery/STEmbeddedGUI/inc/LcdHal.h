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

#include "stm32_eval.h"
#include "graphicObjectTypes.h"

/** @addtogroup Embedded_GUI_Library
  * @{
  */

/** @addtogroup LcdHal 
  * @{
  */ 

/** @defgroup LcdHal_Exported_Types
  * @{
  */
   
/** 
  * @brief  LCD_Direction_TypeDef enumeration definition  
  */
typedef enum
{ _0_degree = 0,
  _90_degree,
  _180_degree,
  _270_degree
}LCD_Direction_TypeDef;

/** 
  * @brief  GL_BusType enumeration definition  
  */
typedef enum {GL_SPI = 0, GL_FSMC = 1, GL_OTHER = 2} GL_BusType;

/** 
  * @brief  GL_FlagStatus, GL_ITStatus enumeration definition  
  */
typedef enum {GL_RESET = 0, GL_SET = !GL_RESET} GL_FlagStatus, GL_ITStatus;

/** 
  * @brief  Signal state enumeration definition  
  */
typedef enum
{ GL_LOW   = Bit_RESET,
  GL_HIGH  = Bit_SET
}GL_SignalActionType;

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

/**
  * @}
  */

/** @defgroup LcdHal_Imported_Variables
  * @{
  */
extern __IO uint16_t          GL_TextColor;
extern __IO uint16_t          GL_BackColor;
/**
  * @}
  */

/** @defgroup LcdHal_Exported_Constants
  * @{
  */
#define GL_OFF                0x00
#define GL_ON                 0x01

/* LCD color */
#define GL_White              White
#define GL_Black              Black
#define GL_Grey               Grey
#define GL_Blue               Blue
#define GL_Blue2              Blue2
#define GL_Red                Red
#define GL_Magenta            Magenta
#define GL_Green              Green
#define GL_Cyan               Cyan
#define GL_Yellow             Yellow

#define GL_Horizontal         0x00
#define GL_Vertical           0x01

#define GL_FONT_BIG           0x00
#define GL_FONT_SMALL         0x01
#define GL_FONT_BIG_WIDTH       16
#define GL_FONT_BIG_HEIGHT      24
#define GL_FONT_SMALL_WIDTH      8
#define GL_FONT_SMALL_HEIGHT    12

#define FirstPixel            0x01
#define MiddlePixel           0x02
#define LastPixel             0x04
#define SinglePixel           0x08

#define CursorColor           GL_Black
//#define GL_TextColor          TextColor
//#define GL_BackColor          BackColor
/**
  * @}
  */

/** @defgroup LcdHal_Exported_Variables
  * @{
  */
extern __IO uint8_t           GL_Font;
extern __IO uint8_t           GL_FontWidth;
extern __IO uint8_t           GL_FontHeight;
extern uint16_t               LCD_Height;
extern uint16_t               LCD_Width;
/**
  * @}
  */

/** @defgroup LcdHal_Exported_Macros
  * @{
  */

/**
  * @}
  */

/** @defgroup LcdHal_Exported_Functions
  * @{
  */
LCD_HW_Parameters_TypeDef* NewLcdHwParamObj (void);
void GL_SetTextColor(__IO uint16_t TextColor);
void GL_SetBackColor(__IO uint16_t BackColor);
void GL_Clear(uint16_t Color);
void GL_LCD_DrawCharTransparent(uint16_t Xpos, uint16_t Ypos, const uint16_t *c); /* 16bit char */
void GL_LCD_DisplayChar(uint16_t Line, uint16_t Column, uint8_t Ascii, GL_bool Trasparent_Flag);
void GL_DisplayAdjStringLine(uint16_t Line, uint16_t Column, uint8_t *ptr, GL_bool Transparent_Flag);
void GL_SetDisplayWindow(uint16_t Xpos, uint16_t Ypos, uint16_t Height, uint16_t Width);
void GL_DrawLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length, uint8_t Direction);
void GL_LCD_DrawRect(uint8_t Xpos, uint16_t Ypos, uint8_t Height, uint16_t Width);
void GL_LCD_DrawCircle(uint8_t Xpos, uint16_t Ypos, uint16_t Radius);
void GL_DrawBMP(uint8_t* ptrBitmap);
void GL_SetFont(uint8_t uFont);
void GL_BackLightSwitch(uint8_t u8_State);
void GL_BUSConfig(GL_BusType busType);
void GL_LCD_SPIConfig(void);
void GL_LCD_FSMCConfig(void);
void GL_LCD_Init(void);
void GL_LCD_WindowModeDisable(void);
void LCD_PutPixel(uint16_t Xpos, uint16_t Ypos, uint16_t Color, uint8_t PixelSpec);
void LCD_FillArea(uint16_t Xpos_Init, uint16_t Ypos_Init, uint16_t Height, uint16_t Width, uint16_t color);
uint16_t LCD_GetPixel(uint16_t Xpos, uint16_t Ypos);
void GL_LCD_DrawChar(uint8_t Xpos, uint16_t Ypos, const uint16_t *c);
void GL_LCD_CtrlLinesWrite(GPIO_TypeDef* GPIOx, uint16_t CtrlPins, GL_SignalActionType BitVal);
uint16_t GL_LCD_ReadRAM(void);
void LCD_WriteRAMWord(uint16_t RGB_Code);

void LCD_Change_Direction(LCD_Direction_TypeDef Direction);
void LCD_WriteChar(uint16_t Xpos, uint16_t Ypos, const uint16_t *c);
void LCD_PrintChar(uint16_t Line, uint16_t Column, uint8_t Ascii);
void LCD_PrintStringLine(uint16_t Line, uint16_t Column, uint8_t *ptr);
void LCD_DrawMonoBMP(const uint8_t *Pict, uint16_t Xpos_Init, uint16_t Ypos_Init, uint16_t Height, uint16_t Width);
void LCD_DrawColorBMP(uint8_t* ptrBitmap, uint16_t Xpos_Init, uint16_t Ypos_Init, uint16_t Height, uint16_t Width);

#ifdef __cplusplus
}
#endif

#endif /*__LCD_HAL_H */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
