/**
  ******************************************************************************
  * @file    TscHal.c
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    11-July-2011
  * @brief   This file contains all the TouchScreen functions whose
  *          implementation depends on the TSC Controller used in your Design.
  *          You only need to change these functions implementations
  *          in order to reuse this code with other TSC Controller 
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

/* Includes ---------------------------------------------------------------------------*/
#include "TscHal.h"
#include "touchscreen.h"

#ifdef USE_STM3210C_EVAL
   #include "stm3210c_eval_ioe.h"
#elif USE_STM32100E_EVAL
   #include "stm32100e_eval_ioe.h"
#elif USE_STM322xG_EVAL
   #include "stm322xg_eval_ioe.h"
#endif

/** @addtogroup Embedded_GUI_Library
  * @{
  */

/** @defgroup TscHal 
  * @brief TscHal main functions
  * @{
  */ 

/* External variables --------------------------------------------------------*/
extern __IO uint32_t u32_TSXCoordinate;
extern __IO uint32_t u32_TSYCoordinate;

extern uint32_t TSC_Value_X;
extern uint32_t TSC_Value_Y;

extern uint32_t EndAddr;
extern uint32_t CalibrationAddr;
extern __IO uint8_t calibration_done;
extern __IO uint8_t touch_done;

/* Touchscreen Hardware Parameters Structure */
TSC_HW_Parameters_TypeDef pTscHwParam;

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
#define    _1_K                     ((uint8_t)1024)

#ifdef STM32F2XX
  #define TOUCH_DELAY     			0x10
#else
  #define TOUCH_DELAY     			0x8
#endif
     
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#ifdef STM32F2XX
/**
  * @brief  FLASH page&size definition for STM32F2xx devices
  */
const uint32_t FLASH_Sectors[2][12] = 
    {
      {
        FLASH_Sector_0,
        FLASH_Sector_1,
        FLASH_Sector_2,
        FLASH_Sector_3,
        FLASH_Sector_4,
        FLASH_Sector_5,
        FLASH_Sector_6,
        FLASH_Sector_7,
        FLASH_Sector_8,
        FLASH_Sector_9,
        FLASH_Sector_10,
        FLASH_Sector_11,
      },
      {
        16*_1_K,
        16*_1_K,
        16*_1_K,
        16*_1_K,
        64*_1_K,  
        128*_1_K,    
        128*_1_K,    
        128*_1_K,    
        128*_1_K,      
        128*_1_K,    
        128*_1_K,    
        128*_1_K,     
      }
    };
#endif

/* Private function prototypes -----------------------------------------------*/
extern void GL_Delay(uint32_t nTime);

/* static function generate warnings if they are not used */
#if defined(USE_STM3210C_EVAL) || defined(USE_STM32100E_EVAL)
static void TSC_GPIO_Configuration(void);
#endif
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Create and initialize a new Touchscreen Hw Parameter structure object
  * @param  None
  * @retval TSC_HW_Parameters_TypeDef*, The created Object pointer
  */

TSC_HW_Parameters_TypeDef* NewTscHwParamObj (void)
{
  return &pTscHwParam;
}

/**
  * @brief  Initializes the GPIOx peripheral according to the specified
  *         parameters in the GPIO_InitStruct.
  * @param  GPIOx: where x can be (A..G) to select the GPIO peripheral.
  * @param  GPIO_InitStruct: pointer to a GPIO_InitTypeDef structure that
  *         contains the configuration information for the specified GPIO
  *         peripheral.
  * @retval None
  */
void GL_GPIO_Init(GPIO_TypeDef * GPIOx, GPIO_InitTypeDef* GPIO_InitStruct)
{
  GPIO_InitTypeDef tmp;
  tmp.GPIO_Pin   = GPIO_InitStruct->GPIO_Pin;
  tmp.GPIO_Speed = GPIO_InitStruct->GPIO_Speed;
  tmp.GPIO_Mode  = GPIO_InitStruct->GPIO_Mode;
#ifdef STM32F2XX  
  tmp.GPIO_OType = GPIO_InitStruct->GPIO_OType;  
  tmp.GPIO_PuPd  = GPIO_InitStruct->GPIO_PuPd; 
#endif
  GPIO_Init( (GPIO_TypeDef*)GPIOx, &tmp);
}

