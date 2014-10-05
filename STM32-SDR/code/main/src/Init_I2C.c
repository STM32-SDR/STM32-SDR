/*
 * Code to initialise the I2C functions
 *
 * STM32-SDR: A software defined HAM radio embedded system.
 * Copyright (C) 2013, STM32-SDR Group
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include	"stm32f4xx.h"
#include	"stm32f4xx_gpio.h"
#include	"stm32f4xx_rcc.h"
#include 	"stm32f4xx_i2c.h"
#include 	"Init_I2C.h"
#include	"xprintf.h"

void I2C_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	/* Enable I2C GPIO clocks */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	/* I2C1 SCL and SDA pins configuration -------------------------------------*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	/* Connect pins to I2C peripheral */
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1 );
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_I2C1 );
}

/**
 * @brief  Initializes the Audio Codec control interface (I2C).
 * @param  None
 * @retval None
 */
void I2C_Cntrl_Init(void)
{
	I2C_InitTypeDef I2C_InitStructure;

	/* Enable the CODEC_I2C peripheral clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

	/* CODEC_I2C peripheral configuration */
	I2C_DeInit(I2C1 );
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = 0x33;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = I2C_SPEED;
	/* Enable the I2C peripheral */
	I2C_Cmd(I2C1, ENABLE);
	I2C_Init(I2C1, &I2C_InitStructure);
}

/**
 * @brief  Writes a Byte to a given register into the audio codec through the
 control interface (I2C)
 * @param  RegisterAddr: The address (location) of the register to be written.
 * @param  RegisterValue: the Byte value to be written into destination register.
 * @retval 0 if correct communication, else wrong communication
 */
uint32_t I2C_WriteRegister(uint8_t DeviceAddr, uint8_t RegisterAddr,
		uint8_t RegisterValue)
{
	uint32_t result = 0;

	/*!< While the bus is busy */
	__IO uint32_t Timeout = I2C_LONG_TIMEOUT;
	while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY )) {
		if ((Timeout--) == 0)
			return 1; //TIMEOUT_UserCallback();
	}

	/* Start the config sequence */
	I2C_GenerateSTART(I2C1, ENABLE);

	/* Test on EV5 and clear it */
	Timeout = I2C_SHORT_TIMEOUT;
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT )) {
		if ((Timeout--) == 0)
			return 2; //TIMEOUT_UserCallback();
	}

	/* Transmit the slave address and enable writing operation */
	I2C_Send7bitAddress(I2C1, DeviceAddr, I2C_Direction_Transmitter );

	/* Test on EV6 and clear it */
	Timeout = I2C_SHORT_TIMEOUT;
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED )) {
		if ((Timeout--) == 0)
			return 3; // Codec_TIMEOUT_UserCallback();
	}

	/* Transmit the first address for write operation */
	I2C_SendData(I2C1, RegisterAddr);

	/* Test on EV8 and clear it */
	Timeout = I2C_SHORT_TIMEOUT;
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING )) {
		if ((Timeout--) == 0)
			return 4; // Codec_TIMEOUT_UserCallback();
	}

	/* Prepare the register value to be sent */
	I2C_SendData(I2C1, RegisterValue);

	/*!< Wait till all data have been physically transferred on the bus */
	Timeout = I2C_LONG_TIMEOUT;
	while (!I2C_GetFlagStatus(I2C1, I2C_FLAG_BTF )) {
		if ((Timeout--) == 0)
			return 5; // Codec_TIMEOUT_UserCallback();
	}

	/* End the configuration sequence */
	I2C_GenerateSTOP(I2C1, ENABLE);

	/* Return the verifying value: 0 (Passed) or 1 (Failed) */
	return result;
}

