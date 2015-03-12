/**
 ******************************************************************************
 * @file    IO_Toggle/stm32f4xx_it.c
 * @author  MCD Application Team
 * @version V1.0.0
 * @date    19-September-2011
 * @brief   Main Interrupt Service Routines.
 *          This file provides template for all exceptions handler and
 *          peripherals interrupt service routine.
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

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"

#include "usb_bsp.h"
#include "usb_hcd_int.h"
#include "usbh_core.h"
#include "stm32f4xx_conf.h"
#include "stm32fxxx_it.h"
#include "ps2Keyboard_Input.h"
#include <assert.h>


/** @addtogroup STM32F4_Discovery_Peripheral_Examples
 * @{
 */

/** @addtogroup IO_Toggle
 * @{
 */
/* Exported variables ------------------------------------------------------------*/
const uint16_t sinetable[]  = {
  127,130,133,136,139,143,146,149,152,155,158,161,164,167,170,173,176,178,181,184,187,190,192,195,198,200,203,205,208,210,212,215,217,219,221,223,225,227,229,231,233,234,236,238,239,240,
  242,243,244,245,247,248,249,249,250,251,252,252,253,253,253,254,254,254,254,254,254,254,253,253,253,252,252,251,250,249,249,248,247,245,244,243,242,240,239,238,236,234,233,231,229,227,225,223,
  221,219,217,215,212,210,208,205,203,200,198,195,192,190,187,184,181,178,176,173,170,167,164,161,158,155,152,149,146,143,139,136,133,130,127,124,121,118,115,111,108,105,102,99,96,93,90,87,84,81,78,
  76,73,70,67,64,62,59,56,54,51,49,46,44,42,39,37,35,33,31,29,27,25,23,21,20,18,16,15,14,12,11,10,9,7,6,5,5,4,3,2,2,1,1,1,0,0,0,0,0,0,0,1,1,1,2,2,3,4,5,5,6,7,9,10,11,12,14,15,16,18,20,21,23,25,27,29,31,
  33,35,37,39,42,44,46,49,51,54,56,59,62,64,67,70,73,76,78,81,84,87,90,93,96,99,102,105,108,111,115,118,121,124
};
uint16_t pulse_width = 128;
uint32_t phase_accumulator = 0;
uint8_t angle = 0;

//Magic Number = 2^32 * f0/fs  where f0 = 500Hz and fs = 100KHz
//#define R 21474836 // 500 Hz
const uint32_t magicR = 23622320; // 550 Hz


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


uint32_t pinsused = 0;

TM_EXTI_Result_t TM_EXTI_Attach(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, TM_EXTI_Trigger_t trigger) {
	GPIO_InitTypeDef GPIO_InitStruct;
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	uint32_t gpio_clock;
	uint8_t pinsource, portsource, irqchannel;

	/* Check if line is already in use */
	if (pinsused & GPIO_Pin) {
		/* Return error */
		return TM_EXTI_Result_Error;
	}

	/* Enable SYSCFG clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	/* Get proper settings */
	if (GPIOx == GPIOE) {
		gpio_clock = RCC_AHB1Periph_GPIOE;
		portsource = EXTI_PortSourceGPIOE;
	} else {
		/* Return error */
		return TM_EXTI_Result_Error;
	}

	switch (GPIO_Pin) {
		case GPIO_Pin_0:
			pinsource = EXTI_PinSource0;
			irqchannel = EXTI0_IRQn;
			break;
		default:
			return TM_EXTI_Result_Error;
	}

	/* Enable clock for GPIO */
	RCC_AHB1PeriphClockCmd(gpio_clock, ENABLE);

	/* Set pin as input */
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;

	if (trigger == TM_EXTI_Trigger_Falling) {
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	} else if (trigger == TM_EXTI_Trigger_Rising) {
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	} else {
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	}

	/* Initialize pin */
	GPIO_Init(GPIOx, &GPIO_InitStruct);

	/* Connect proper GPIO */
	SYSCFG_EXTILineConfig(portsource, pinsource);

	/* Enable EXTI */
	EXTI_InitStruct.EXTI_Line = GPIO_Pin;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = (EXTITrigger_TypeDef)trigger;
	EXTI_Init(&EXTI_InitStruct);

	/* Add to NVIC */
	NVIC_InitStruct.NVIC_IRQChannel = irqchannel;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = TM_EXTI_PRIORITY;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = pinsource;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);

	/* Add to used pins */
	pinsused |= GPIO_Pin;

	/* Return OK */
	return TM_EXTI_Result_Ok;
}