/**
  * @brief  Initializes the SPIx peripheral according to the specified
  *         parameters in the SPI_InitStruct.
  * @param  SPIx: where x can be 1, 2 or 3 to select the SPI peripheral.
  * @param  SPI_InitStruct: pointer to a SPI_InitTypeDef structure that
  *         contains the configuration information for the specified
  *         SPI peripheral.
  * @retval None
  */
void GL_SPI_Init(SPI_TypeDef* SPIx, SPI_InitTypeDef* SPI_InitStruct)
{
  SPI_InitTypeDef tmp;
  tmp.SPI_Direction         = SPI_InitStruct->SPI_Direction;
  tmp.SPI_Mode              = SPI_InitStruct->SPI_Mode;
  tmp.SPI_DataSize          = SPI_InitStruct->SPI_DataSize;
  tmp.SPI_CPOL              = SPI_InitStruct->SPI_CPOL;
  tmp.SPI_CPHA              = SPI_InitStruct->SPI_CPHA;
  tmp.SPI_NSS               = SPI_InitStruct->SPI_NSS;
  tmp.SPI_BaudRatePrescaler = SPI_InitStruct->SPI_BaudRatePrescaler;
  tmp.SPI_FirstBit          = SPI_InitStruct->SPI_FirstBit;
  tmp.SPI_CRCPolynomial     = SPI_InitStruct->SPI_CRCPolynomial;
  SPI_Init(SPIx, &tmp);
}

/**
  * @brief  Sets the vector table location and Offset.
  * @param  NVIC_VectTab: specifies if the vector table is in RAM or
  *         FLASH memory. This parameter can be one of the following values:
  *     @arg NVIC_VectTab_RAM
  *     @arg NVIC_VectTab_FLASH
  * @param  Offset: Vector Table base offset field.
  *         This value must be a multiple of 0x100.
  * @retval None
  */
void GL_NVIC_SetVectorTable(uint32_t NVIC_VectTab, uint32_t Offset)
{
  NVIC_SetVectorTable(NVIC_VectTab, Offset);
}

/**
  * @brief  Initializes the NVIC peripheral according to the specified
  *         parameters in the NVIC_InitStruct.
  * @param  NVIC_InitStruct: pointer to a NVIC_InitTypeDef structure
  *           that contains the configuration information for the
  *           specified NVIC peripheral.
  * @retval None
  */
void GL_NVIC_Init(NVIC_InitTypeDef* NVIC_InitStruct)
{
  NVIC_InitTypeDef tmp;
  tmp.NVIC_IRQChannel                   = NVIC_InitStruct->NVIC_IRQChannel;
  tmp.NVIC_IRQChannelPreemptionPriority = NVIC_InitStruct->NVIC_IRQChannelPreemptionPriority;
  tmp.NVIC_IRQChannelSubPriority        = NVIC_InitStruct->NVIC_IRQChannelSubPriority;
  tmp.NVIC_IRQChannelCmd                = (FunctionalState)NVIC_InitStruct->NVIC_IRQChannelCmd;
  NVIC_Init(&tmp);
}

/**
  * @brief  Configures the priority grouping: pre-emption priority
  *         and subpriority.
  * @param  NVIC_PriorityGroup: specifies the priority grouping bits
  *         length. This parameter can be one of the following values:
  *     @arg - NVIC_PriorityGroup_0: 0 bits for pre-emption priority
  *            4 bits for subpriority
  *     @arg - NVIC_PriorityGroup_1: 1 bits for pre-emption priority
  *            3 bits for subpriority
  *     @arg - NVIC_PriorityGroup_2: 2 bits for pre-emption priority
  *            2 bits for subpriority
  *     @arg - NVIC_PriorityGroup_3: 3 bits for pre-emption priority
  *            1 bits for subpriority
  *     @arg - NVIC_PriorityGroup_4: 4 bits for pre-emption priority
  *            0 bits for subpriority
  * @retval None
  */
void GL_NVIC_PriorityGroupConfig(uint32_t NVIC_PriorityGroup)
{
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup);
}