uint32_t I2C_WriteRegister_N(uint8_t DeviceAddr, uint8_t RegisterAddr,
		uint8_t write[], uint8_t N)
{
	uint32_t result = 0;
	uint8_t i;

	/*!< While the bus is busy */
	__IO uint32_t Timeout = I2C_LONG_TIMEOUT;
	while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY )) {
		if ((Timeout--) == 0)
			return 1; //TIMEOUT_UserCallback();
	}

	/* Start the config sequence */
	I2C_GenerateSTART(I2C1, ENABLE);

	/* Test on EV5 and clear it */
	Timeout = I2C_SHORT_TIMEOUT;
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT )) {
		if ((Timeout--) == 0)
			return 2; //TIMEOUT_UserCallback();
	}

	/* Transmit the slave address and enable writing operation */
	I2C_Send7bitAddress(I2C1, DeviceAddr, I2C_Direction_Transmitter );

	/* Test on EV6 and clear it */
	Timeout = I2C_SHORT_TIMEOUT;
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED )) {
		if ((Timeout--) == 0)
			return 3; // Codec_TIMEOUT_UserCallback();
	}

	/* Transmit the first address for write operation */
	I2C_SendData(I2C1, RegisterAddr);

	/* Test on EV8 and clear it */
	Timeout = I2C_SHORT_TIMEOUT;
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING )) {
		if ((Timeout--) == 0)
			return 4; // Codec_TIMEOUT_UserCallback();
	}

	for (i = 0; i < N; i++) {

		/* Prepare the register value to be sent */
		I2C_SendData(I2C1, write[i]);

		/*!< Wait till all data have been physically transferred on the bus */
		Timeout = I2C_LONG_TIMEOUT;
		while (!I2C_GetFlagStatus(I2C1, I2C_FLAG_BTF )) {
			if ((Timeout--) == 0)
				return 5; // Codec_TIMEOUT_UserCallback();
		}

	} // end of for statement

	/* End the configuration sequence */
	I2C_GenerateSTOP(I2C1, ENABLE);

	/* Return the verifying value: 0 (Passed) or 1 (Failed) */
	return result;
}

/**
 * @brief  Reads and returns the value of a register through the
 *         control interface (I2C).
 * @param  RegisterAddr: Address of the register to be read.
 * @retval Value of the register to be read or dummy value if the communication
 *         fails.
 */
uint32_t I2C_ReadSlave(uint8_t Address, uint8_t Register)
{
	uint32_t result = 0;

	/*!< While the bus is busy */
	__IO uint32_t Timeout = I2C_LONG_TIMEOUT;
	while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY )) {
		if ((Timeout--) == 0)
			return 1; //Codec_TIMEOUT_UserCallback();
	}

	/* Start the config sequence */
	I2C_GenerateSTART(I2C1, ENABLE);

	/* Test on EV5 and clear it */
	Timeout = I2C_SHORT_TIMEOUT;
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT )) {
		if ((Timeout--) == 0)
			return 2; //Codec_TIMEOUT_UserCallback();
	}

	/* Transmit the slave address and enable writing operation */
	I2C_Send7bitAddress(I2C1, Address, I2C_Direction_Transmitter );

	/* Test on EV6 and clear it */
	Timeout = I2C_SHORT_TIMEOUT;
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED )) {
		if ((Timeout--) == 0)
			return 3; //Codec_TIMEOUT_UserCallback();
	}

	/* Transmit the register address to be read */
	I2C_SendData(I2C1, Register);

	/* Test on EV8 and clear it */
	Timeout = I2C_SHORT_TIMEOUT;
	while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BTF ) == RESET) {
		if ((Timeout--) == 0)
			return 4; // Codec_TIMEOUT_UserCallback();
	}

	/*!< Send START condition a second time */
	I2C_GenerateSTART(I2C1, ENABLE);

	/*!< Test on EV5 and clear it (cleared by reading SR1 then writing to DR) */
	Timeout = I2C_SHORT_TIMEOUT;
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT )) {
		if ((Timeout--) == 0)
			return 5; //Codec_TIMEOUT_UserCallback();
	}

	/*!< Send Codec address for read */
	I2C_Send7bitAddress(I2C1, Address, I2C_Direction_Receiver );

	/* Wait on ADDR flag to be set (ADDR is still not cleared at this level */
	Timeout = I2C_SHORT_TIMEOUT;
	while (I2C_GetFlagStatus(I2C1, I2C_FLAG_ADDR ) == RESET) {
		if ((Timeout--) == 0)
			return 6; //Codec_TIMEOUT_UserCallback();
	}

	/*!< Disable Acknowledgment */
	I2C_AcknowledgeConfig(I2C1, DISABLE);

	/* Clear ADDR register by reading SR1 then SR2 register (SR1 has already been read) */
	(void) I2C1 ->SR2;

	/*!< Send STOP Condition */
	I2C_GenerateSTOP(I2C1, ENABLE);

	/* Wait for the byte to be received */
	Timeout = I2C_SHORT_TIMEOUT;
	while (I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE ) == RESET) {
		if ((Timeout--) == 0)
			return 7;  //Codec_TIMEOUT_UserCallback();
	}

	/*!< Read the byte received from the Codec */
	result = I2C_ReceiveData(I2C1 );

	/* Wait to make sure that STOP flag has been cleared */
	Timeout = I2C_SHORT_TIMEOUT;
	while (I2C1 ->CR1 & I2C_CR1_STOP ) {
		if ((Timeout--) == 0)
			return 8; //Codec_TIMEOUT_UserCallback();
	}

	/*!< Re-Enable Acknowledgment to be ready for another reception */
	I2C_AcknowledgeConfig(I2C1, ENABLE);

	/* Clear AF flag for next communication */
	I2C_ClearFlag(I2C1, I2C_FLAG_AF );

	/* Return the byte read from I2C Device */
	return result;
}

