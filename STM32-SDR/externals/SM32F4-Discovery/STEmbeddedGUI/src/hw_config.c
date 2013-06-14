/**
  ******************************************************************************
  * @file    hw_config.c
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    11-July-2011
  * @brief   This file provides targets hardware configuration.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  */

/* Includes ------------------------------------------------------------------*/
#include "hw_config.h"

/** @addtogroup Embedded_GUI_Example
  * @{
  */

/** @defgroup HWConfig
  * @brief Target Hardware initialization and configuration
  * @{
  */

/** @defgroup HWConfig_Private_TypesDefinitions
  * @{
  */

/**
  * @}
  */

/** @defgroup HWConfig_Private_Variables
  * @{
  */

/**
  * @}
  */

/** @defgroup HWConfig_Private_Functions
  * @{
  */

/**
  * @brief  Set Joystick parameters
  * @param  None
  * @retval None
  */
static void HWConfig_JoyParamInit(void)
{
  JOY_HW_Parameters_TypeDef* pJoyParam = NewJoyHwParamObj ();

#if defined(USE_STM3210C_EVAL) || defined(USE_STM322xG_EVAL)
  /* Assign the following values for Joystick Parameters Structure */
  pJoyParam->JOY_PinSource               = JOY_GPIO_PIN_SOURCE;
  pJoyParam->JOY_PortSource              = JOY_GPIO_PORT_SOURCE;
  pJoyParam->JOY_Exti_IrqChannel         = JOY_EXTI_IRQ_CHANNEL;
  pJoyParam->JOY_Exti_Line               = JOY_EXTI_LINE;
  pJoyParam->JOY_DeviceRegister          = JOY_I2C_DEVICE_REGISTER;
  pJoyParam->JOY_IT_Gpio_Port            = JOY_IT_GPIO_PORT;
  pJoyParam->JOY_IT_Gpio_Pin             = JOY_IT_GPIO_PIN;
#else
  pJoyParam->JOY_Select_Port             = JOY_GPIO_SELECT_PORT;
  pJoyParam->JOY_Right_Port              = JOY_GPIO_RIGHT_PORT;
  pJoyParam->JOY_Left_Port               = JOY_GPIO_LEFT_PORT;
  pJoyParam->JOY_Up_Port                 = JOY_GPIO_UP_PORT;
  pJoyParam->JOY_Down_Port               = JOY_GPIO_DOWN_PORT;
  pJoyParam->JOY_Rcc_BusPeriph_GpioSel   = JOY_GPIO_RCC_BUS_PERIPH1;
  pJoyParam->JOY_Rcc_BusPeriph_GpioRight = JOY_GPIO_RCC_BUS_PERIPH2;
  pJoyParam->JOY_Rcc_BusPeriph_GpioLeft  = JOY_GPIO_RCC_BUS_PERIPH3;
  pJoyParam->JOY_Rcc_BusPeriph_GpioUp    = JOY_GPIO_RCC_BUS_PERIPH4;
  pJoyParam->JOY_Rcc_BusPeriph_GpioDown  = JOY_GPIO_RCC_BUS_PERIPH5;
#endif

  pJoyParam->JOY_Select_Pin              = JOY_GPIO_SELECT_PIN;
  pJoyParam->JOY_Right_Pin               = JOY_GPIO_RIGHT_PIN;
  pJoyParam->JOY_Left_Pin                = JOY_GPIO_LEFT_PIN;
  pJoyParam->JOY_Up_Pin                  = JOY_GPIO_UP_PIN;
  pJoyParam->JOY_Down_Pin                = JOY_GPIO_DOWN_PIN;


}


/**
  * @brief  Set Touchscreen Controller parameters
  * @param  None
  * @retval None
  */
