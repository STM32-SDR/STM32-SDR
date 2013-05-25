/*
 * Init_I2S.h
 *
 *  Created on: Sep 30, 2012
 *      Author: CharleyK
 */
#define I2S_Standard_Phillips           ((uint16_t)0x0000)
#define I2S_Standard_MSB                ((uint16_t)0x0010)
#define CODEC_MCLK_ENABLED

/* I2S peripheral configuration defines */
#define CODEC_I2S                      SPI3
#define CODEC_I2S_CLK                  RCC_APB1Periph_SPI3
#define CODEC_I2S_ADDRESS              0x40003C0C
#define CODEC_I2S_GPIO_AF              GPIO_AF_SPI3
#define CODEC_I2S_IRQ                  SPI3_IRQn
#define CODEC_I2S_GPIO_CLOCK           (RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOA)
//#define CODEC_I2S_WS_PIN               GPIO_Pin_4
#define CODEC_I2S_WS_PIN               GPIO_Pin_15  //chh changed to free up PA4 for SPI1 interface
#define CODEC_I2S_SCK_PIN              GPIO_Pin_10
#define CODEC_I2S_SD_PIN               GPIO_Pin_12
#define CODEC_I2Sext_SD_PIN            GPIO_Pin_11
#define CODEC_I2S_MCK_PIN              GPIO_Pin_7
//#define CODEC_I2S_WS_PINSRC            GPIO_PinSource4
#define CODEC_I2S_WS_PINSRC            GPIO_PinSource15 //chh changed to free up PA4 for SPI1 interface
#define CODEC_I2S_SCK_PINSRC           GPIO_PinSource10
#define CODEC_I2S_SD_PINSRC            GPIO_PinSource12
#define CODEC_I2Sext_SD_PINSRC         GPIO_PinSource11
#define CODEC_I2S_MCK_PINSRC           GPIO_PinSource7
#define CODEC_I2S_GPIO                 GPIOC
#define CODEC_I2S_WS_GPIO              GPIOA
#define CODEC_I2S_MCK_GPIO             GPIOC
#define Audio_I2S_IRQHandler           SPI3_IRQHandler


  void I2S_GPIO_Init(void);

  void Codec_AudioInterface_Init(uint32_t AudioFreq);


