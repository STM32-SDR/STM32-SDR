/**
  ******************************************************************************
  * @file    gl_fonts.h
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    11-July-2011
  * @brief   Header for gl_fonts.c
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GL_FONTS_H
#define __GL_FONTS_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/** @addtogroup Embedded_GUI_Library
  * @{
  */
   
/** @addtogroup GL_FONTS
  * @{
  */ 
  
/* Exported types ------------------------------------------------------------*/
/** @defgroup GL_FONTS_Exported_Types
  * @{
  */ 
typedef struct tFont
{    
  const uint16_t *table;
  uint16_t Width;
  uint16_t Height;
}gl_sFONT;

/** 
  * @brief  exported fonts  
  */
extern gl_sFONT GL_Font16x24;
extern gl_sFONT GL_Font12x12;
extern gl_sFONT GL_Font8x12;
extern gl_sFONT GL_Font8x12_bold;
extern gl_sFONT GL_Font8x8;

/**
  * @}
  */
  
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/** @defgroup GL_FONTS_Exported_Macros
  * @{
  */
#define LINE(x) ((x) * (((sFONT *)LCD_GetFont())->Height)) 
/**
  * @}
  */
  
/* Exported functions ------------------------------------------------------- */ 

#ifdef __cplusplus
}
#endif

#endif /* __GL_FONTS_H */

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
