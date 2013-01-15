#ifndef MISC_H
#define MISC_H
#include "stm32f0xx.h"


#define DOWN_READ()     ((GPIOC->IDR & GPIO_Pin_13) >> 13)
#define UP_READ()       ((GPIOC->IDR & GPIO_Pin_14) >> 14)
#define BASE_READ()     ((GPIOC->IDR & GPIO_Pin_15) >> 15)
#define DOOR_READ()     ((GPIOB->IDR & GPIO_Pin_8) >> 8)
#define HUM_READ()      ((GPIOB->IDR & GPIO_Pin_9) >> 9)


#define DIR_UP      1
#define DIR_DOWN    2






void addr_init(void);
void input_init(void);
void err_pckr_handle(void);
void err_fpckr_handle(void);
void err_mqwtt_handle(void);
void err_mqwkm_handle(void);
void err_csjkm_handle(void);
void err_kmzt_handle(void);
void err_ffkgm_handle(void);
void err_cs_handle(void);
void err_yxcs_handle(void);
void floor_state_handle(void);
void err_cd_dd_handle(void);
void err_base_handle(void);
void hum_door_check_handle(void);
void door_num_handle(void);
void led_state_handle(void);
void yxsj_handle(void);



#endif

