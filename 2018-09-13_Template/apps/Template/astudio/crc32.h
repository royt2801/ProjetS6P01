/*
 * crc32.h
 *
 * Created: 2018-11-29 10:40:11
 *  Author: rolj1802
 */ 
#include "sys.h"
#include "phy.h"
#include <stdio.h>
#include <stdarg.h>


#ifndef CRC32_H_
#define CRC32_H_

uint32_t CRC32Table[256];
uint32_t reflect(uint32_t data, uint8_t dataLength);
void initCRC32Table(void);
uint32_t calculateCRC32(uint8_t* data, uint32_t dataLength, uint32_t crc32);


#endif /* CRC32_H_ */