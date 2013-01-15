/**
  ******************************************************************************
  * @file    Project/STM32F0xx_StdPeriph_Templates/main.h 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    18-May-2012
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"
#include <stdio.h>
#include "eeprom.h"
#include "flash_eep.h"
#include "tm1640.h"
#include "crc16.h"
#include "led.h"
#include "misc.h"
#include "mma8451.h"
#include "uart.h"

uint8_t send_buf[50] = {0};
uint8_t send_num = 0;
uint8_t *buf_head = send_buf;
uint8_t rec_buf[10] = {0};
uint8_t rec_num = 0;
uint8_t mb_step = 0;


uint8_t send_flag = 0;      //发送标志
uint8_t clear_flag = 0;
uint16_t clear_num = 0;

uint8_t mb_send_flag = 0;
uint8_t mb_send_num = 0;
uint8_t mb_send_size = 0;



uint8_t time_1ms_flag = 0;
uint8_t time_10ms_flag = 0;
uint8_t time_100ms_flag = 0;
uint8_t time_1s_flag = 0;


uint8_t err_flag = 0;       //故障标志
uint8_t dev_addr = 0;       //设备地址

//eeprom
uint16_t VirtAddVarTab[NB_OF_VAR] =
{
    0xaa00, 0xaa01, 0xaa02, 0xaa03, 0xaa04, 0xaa05, 0xaa06, 0xaa07,
    0xaa08, 0xaa09, 0xaa0a, 0xaa0b, 0xaa0c, 0xaa0d, 0xaa0e, 0xaa0f,
    0xaa10,
};


//数码管
uint8_t seg_dat[29] = {
    0x3F, // 0
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x6F, // 9
    0x77, // A
    0x7C, // b
    0x39, // C
    0x5E, // d
    0x79, // E
    0x71, // F
    0x3D, // G
    0x76, // H
    0x0E, // J
    0x38, // L
    0x37, // N
    0x5C, // o
    0x73, // P
    0x67, // q
    0x50, // r
    0x3E, // U
    0x6E, // Y
    0x40, // -
    0x00  // 不显示
};
    
uint8_t seg_buf[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t seg_back[2] = {0, 0};

uint8_t led_buf[8] = {0, 0, 0, 0, 0, 0, 0, 0};
uint32_t time_lj = 0;           //累计运行时间
uint32_t time_b = 0;            //临时运行时间，单位ms，累计到1分钟清零
uint8_t time_b_flag = 0;        //累计运行时间启动标志
uint32_t num_b = 0;             //累计开门次数

uint8_t eep_wr_num = 0;         //1分钟累计
uint8_t eep_wr_flag = 0;        //30分钟写入EEP楼层累计，开门次数

uint8_t floor_state_in = 0;     //电梯进入的状态
uint8_t floor_state_out = 0;    //电梯移出的状态
uint8_t floor_state_flag = 0;   //电梯楼层判断

//故障判断
uint16_t pckr_num = 0;
uint8_t pckr_r_num = 0;
uint8_t pckr_step = 0;

uint16_t mqwtt_num = 0;

uint16_t mqwkm_num = 0;

uint16_t csjkm_num = 0;

uint16_t kmzt_num = 0;

uint16_t ffkgm_num = 0;
uint8_t ffkgm_step = 0;

uint16_t fpckr_num = 0;
uint8_t fpckr_r_num = 0;
uint8_t fpckr_step = 0;

uint32_t cs_dat = 0;
uint8_t cs_step = 0;

uint32_t yxcs_num = 0;
uint16_t yxcs_num1 = 0;
uint8_t yxcs_step = 0;

uint8_t floor_base_flag = 0;
int16_t floor_now = 0;
uint8_t floor_step = 0;
uint8_t floor_base_num = 0;


uint16_t door_num = 0;
uint8_t door_flag = 0;


//modbus相关
uint16_t mod_buf[MODBUS_SIZE];

uint8_t cal_flag = 0;
uint8_t cal_step = 0;
int8_t xmax = 0, xmin = 0;
int8_t ymax = 0, ymin = 0;
int8_t zmax = 0, zmin = 0;
uint8_t x_s = 0, y_s = 0, z_s = 0;



void systick_init(void);


#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