//Store a single byte in eeprom. eeprom address is 14 bits long so address is an integer
uint32_t I2C_WriteEEProm(uint16_t RegisterAddr, uint8_t RegisterValue)
{
	uint32_t result = 0;

	// check to see if writing is necessary
	uint8_t temp = (uint8_t) I2C_ReadEEProm(RegisterAddr);
	if (temp != RegisterValue){

		debug(GUI, "I2C_WriteEEProm\n");


	/*!< While the bus is busy */
	__IO uint32_t Timeout = I2C_SHORT_TIMEOUT;
	while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY )) {
		if ((Timeout--) == 0)
			return 1; //TIMEOUT_UserCallback();
	}

	/* Start the config sequence */
	I2C_GenerateSTART(I2C1, ENABLE);

	/* Test on EV5 and clear it */
	Timeout = I2C_SHORT_TIMEOUT;
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT )) {
		if ((Timeout--) == 0)
			return 2; //TIMEOUT_UserCallback();
	}

	/* Transmit the slave address and enable writing operation */
	I2C_Send7bitAddress(I2C1, EEPROM_Write_Address, I2C_Direction_Transmitter ); //EEPROM_Write_Address	= 0b10100000

	/* Test on EV6 and clear it */
	Timeout = I2C_SHORT_TIMEOUT;
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED )) {
		if ((Timeout--) == 0)
			return 3; // Codec_TIMEOUT_UserCallback();
	}

	/* Transmit the address hi byte for write operation */
	I2C_SendData(I2C1, (RegisterAddr >> 8) & 0x3F);

	/* Test on EV8 and clear it */
	Timeout = I2C_SHORT_TIMEOUT;
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING )) {
		if ((Timeout--) == 0)
			return 4; // Codec_TIMEOUT_UserCallback();
	}

	/* Transmit the address lo byte for write operation */
	I2C_SendData(I2C1, RegisterAddr & 0xFF);

	/* Test on EV8 and clear it */
	Timeout = I2C_SHORT_TIMEOUT;
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING )) {
		if ((Timeout--) == 0)
			return 5; // Codec_TIMEOUT_UserCallback();
	}

	/* Prepare the register value to be sent */
	I2C_SendData(I2C1, RegisterValue);

	/*!< Wait till all data have been physically transferred on the bus */
	Timeout = I2C_LONG_TIMEOUT;
	while (!I2C_GetFlagStatus(I2C1, I2C_FLAG_BTF )) {
		if ((Timeout--) == 0)
			return 6; // Codec_TIMEOUT_UserCallback();
	}

	/* End the configuration sequence */
	I2C_GenerateSTOP(I2C1, ENABLE);

	Delay(100000); //hopefully around 5 msec???
	}
	/* Return the verifying value: 0 (Passed) or 1-5 (Failed) */
	return result;
}

