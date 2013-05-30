/*
 * I2C_Init.h
 *
 *  Created on: Sep 27, 2012
 *      Author: CharleyK
 */
#define CODEC_ADDRESS                   0x18<<1
#define AUDIO_RESET_GPIO_CLK           RCC_AHB1Periph_GPIOB //Redefinition for TFT
#define AUDIO_RESET_PIN                GPIO_Pin_0
#define AUDIO_RESET_GPIO               GPIOB
#define Codec_Pause						1

#define CODEC_RESET_DELAY               0x4FF //Delay for the Codec to be correctly reset
void Codec_GPIO_Init(void);
void Codec_Reset(void);
void Codec_Init(void);