static void HWConfig_TSCParamInit(void)
{
#if defined(USE_STM3210C_EVAL) || defined(USE_STM32100E_EVAL) || defined(USE_STM322xG_EVAL)
  TSC_HW_Parameters_TypeDef* pTscParam = NewTscHwParamObj ();

  /* Assign the following values for Touchscreen Controller Parameters Structure */
  pTscParam->TSC_I2C_Clk                = TSC_I2C_CLK;
  pTscParam->TSC_I2C_Sda_Gpio_Pin       = TSC_I2C_SDA_GPIO_PIN;
  pTscParam->TSC_I2C_Sda_Gpio_Port      = TSC_I2C_SDA_GPIO_PORT;
  pTscParam->TSC_I2C_Sda_Gpio_Clk       = TSC_I2C_SDA_GPIO_CLK;
  pTscParam->TSC_I2C_Sda_PinSource      = TSC_I2C_SDA_SOURCE;
  pTscParam->TSC_I2C_Sda_AltFunc        = (uint32_t)TSC_I2C_SDA_AF;
  pTscParam->TSC_I2C_Scl_Gpio_Pin       = TSC_I2C_SCL_GPIO_PIN;
  pTscParam->TSC_I2C_Scl_Gpio_Port      = TSC_I2C_SCL_GPIO_PORT;
  pTscParam->TSC_I2C_Scl_Gpio_Clk       = TSC_I2C_SCL_GPIO_CLK;
  pTscParam->TSC_I2C_Scl_PinSource      = TSC_I2C_SCL_SOURCE;
  pTscParam->TSC_I2C_Scl_AltFunc        = (uint32_t)TSC_I2C_SCL_AF;
  pTscParam->TSC_I2C_Clk                = TSC_I2C_CLK;
  pTscParam->TSC_IT_Exti_Pin_Source     = TSC_IT_EXTI_PIN_SOURCE;
  pTscParam->TSC_IT_Gpio_Clk            = TSC_IT_GPIO_CLK;
  pTscParam->TSC_PortSource             = TSC_GPIO_PORT_SOURCE;
  pTscParam->TSC_PinSource              = TSC_GPIO_PIN_SOURCE;
  pTscParam->TSC_Exti_IrqChannel        = TSC_EXTI_IRQ_CHANNEL;
  pTscParam->TSC_Exti_Line              = TSC_EXTI_LINE;
  pTscParam->TSC_IT_Gpio_Port           = TSC_IT_GPIO_PORT;
  pTscParam->TSC_IT_Gpio_Pin            = TSC_IT_GPIO_PIN;
  pTscParam->TSC_DeviceRegister         = TSC_I2C_DEVICE_REGISTER;
  pTscParam->TSC_Bus_Port               = TSC_I2C_PORT;
#endif

}

/**
  * @brief  Set LCD Controller Parameters
  * @param  None
  * @retval None
  */
static void HWConfig_LCDParamInit(void)
{
  LCD_HW_Parameters_TypeDef* pLcdParam = NewLcdHwParamObj ();

  /* Assign the following values for LCD Controller Parameters Structure */
  pLcdParam->LCD_Connection_Mode        = LCD_CONNECTION_MODE;
  pLcdParam->LCD_Rcc_Bus_Periph         = LCD_RCC_BUS_PERIPH;

  /* Configuration for SPI interfaced LCDs */
#if defined(USE_STM3210C_EVAL) || defined(USE_STM3210B_EVAL) ||\
  defined(USE_STM32100B_EVAL) ||   defined(USE_STM32L152_EVAL)
  pLcdParam->LCD_Ctrl_Port_NCS          = LCD_CTRL_PORT_NCS;
  pLcdParam->LCD_Gpio_Data_Port         = LCD_GPIO_DATA_PORT;
  pLcdParam->LCD_Ctrl_Pin_NCS           = LCD_CTRL_PIN_NCS;
  pLcdParam->LCD_Gpio_Pin_SCK           = LCD_GPIO_PIN_SCK;
  pLcdParam->LCD_Gpio_Pin_MISO          = LCD_GPIO_PIN_MISO;
  pLcdParam->LCD_Gpio_Pin_MOSI          = LCD_GPIO_PIN_MOSI;
  pLcdParam->LCD_Gpio_RemapPort         = LCD_GPIO_REMAP_PORT;
  pLcdParam->LCD_Rcc_BusPeriph_GPIO     = LCD_GPIO_RCC_BUS_PERIPH;
  pLcdParam->LCD_Rcc_BusPeriph_GPIO_Ncs = LCD_GPIO_RCC_BUS_PERIPH_NCS;
  pLcdParam->LCD_Bus_Port               = LCD_SPI_PORT;
#endif

}

/**
  * @brief  Set evaluation board Parameters parameters
  * @param  None
  * @retval None
  */
void HWConfig_SetHardwareParams(void)
{
  BTN_HW_Parameters_TypeDef* pBtnParam = NewBtnHwParamObj ();

  HWConfig_JoyParamInit();
  HWConfig_TSCParamInit();
  HWConfig_LCDParamInit();

  /* Assign the following values for Button Parameters Structure */
  pBtnParam->BTN_Rcc_BusPeriph_Gpio      = USER_BUTTON_BUS_PERIPH;
  pBtnParam->BTN_Port                    = USER_BUTTON_PORT;
  pBtnParam->BTN_Pin                     = USER_BUTTON_PIN;
  pBtnParam->BTN_Pin_Active              = USER_BUTTON_PIN_ACTIVE;
}

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
