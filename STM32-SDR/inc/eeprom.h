/*
 * eeprom.h
 *
 *  Created on: Mar 14, 2013
 *      Author: CharleyK
 */


void EEProm_Test(void);
void check_eeprom(void);

int16_t Write_Int_EEProm(uint16_t EEProm_Addr, int16_t value);
int16_t Read_Int_EEProm(uint16_t EEProm_Addr);
uint32_t Write_Long_EEProm(uint16_t EEProm_Addr, uint32_t value);
uint32_t Read_Long_EEProm(uint16_t EEProm_Addr);

void Store_SI570_Data(void);
//void Read_SI570_Data(void);