/**
  * @brief  Configures the specified System Handlers priority.
  * @param  SystemHandler: specifies the system handler to be
  *         enabled or disabled.
  *         This parameter can be one of the following values:
  *     @arg - SystemHandler_MemoryManage
  *     @arg - SystemHandler_BusFault
  *     @arg - SystemHandler_UsageFault
  *     @arg - SystemHandler_SVCall
  *     @arg - SystemHandler_DebugMonitor
  *     @arg - SystemHandler_PSV
  *     @arg - SystemHandler_SysTick
  * @param  SystemHandlerPreemptionPriority: new priority group of the
  *         specified system handlers.
  * @param  SystemHandlerSubPriority: new sub priority of the specified
  *         system handlers.
  * @retval None
  */
void GL_NVIC_SystemHandlerPriorityConfig(uint32_t SystemHandler, uint8_t SystemHandlerPreemptionPriority, uint8_t SystemHandlerSubPriority)
{
  /*   NVIC_SystemHandlerPriorityConfig(SystemHandler, SystemHandlerPreemptionPriority, SystemHandlerSubPriority); */
}

/**
  * @brief  Initializes the EXTI peripheral according to the specified
  *         parameters in the EXTI_InitStruct.
  * @param  EXTI_InitStruct: pointer to a EXTI_InitTypeDef structure
  *         that contains the configuration information for the EXTI
  *         peripheral.
  * @retval None
  */
void GL_EXTI_Init(EXTI_InitTypeDef* EXTI_InitStruct)
{
  EXTI_InitTypeDef tmp;
  tmp.EXTI_Line    = EXTI_InitStruct->EXTI_Line;
  tmp.EXTI_Mode    = EXTI_InitStruct->EXTI_Mode;
  tmp.EXTI_Trigger = EXTI_InitStruct->EXTI_Trigger;
  tmp.EXTI_LineCmd = (FunctionalState)EXTI_InitStruct->EXTI_LineCmd;
  EXTI_Init(&tmp);
}

/**
  * @brief  Selects the GPIO pin used as EXTI Line.
  * @param  GPIO_PortSource: selects the GPIO port to be used as source for
  *         EXTI lines.
  *         This parameter can be GPIO_PortSourceGPIOx where x can be (A..G).
  * @param  GPIO_PinSource: specifies the EXTI line to be configured.
  *         This parameter can be GPIO_PinSourcex where x can be (0..15).
  * @retval None
  */
void GL_GPIO_EXTILineConfig(uint8_t GPIO_PortSource, uint8_t GPIO_PinSource)
{
#if defined(STM32F2XX) || defined(STM32L1XX_MD)
  SYSCFG_EXTILineConfig(GPIO_PortSource, GPIO_PinSource);
#else  
  GPIO_EXTILineConfig(GPIO_PortSource, GPIO_PinSource);
#endif
}

/**
  * @brief  Deinitializes the EXTI peripheral registers to their default
  *         reset values.
  * @param  None
  * @retval None
  */
void GL_EXTI_DeInit(void)
{
  EXTI_DeInit();
}

/**
  * @brief  Deinitializes the NVIC peripheral registers to their default
  *         reset values.
  * @param  None
  * @retval None
  */
void GL_NVIC_DeInit(void)
{
  /*  NVIC_DeInit(); */
}


/**
  * @brief  This function handles External lines interrupt request for Touchscreen.
  * @param  None
  * @retval None
  */
void GL_EXTI_TSC_IRQHandler( void )
{
#if TOUCH_SCREEN_CAPABILITY
  if ( EXTI_GetITStatus(pTscHwParam.TSC_Exti_Line) != RESET )
  {
   if (IOE_GetGITStatus(pTscHwParam.TSC_DeviceRegister, IOE_TS_IT) & IOE_TS_IT)
   {
      TSC_Read();
     /* Clear the interrupt pending bits */    
     IOE_ClearGITPending(pTscHwParam.TSC_DeviceRegister, IOE_TS_IT); 
   }
    EXTI_ClearITPendingBit(pTscHwParam.TSC_Exti_Line);
  }
  
#endif
}

#if TOUCH_SCREEN_CAPABILITY
/**
  * @brief  Configure GPIO
  * @param  None
  * @retval None
  */