TM_EXTI_Result_t TM_EXTI_Detach(uint16_t GPIO_Pin) {
	EXTI_InitTypeDef EXTI_InitStruct;

	/* Check if pin is used */
	if (!(pinsused & GPIO_Pin)) {
		/* Return Error */
	}

	/* Disable EXTI */
	EXTI_InitStruct.EXTI_Line = GPIO_Pin;
	EXTI_InitStruct.EXTI_LineCmd = DISABLE;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStruct);

	/* Clear from used pins */
	pinsused &= ~GPIO_Pin;

	/* Return OK */
	return TM_EXTI_Result_Ok;
}

#ifndef TM_EXTI_DISABLE_DEFAULT_HANDLER_0
void EXTI0_IRQHandler(void) {
	/* Check status */
	if (EXTI_GetITStatus(EXTI_Line0) != RESET) {
		/* Clear bit */
		EXTI_ClearITPendingBit(EXTI_Line0);
		/* Call user function */
		kbInterrupt();
	}
}
#endif

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
 * @brief   This function handles NMI exception.
 * @param  None
 * @retval None
 */
void NMI_Handler(void)
{
}

/**
 * @brief  This function handles Hard Fault exception.
 * @param  None
 * @retval None
 */
void HardFault_Handler(void)
{
	/* Go to infinite loop when Hard Fault exception occurs */
	//while (1) {
	//}
	//assert(0);
}

/**
 * @brief  This function handles Memory Manage exception.
 * @param  None
 * @retval None
 */
void MemManage_Handler(void)
{
	/* Go to infinite loop when Memory Manage exception occurs */
	while (1) {
	}
}

/**
 * @brief  This function handles Bus Fault exception.
 * @param  None
 * @retval None
 */
void BusFault_Handler(void)
{
	/* Go to infinite loop when Bus Fault exception occurs */
	while (1) {
	}
}

/**
 * @brief  This function handles Usage Fault exception.
 * @param  None
 * @retval None
 */
void UsageFault_Handler(void)
{
	/* Go to infinite loop when Usage Fault exception occurs */
	while (1) {
	}
}

/**
 * @brief  This function handles SVCall exception.
 * @param  None
 * @retval None
 */
void SVC_Handler(void)
{
}

/**
 * @brief  This function handles Debug Monitor exception.
 * @param  None
 * @retval None
 */
void DebugMon_Handler(void)
{
}

/**
 * @brief  This function handles PendSVC exception.
 * @param  None
 * @retval None
 */
void PendSV_Handler(void)
{
}

/**
 * @brief  This function handles SysTick Handler.
 * @param  None
 * @retval None
 */
void SysTick_Handler(void)
{
}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/

/**
 * @brief  This function handles PPP interrupt request.
 * @param  None
 * @retval None
 */
/*void PPP_IRQHandler(void)
 {
 }*/


// ------------------------------------------------------------------------------
// USB Changes:

extern USB_OTG_CORE_HANDLE          USB_OTG_Core_dev;   // Defined in main.h (likely)
extern USBH_HOST                    USB_Host;           // Defined in main.h (likely)
extern void USB_OTG_BSP_TimerIRQ (void);

/**
  * @brief  EXTI1_IRQHandler
  *         This function handles External line 1 interrupt request.
  * @param  None
  * @retval None
  */

void EXTI1_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line1) != RESET)
  {
      USB_Host.usr_cb->OverCurrentDetected();
      EXTI_ClearITPendingBit(EXTI_Line1);
  }
}

/**
  * @brief  TIM2_IRQHandler
  *         This function handles Timer2 Handler.
  * @param  None
  * @retval None
  */
void TIM2_IRQHandler(void)
{
  USB_OTG_BSP_TimerIRQ();
}

/**
  * @brief  OTG_FS_IRQHandler
  *          This function handles USB-On-The-Go FS global interrupt request.
  *          requests.
  * @param  None
  * @retval None
  */
#ifdef USE_USB_OTG_FS
void OTG_FS_IRQHandler(void)
#else
void OTG_HS_IRQHandler(void)
#endif
{
  USBH_OTG_ISR_Handler(&USB_OTG_Core_dev);
}

// ------------------------------------------------------------------------------
#include "CW_Mod.h"
/**
  * @brief  TIM3_IRQHandler
  *         This function handles Timer3 Handler.
  * @param  None
  * @retval None
  */
void TIM3_IRQHandler(void)
{
	CW_KeyPollTimerIRQ();
}

#include "Oscillator.h"
void TIM5_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)
  {

    PWM_SetDC(pulse_width);
    // Calculate a new pulse width
    phase_accumulator+=magicR;
    angle=(uint8_t)(phase_accumulator>>24);
    pulse_width = sinetable[angle];
    TIM_ClearITPendingBit(TIM5, TIM_IT_Update);

  }
}

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
