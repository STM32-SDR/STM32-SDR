/**
  ******************************************************************************
  * @file    images.h
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    11-July-2011
  * @brief   Hex dumps of the 16-color 565 RGB images used by Graphics Library.
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
#ifndef __IMAGES_H
#define __IMAGES_H

#ifdef __cplusplus
 extern "C" {
#endif
   
/* Includes ------------------------------------------------------------------*/
#include "stm32HAL.h"
   
/** @addtogroup Embedded_GUI_Library
  * @{
  */

/** @addtogroup Images
  * @{
  */ 

/* Exported constants --------------------------------------------------------*/
/** @defgroup Images_Exported_Constants
  * @{
  */
#define Red_BG    0xFD4F
#define Green_BG  0xBF22
#define Blue_BG   0x65BF
#define Blue_BG2  0x95FF


extern const uint8_t RButtonA[866];            /*!< radio button bitmap table */
extern const uint8_t RButtonB[866];            /*!< radio button bitmap table */
extern const uint8_t CheckBox1[866];           /*!< CheckBox bitmap table 1 */
extern const uint8_t CheckBox2[866];           /*!< CheckBox bitmap table 2 */
extern const uint8_t ArrowUpTouched[1034];     /*!< ArrowUp Touched bitmap table */
extern const uint8_t ArrowUpUntouched[1034];   /*!< ArrowUp Untouched bitmap table */
extern const uint8_t ArrowDownTouched[1034];   /*!< ArrowDown Touched bitmap table */
extern const uint8_t ArrowDownUntouched[1034]; /*!< ArrowDown Untouched bitmap table */
extern const uint8_t BtnNormalLeft[482];       /*!< Normal Button left part bitmap table */
extern const uint8_t BtnNormalRight[482];      /*!< Normal Button right part bitmap table */
extern const uint8_t BtnNormal[482];           /*!< Normal Button centre part bitmap table */
extern const uint8_t BtnPressedLeft[482];      /*!< Pressed Button left part bitmap table */
extern const uint8_t BtnPressedRight[482];     /*!< Pressed Button right part bitmap table */
extern const uint8_t BtnPressed[482];          /*!< Pressed Button centre part bitmap table */
extern const uint8_t SwitchNormalLeft[482];    /*!< Normal Switch left part bitmap table */
extern const uint8_t SwitchNormalRight[482];   /*!< Normal Switch right part bitmap table */
extern const uint8_t SwitchNormal[482];        /*!< Normal Switch centre part bitmap table */
extern const uint8_t SwitchPressedLeft[482];   /*!< Pressed Switch left part bitmap table */
extern const uint8_t SwitchPressedRight[482];  /*!< Pressed Switch right part bitmap table */
extern const uint8_t SwitchPressed[482];       /*!< Pressed Switch centre part bitmap table */
extern const uint8_t SlidebarLeft[210];        /*!< Slidebar Left part bitmap table */
extern const uint8_t SlidebarRight[210];       /*!< PSlidebar Right centre part bitmap table */
extern const uint8_t SlidebarCentral[210];     /*!< Slidebar Central part bitmap table */
extern const uint8_t SlidebarCursor[306];      /*!< Slidebar Cursor part bitmap table */
  
/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /*__IMAGES_H */

/**
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
