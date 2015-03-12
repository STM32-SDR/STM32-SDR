/**
 ******************************************************************************
 * @file    GPIO/IOToggle/stm32f4xx_it.h
 * @author  MCD Application Team
 * @version V1.0.0
 * @date    19-September-2011
 * @brief   This file contains the headers of the interrupt handlers.
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
#ifndef __STM32F4xx_IT_H
#define __STM32F4xx_IT_H

#ifdef __cplusplus
extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/
/* Exported variables ------------------------------------------------------------*/
const extern uint16_t sinetable[];
extern uint16_t pulse_width;
extern uint32_t phase_accumulator;
extern uint8_t angle;

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* __STM32F4xx_IT_H */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

/* Default priority for NVIC */
#define TM_EXTI_PRIORITY	0x0A

/**
 * Interrupt trigger enumeration
 *
 * Parameters:
 * 	- TM_EXTI_Trigger_Rising:
 * 		Trigger interrupt on rising edge on line
 * 	- TM_EXTI_Trigger_Falling:
 * 		Trigger interrupt on falling edge on line
 * 	- TM_EXTI_Trigger_Rising_Falling:
 * 		Trigger interrupt on any edge on line
 */
typedef enum {
	TM_EXTI_Trigger_Rising = 0x08,
	TM_EXTI_Trigger_Falling = 0x0C,
	TM_EXTI_Trigger_Rising_Falling = 0x10
} TM_EXTI_Trigger_t;

/**
 * Result enumeration
 *
 * Parameters:
 * 	- TM_EXTI_Result_Ok:
 * 		Everything ok
 * 	- TM_EXTI_Result_Error:
 * 		An error has occured
 */
typedef enum {
	TM_EXTI_Result_Ok = 0,
	TM_EXTI_Result_Error
} TM_EXTI_Result_t;

TM_EXTI_Result_t TM_EXTI_Attach(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, TM_EXTI_Trigger_t trigger);
