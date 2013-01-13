#ifndef _FLASH_EEP_H__
#define _FLASH_EEP_H__

#include "stm32f0xx.h"
#include "eeprom.h"

#define PASS_WORD       0x1f2e3c4d
#define REG_PASS        0
#define REG_TIME        2
#define REG_NUM         4
#define REG_UP          6
#define REG_DOWN        7
#define REG_BASE        8
#define REG_PCKR        9
#define REG_FPCKR       10
#define REG_MQWTT       11
#define REG_CS          12
#define REG_YXCS        13
#define REG_FFKGM       14
#define REG_CSJKM       15
#define REG_LCPJJJ      16



void flash_init(void);
void eep_write16(uint8_t addr, uint16_t dat);
void eep_write32(uint8_t addr, uint32_t dat);
uint16_t eep_read16(uint8_t addr);
uint32_t eep_read32(uint8_t addr);
void eep_init(void);



#endif













