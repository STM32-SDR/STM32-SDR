/**
  ******************************************************************************
  * @file    touchscreen.h
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    11-July-2011
  * @brief   Touchscreen Driver 
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
#ifndef __TOUCHSREEN_H
#define __TOUCHSREEN_H

/* Includes --------------------------------------------------------------------*/
#include "stm32HAL.h"

/** @addtogroup Embedded_GUI_Library
  * @{
  */

/** @addtogroup touchscreen 
  * @{
  */ 

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */ 
void TS_GetCursorPosition(uint16_t *x, uint16_t *y);
void TS_Calibration(void);
void TS_CheckCalibration(void);
uint16_t getDisplayCoordinateX(uint16_t x_touch, uint16_t y_touch);
uint16_t getDisplayCoordinateY(uint16_t x_touch, uint16_t y_touch);
FlagStatus TS_IsCalibrationDone(void);

#ifdef __cplusplus
}
#endif

#endif /* __TOUCHSREEN_H */


/**
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
