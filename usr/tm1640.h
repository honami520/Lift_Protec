#ifndef _TM1640_H__
#define _TM1640_H__
#include "stm32f0xx.h"


#define SDA_H()     (GPIOF->BSRR = GPIO_Pin_6)
#define SDA_L()     (GPIOF->BRR = GPIO_Pin_6)
#define SCL_H()     (GPIOF->BSRR = GPIO_Pin_7)
#define SCL_L()     (GPIOF->BRR = GPIO_Pin_7)
#define SDA_OUT()    GPIOF->MODER &= 0xffffcfff; GPIOF->MODER |= 0x0000d000
#define SDA_IN()     GPIOF->MODER &= 0xffffcfff
#define SDA_READ()   (GPIOF->IDR & GPIO_Pin_6)


#define SEG_0       0
#define SEG_1       1
#define SEG_2       2
#define SEG_3       3
#define SEG_4       4
#define SEG_5       5
#define SEG_6       6
#define SEG_7       7
#define SEG_8       8
#define SEG_9       9
#define SEG_A       10
#define seg_B       11
#define SEG_C       12
#define SEG_D       13
#define SEG_E       14
#define SEG_F       15
#define SEG_G       16
#define SEG_H       17
#define SEG_J       18
#define SEG_L       19
#define SEG_N       20
#define SEG_O       21
#define SEG_P       22
#define SEG_q       23
#define SEG_R       24
#define SEG_U       25
#define SEG_Y       26
#define SEG_HANG    27
#define SEG_BLANK   28



void udelay(uint16_t i);
void mdelay(uint16_t i);
void tm1640_init(void);
void tm1640_start(void);
void tm1640_stop(void);
void tm1640_write_byte(uint8_t dat);
void write_cmd(uint8_t cmd);
void write_dat(uint8_t cmd, uint8_t *buf);
void sg_write(void);


#endif


