/*
 * AsciiLib.h
 *
 *  Created on: Dec 19, 2012
 *      Author: CharleyK
 */

#ifndef ASCIILIB_H_
#define ASCIILIB_H_

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "stdint.h"

/* Private function prototypes -----------------------------------------------*/
void GetASCIICode1(unsigned char* pBuffer, unsigned char ASCII);
void GetASCIICode2(unsigned char* pBuffer, unsigned char ASCII);
void GetASCIICode3(short int * pBuffer, unsigned char ASCII);

void GetASCIICode4(unsigned char* pBuffer, unsigned char ASCII);
void GetASCIICode5(short int * pBuffer, unsigned char ASCII);

#endif /* ASCIILIB_H_ */