static void TSC_GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
#ifndef STM32F2XX
  /* Enable TSC_I2C_Channel and IOExpander I2C_PORT & AFEN(Alternate Function) clocks */
  GL_RCC_APBPeriphClockCmd(pTscHwParam.TSC_I2C_Clk, GL_ENABLE, 1);
  GL_RCC_APBPeriphClockCmd(pTscHwParam.TSC_I2C_Scl_Gpio_Clk | pTscHwParam.TSC_I2C_Sda_Gpio_Clk |
                           pTscHwParam.TSC_IT_Gpio_Clk | RCC_APB2Periph_AFIO, GL_ENABLE, 2);
  
  /* Reset TSC_I2C_Channel IP (affected by other peripherals-E2p,Audio,MEMS on TSC_I2C_Channel?) */
  RCC_APB1PeriphResetCmd(pTscHwParam.TSC_I2C_Clk, ENABLE);
  /* Release reset signal of TSC_I2C_Channel IP */
  RCC_APB1PeriphResetCmd(pTscHwParam.TSC_I2C_Clk, DISABLE);
   
  /* TSC_I2C_Channel SCL and SDA pins configuration */
  GPIO_InitStructure.GPIO_Pin = pTscHwParam.TSC_I2C_Scl_Gpio_Pin;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
  GL_GPIO_Init(pTscHwParam.TSC_I2C_Sda_Gpio_Port, &GPIO_InitStructure);
  
  /* TSC_I2C_Channel SCL and SDA pins configuration */
  GPIO_InitStructure.GPIO_Pin = pTscHwParam.TSC_I2C_Sda_Gpio_Pin;
  GPIO_Init(pTscHwParam.TSC_I2C_Sda_Gpio_Port, &GPIO_InitStructure);
  
  /* Set EXTI pin as Input PullUp - IO_Expander_INT */
  GPIO_InitStructure.GPIO_Pin = pTscHwParam.TSC_IT_Gpio_Pin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
  GL_GPIO_Init(pTscHwParam.TSC_IT_Gpio_Port, &GPIO_InitStructure);

#else

  /* Enable IOE_I2C and IOE_I2C_GPIO_PORT & Alternate Function clocks */
  RCC_APB1PeriphClockCmd(pTscHwParam.TSC_I2C_Clk, ENABLE);
  RCC_AHB1PeriphClockCmd(pTscHwParam.TSC_I2C_Scl_Gpio_Clk | pTscHwParam.TSC_I2C_Sda_Gpio_Clk |
                           pTscHwParam.TSC_IT_Gpio_Clk, ENABLE);
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  
  /* Reset IOE_I2C IP */
  RCC_APB1PeriphResetCmd(pTscHwParam.TSC_I2C_Clk, ENABLE);
  
  /* Release reset signal of IOE_I2C IP */
  RCC_APB1PeriphResetCmd(pTscHwParam.TSC_I2C_Clk, DISABLE);
  
  /* IOE_I2C SCL and SDA pins configuration */
  GPIO_InitStructure.GPIO_Pin = pTscHwParam.TSC_I2C_Scl_Gpio_Pin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GL_GPIO_Init(pTscHwParam.TSC_I2C_Scl_Gpio_Port, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = pTscHwParam.TSC_I2C_Sda_Gpio_Pin;
  GL_GPIO_Init(pTscHwParam.TSC_I2C_Sda_Gpio_Port, &GPIO_InitStructure);
  
  GPIO_PinAFConfig(pTscHwParam.TSC_I2C_Scl_Gpio_Port,  pTscHwParam.TSC_I2C_Scl_PinSource, pTscHwParam.TSC_I2C_Scl_AltFunc);
  GPIO_PinAFConfig(pTscHwParam.TSC_I2C_Sda_Gpio_Port,  pTscHwParam.TSC_I2C_Sda_PinSource, pTscHwParam.TSC_I2C_Sda_AltFunc);  
  
  /* Set EXTI pin as Input PullUp - IO_Expander_INT */
  GPIO_InitStructure.GPIO_Pin = pTscHwParam.TSC_IT_Gpio_Pin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GL_GPIO_Init(pTscHwParam.TSC_IT_Gpio_Port, &GPIO_InitStructure);  
  
#endif  
  /* Connect IO Expander IT line to EXTI line */
  GL_GPIO_EXTILineConfig(pTscHwParam.TSC_PortSource, pTscHwParam.TSC_PinSource);
}
#endif

