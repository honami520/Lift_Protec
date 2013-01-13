#ifndef _LED_H__
#define _LED_H__
#include "stm32f0xx.h"

#define led_clk_l()     (GPIOB->BRR = GPIO_Pin_15)
#define led_clk_h()     (GPIOB->BSRR = GPIO_Pin_15)
#define led_lch_l()     (GPIOB->BRR = GPIO_Pin_14)
#define led_lch_h()     (GPIOB->BSRR = GPIO_Pin_14)
#define led_sdi_l()     (GPIOB->BRR = GPIO_Pin_13)
#define led_sdi_h()     (GPIOB->BSRR = GPIO_Pin_13)



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



#define LED_UP      1
#define LED_DOWN    4
#define LED_BASE    0
#define LED_DOLL    3
#define LED_HUM     2
#define LED_DAT     5
#define LED_FLOOR   7
#define LED_ERR     6


void led_init(void);
void led_send(uint32_t value);
void led_handle(void);
void seg_handle(void);


#endif


