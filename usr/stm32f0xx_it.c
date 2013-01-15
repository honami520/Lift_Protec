/**
  ******************************************************************************
  * @file    Project/STM32F0xx_StdPeriph_Templates/stm32f0xx_it.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    18-May-2012
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
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

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_it.h"
#include "uart.h"
#include "misc.h"

extern uint8_t floor_state_in;
extern uint8_t floor_state_out;
extern uint8_t floor_state_flag;

extern uint8_t eep_wr_flag;
extern uint8_t eep_wr_num;

extern uint8_t time_1ms_flag;
extern uint8_t time_10ms_flag;
extern uint8_t time_100ms_flag;
extern uint8_t time_1s_flag;

/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M0 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
    static uint16_t i = 0;
    static uint8_t j = 0;
    
    i ++;

    if((UP_READ() == 0) && (DOWN_READ() == 0))
    {
        floor_state_flag = 1;
    }
    else
    {
        floor_state_flag = 0;
    }


    if((i % 10) == 0)
    {
        time_1ms_flag = 1;
    }

    if((i % 100) == 0)
    {
        time_10ms_flag = 1;
    }
    
    if((i % 1000) == 0)
    {
        time_100ms_flag = 1;
    }   
    
    if((i % 10000) == 0)
    {
        time_1s_flag = 1;
        i = 0;
        j ++;
    }

    if(j == 60)
    {
        j = 0;
        eep_wr_num ++;

        if(eep_wr_num == 30)
        {
            //30分钟写入累计楼层，累计开门次数
            eep_wr_num = 0;
            eep_wr_flag = 1;
        }
    }
}


void USART1_IRQHandler(void)
{
    uint8_t tmp = 0;

    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        tmp = USART_ReceiveData(USART1);
        modbus_rec(tmp);
    }
}


void DMA1_Channel2_3_IRQHandler(void)
{
    if (DMA_GetITStatus(DMA1_IT_TC2) == SET)
    {
        DMA_ClearITPendingBit(DMA1_IT_TC2);
    }
}


void EXTI4_15_IRQHandler(void)
{
    uint8_t tf = 0;

    if(EXTI_GetITStatus(EXTI_Line13) != RESET)
    {
        /* Clear the EXTI line 13 pending bit */
        EXTI_ClearITPendingBit(EXTI_Line13);
        tf = 1;
    }

    if(EXTI_GetITStatus(EXTI_Line14) != RESET)
    {
        /* Clear the EXTI line 14 pending bit */
        EXTI_ClearITPendingBit(EXTI_Line14);
        tf = 1;
    }

    if(EXTI_GetITStatus(EXTI_Line15) != RESET)
    {
        /* Clear the EXTI line 15 pending bit */
        EXTI_ClearITPendingBit(EXTI_Line15);

        //到达基层，将当前楼层改成基层
        err_base_handle();          //基层判断，或者是基层对应指示灯两灭判断
    }

    if(tf == 1)
    {
        tf = 0;
        floor_state_handle();         //冲顶、蹲低故障判断、楼层判断
    }
}

/******************************************************************************/
/*                 STM32F0xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f0xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