/**
  * @brief  Init the TS interface
  * @param  None
  * @retval None
  */
void TSC_Init(void)
{
#if TOUCH_SCREEN_CAPABILITY
  /* Reset the STMPE811 using the serial communication interface */
    IOE_Config();
#endif
}

/**
  * @brief  Initializes the IO Expander registers.
  * @param  None
  * @retval - 0: if all initializations are OK.
  */
uint32_t GL_TSC_Interface_Init(void)
{
#if TOUCH_SCREEN_CAPABILITY

  /* Configure the needed pins */
  TSC_GPIO_Configuration();

  /* Read IO Expander 1 ID  */
  if(IOE_IsOperational(pTscHwParam.TSC_DeviceRegister))
  {
    return 1;
  }

  /* Generate IOExpander Software reset */
  IOE_Reset(pTscHwParam.TSC_DeviceRegister);
  
  /* Disable all the Functionnalities */
  IOE_FnctCmd(pTscHwParam.TSC_DeviceRegister, IOE_IO_FCT | IOE_TEMPSENS_FCT | IOE_TS_FCT | IOE_ADC_FCT, DISABLE);
#endif

  return 0; /* Configuration is OK */
}

/**
  * @brief  Configure NVIC for the Touchscreen
  * @param  None
  * @retval None
  */
void TSC_NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  /* Enable and configure the priority of the IRQ Channel */
  NVIC_InitStructure.NVIC_IRQChannel = pTscHwParam.TSC_Exti_IrqChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

  GL_NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  Configure the EXTI for the Touchscreen
  * @param  None
  * @retval None
  */
void TSC_EXTI_Configuration(void)
{
  EXTI_InitTypeDef EXTI_InitStructure;

  GL_EXTI_DeInit();

  EXTI_InitStructure.EXTI_Line = pTscHwParam.TSC_Exti_Line;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  GL_EXTI_Init(&EXTI_InitStructure);
  GL_GPIO_EXTILineConfig(pTscHwParam.TSC_PortSource, pTscHwParam.TSC_PinSource);
}

/**
  * @brief  Configures the used IRQ Channels and sets their priority.
  * @param  None
  * @retval None
  */
void TSC_InterruptConfig(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Deinitializes the NVIC */
  GL_NVIC_DeInit();

  /* Set the Vector Table base address at 0x08000000 */
  GL_NVIC_SetVectorTable(GL_NVIC_VectTab_FLASH, 0x00);

  /* Configure the Priority Group to 2 bits */
  GL_NVIC_PriorityGroupConfig(GL_NVIC_PriorityGroup_2);

  /* Enable and configure RCC global IRQ channel */
  NVIC_InitStructure.NVIC_IRQChannel = GL_RCC_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  GL_NVIC_Init(&NVIC_InitStructure);

  /* Configure one bit for preemption priority */
  GL_NVIC_PriorityGroupConfig(GL_NVIC_PriorityGroup_1);
  /* Enable the EXTI15_10 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = pTscHwParam.TSC_Exti_IrqChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  GL_NVIC_Init(&NVIC_InitStructure);

  GL_EXTI_DeInit();
}

/**
  * @brief  Enables or disables the High Speed APB (APB1 or APB2) peripheral clock.
  * @param  RCC_APBPeriph: specifies the APB peripheral to gates its clock.
  * @param  NewState: new state of the specified peripheral clock.
  *         This parameter can be one of the following values:
  *     @arg  ENABLE
  *     @arg  DISABLE
  * @retval None
  */
void GL_RCC_APBPeriphClockCmd(uint32_t RCC_APBPeriph, GL_FunctionalState NewState, uint8_t APB_Selector)
{
#ifndef STM32F2XX
  if (APB_Selector == 1)
    RCC_APB1PeriphClockCmd(RCC_APBPeriph, (FunctionalState)NewState);
  else
    RCC_APB2PeriphClockCmd(RCC_APBPeriph, (FunctionalState)NewState);
#endif  
}

/**
  * @brief  Enables or disables the AHB peripheral clock.
  * @param  RCC_APBPeriph: specifies the APB peripheral to gates its clock.
  *         For @b STM32_Connectivity_line_devices, this parameter can be any combination
  *         of the following values:
  *     @arg  RCC_AHBPeriph_DMA1
  *     @arg  RCC_AHBPeriph_DMA2
  *     @arg  RCC_AHBPeriph_SRAM
  *     @arg  RCC_AHBPeriph_FLITF
  *     @arg  RCC_AHBPeriph_CRC
  *     @arg  RCC_AHBPeriph_OTG_FS
  *     @arg  RCC_AHBPeriph_ETH_MAC
  *     @arg  RCC_AHBPeriph_ETH_MAC_Tx
  *     @arg  RCC_AHBPeriph_ETH_MAC_Rx
  *         For @b other_STM32_devices, this parameter can be any combination
  *         of the following values:
  *     @arg  RCC_AHBPeriph_DMA1
  *     @arg  RCC_AHBPeriph_DMA2
  *     @arg  RCC_AHBPeriph_SRAM
  *     @arg  RCC_AHBPeriph_FLITF
  *     @arg  RCC_AHBPeriph_CRC
  *     @arg  RCC_AHBPeriph_FSMC
  *     @arg  RCC_AHBPeriph_SDIO
  * @param  NewState: new state of the specified peripheral clock.
  *         This parameter can be one of the following values:
  *     @arg  ENABLE
  *     @arg  DISABLE
  * @retval None
  */
void GL_RCC_AHBPeriphClockCmd(uint32_t RCC_AHBPeriph, GL_FunctionalState NewState)
{
#ifndef STM32F2XX
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph, (FunctionalState)NewState);
#endif
}