//Read a single byte in eeprom. eeprom address is 14 bits long so address is an integer
uint32_t I2C_ReadEEProm(uint16_t RegisterAddr)
{
	uint32_t result = 0;

	/*!< While the bus is busy */
	__IO uint32_t Timeout = I2C_LONG_TIMEOUT;
	while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY )) {
		if ((Timeout--) == 0)
			return 1; //Codec_TIMEOUT_UserCallback();
	}

	/* Start the config sequence */
	I2C_GenerateSTART(I2C1, ENABLE);

	/* Test on EV5 and clear it */
	Timeout = I2C_SHORT_TIMEOUT;
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT )) {
		if ((Timeout--) == 0)
			return 2; //Codec_TIMEOUT_UserCallback();
	}

	/* Transmit the slave address and enable writing operation */
	I2C_Send7bitAddress(I2C1, EEPROM_Write_Address, I2C_Direction_Transmitter ); // EEPROM_Write_Address = 0b10100000

	/* Test on EV6 and clear it */
	Timeout = I2C_SHORT_TIMEOUT;
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED )) {
		if ((Timeout--) == 0)
			return 3; //Codec_TIMEOUT_UserCallback();
	}

	/* Transmit the register hi address to be read */
	I2C_SendData(I2C1, (RegisterAddr >> 8) & 0x3F);

	/* Test on EV8 and clear it */
	Timeout = I2C_SHORT_TIMEOUT;
	while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BTF ) == RESET) {
		if ((Timeout--) == 0)
			return 4; // Codec_TIMEOUT_UserCallback();
	}

	/* Transmit the register lo address to be read */
	I2C_SendData(I2C1, RegisterAddr & 0xFF);

	/* Test on EV8 and clear it */
	Timeout = I2C_SHORT_TIMEOUT;
	while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BTF ) == RESET) {
		if ((Timeout--) == 0)
			return 5; // Codec_TIMEOUT_UserCallback();
	}

	/*!< Send START condition a second time */
	I2C_GenerateSTART(I2C1, ENABLE);

	/*!< Test on EV5 and clear it (cleared by reading SR1 then writing to DR) */
	Timeout = I2C_SHORT_TIMEOUT;
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT )) {
		if ((Timeout--) == 0)
			return 6; //Codec_TIMEOUT_UserCallback();
	}

	/*!< Send eeprom address for w */
	I2C_Send7bitAddress(I2C1, EEPROM_Read_Address, I2C_Direction_Receiver ); //EEPROM_Read_Address =	0b10100001

	/* Wait on ADDR flag to be set (ADDR is still not cleared at this level */
	Timeout = I2C_SHORT_TIMEOUT;
	while (I2C_GetFlagStatus(I2C1, I2C_FLAG_ADDR ) == RESET) {
		if ((Timeout--) == 0)
			return 7; //Codec_TIMEOUT_UserCallback();
	}

	/*!< Disable Acknowledgment */
	I2C_AcknowledgeConfig(I2C1, DISABLE);

	/* Clear ADDR register by reading SR1 then SR2 register (SR1 has already been read) */
	(void) I2C1 ->SR2;

	/*!< Send STOP Condition */
	I2C_GenerateSTOP(I2C1, ENABLE);

	/* Wait for the byte to be received */
	Timeout = I2C_SHORT_TIMEOUT;
	while (I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE ) == RESET) {
		if ((Timeout--) == 0)
			return 8;  //Codec_TIMEOUT_UserCallback();
	}

	/*!< Read the byte received from the Codec */
	result = I2C_ReceiveData(I2C1 );

	/* Wait to make sure that STOP flag has been cleared */
	Timeout = I2C_SHORT_TIMEOUT;
	while (I2C1 ->CR1 & I2C_CR1_STOP ) {
		if ((Timeout--) == 0)
			return 9; //Codec_TIMEOUT_UserCallback();
	}

	/*!< Re-Enable Acknowledgment to be ready for another reception */
	I2C_AcknowledgeConfig(I2C1, ENABLE);

	/* Clear AF flag for next communication */
	I2C_ClearFlag(I2C1, I2C_FLAG_AF );

	/* Return the byte read from I2C Device */
	return result;
}

/**
 * @brief  Inserts a delay time (not accurate timing).
 * @param  nCount: specifies the delay time length.
 * @retval None
 */
void Delay(uint32_t nCount)
{
	volatile int i, j;
	for (i = 0; i < nCount; i++) {
		j++;
	}
}

