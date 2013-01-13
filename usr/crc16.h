#ifndef __CRC16_H
#define __CRC16_H
 
#include <stdint.h>
#include "stm32f0xx.h"
 

uint16_t crc16_byte(uint16_t crc, uint8_t data); 
uint16_t crc16(uint16_t crc, uint8_t *buffer, uint16_t len);
 
#endif /* __CRC16_H */