/**
  * @brief  Write Data in data buffer to Flash.
  * @param  FlashFree_Address: Page address
  * @param  Data: pointer to data buffer
  * @param  Size: data buffer size in bytes
  * @retval FLASH programming status
  */
TSC_FLASH_Status TSC_WriteDataToNVM(uint32_t FlashFree_Address, int32_t *Data, uint32_t Size)
{
  TSC_FLASH_Status TSC_FlashStatus = TSC_FLASH_COMPLETE;
  
#if TOUCH_SCREEN_CAPABILITY
  
  uint32_t words = (Size/sizeof(uint32_t)) + ((Size%sizeof(uint32_t))?1:0);
  uint32_t index = 0;
  

  /* Unlock the Flash Program Erase controller */
  TSC_FLASH_Unlock();

  /* Erase Flash sectors ******************************************************/

  /* Clear All pending flags */
  TSC_FLASH_ClearFlag( TSC_FLASH_FLAG_BSY | TSC_FLASH_FLAG_EOP | TSC_FLASH_FLAG_PGERR | TSC_FLASH_FLAG_WRPRTERR);

  /* Erase Last Flash Page */
  TSC_FlashStatus = TSC_FLASH_ErasePage( FlashFree_Address );

  for(index = 0; index < words; index++)
  {
     /* Writing calibration parameters to the Flash Memory */
    TSC_FlashStatus = TSC_FLASH_ProgramWord( FlashFree_Address, Data[index]);
    /* Increasing Flash Memory Page Address */
    FlashFree_Address = FlashFree_Address + 4;
  }
#endif
  return TSC_FlashStatus;
}

/**
  * @brief  Unlocks the FLASH Program Erase Controller.
  * @param  None
  * @retval None
  */
void TSC_FLASH_Unlock(void)
{
  FLASH_Unlock();
}

/**
  * @brief  Clears the FLASH’s pending flags.
  * @param  LASH_FLAG: specifies the FLASH flags to clear.
  *         This parameter can be any combination of the following values:
  *     @arg  TSC_FLASH_FLAG_PGERR: FLASH Program error flag
  *     @arg  TSC_FLASH_FLAG_WRPRTERR: FLASH Write protected error flag
  *     @arg  TSC_FLASH_FLAG_EOP: FLASH End of Operation flag
  * @retval None
  */
void TSC_FLASH_ClearFlag(uint32_t FLASH_FLAG)
{
  FLASH_ClearFlag(FLASH_FLAG);
}

/**
  * @brief  Erases a specified FLASH page.
  * @param  Page_Address: The page address to be erased.
  * @retval None
  */
TSC_FLASH_Status TSC_FLASH_ErasePage(uint32_t Page_Address)
{
  TSC_FLASH_Status TSC_FlashStatus = TSC_FLASH_COMPLETE;
#ifndef STM32F2XX  
   TSC_FlashStatus = FLASH_ErasePage(Page_Address);
#else
   uint8_t index = 0;
   uint32_t size = 0;
   
   while(size < Page_Address && index < 12)
   {
     size += FLASH_Sectors[1][index];
     index++;
   }
   
   if(size != Page_Address && index != 0)
   {
     index--;
   }
   TSC_FlashStatus = FLASH_EraseSector(FLASH_Sectors[0][index], VoltageRange_3);

#endif   
   return TSC_FlashStatus;
}

/**
  * @brief  Programs a word at a specified address.
  * @param  Address: specifies the address to be programmed.
  * @param  Data: specifies the data to be programmed
  * @retval None
  */
TSC_FLASH_Status TSC_FLASH_ProgramWord(uint32_t Address, uint32_t Data)
{
  return FLASH_ProgramWord(Address, Data);
}

/**
  * @brief  Set the last Flash Memory address
  * @param  address: Pointer to the penultimate memory page
  * @retval None
  */
void Set_LastFlashMemoryAddress( uint32_t address)
{
  EndAddr = address;

  /* Calculate the address of the Penultimate Flash Memory Page, where the calibration parameters will be saved. */
  CalibrationAddr = (uint32_t)(EndAddr - 0x800);
}

/**
  * @brief  Read the coordinate of the point touched and assign their
  *         value to the variables u32_TSXCoordinate and u32_TSYCoordinate
  * @param  None
  * @retval None
  */
void TSC_Read(void)
{
#if TOUCH_SCREEN_CAPABILITY
  TSC_Value_X = 0x00;
  TSC_Value_Y = 0x00;
  if((I2C_ReadDeviceRegister(pTscHwParam.TSC_DeviceRegister, GL_TSC_CTRL) & 0x80))
  {
      GL_Delay(TOUCH_DELAY);

      /* Stop touchscreen controller */
      I2C_WriteDeviceRegister(pTscHwParam.TSC_DeviceRegister, GL_TSC_CTRL, 0x00);
      
      TSC_Value_X = I2C_ReadDataBuffer(pTscHwParam.TSC_DeviceRegister, GL_TSC_DATA_Y);    
      TSC_Value_Y = I2C_ReadDataBuffer(pTscHwParam.TSC_DeviceRegister, GL_TSC_DATA_X);
      I2C_ReadDataBuffer(pTscHwParam.TSC_DeviceRegister, GL_TSC_DATA_Z);
      
      u32_TSXCoordinate = getDisplayCoordinateX( TSC_Value_X, TSC_Value_Y );
      u32_TSYCoordinate = getDisplayCoordinateY( TSC_Value_X, TSC_Value_Y );
      
      touch_done = 1;
      
      /* Clear the interrupt pending bit and enable the FIFO again */
      I2C_WriteDeviceRegister(pTscHwParam.TSC_DeviceRegister, GL_FIFO_CTRL_STA, 0x01);
      I2C_WriteDeviceRegister(pTscHwParam.TSC_DeviceRegister, GL_INT_STA, IOE_TS_IT);
      I2C_WriteDeviceRegister(pTscHwParam.TSC_DeviceRegister, GL_FIFO_CTRL_STA, 0x00);      
      GL_Delay(0x02);
      
      /* Enable touchscreen controller */
      I2C_WriteDeviceRegister(pTscHwParam.TSC_DeviceRegister, GL_TSC_CTRL, 0x01);
      
      GL_Delay(0x05);

      /* check if the FIFO is not empty */
      if(I2C_ReadDeviceRegister(pTscHwParam.TSC_DeviceRegister, GL_FIFO_CTRL_STA) != 0x20)
      {
        /* Flush the FIFO */
        I2C_WriteDeviceRegister(pTscHwParam.TSC_DeviceRegister, GL_FIFO_CTRL_STA, 0x01);
        I2C_WriteDeviceRegister(pTscHwParam.TSC_DeviceRegister, GL_FIFO_CTRL_STA, 0x00);
      }
  }
  else
  {
    GL_Delay(1);
  }
#endif
}


/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
