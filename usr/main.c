#include "main.h"


void systick_init(void)
{
    if(SysTick_Config(SystemCoreClock / 1000))
    {
        while(1);
    }
}


//累计开门次数、累计运行层数写入函数
void eep_wr_handle(void)
{
    if(eep_wr_flag == 1)
    {
        //系统累计开门次数写入eeprom
        mod_buf_write32(PS_LF_NUMH, num_b);
        eep_write32(REG_NUM, num_b);

        //累计运行时间写入eeprom
        mod_buf_write32(PS_LF_TIMEH, time_lj);
        eep_write32(REG_TIME, time_lj);

        eep_wr_flag = 0;
    }
}


int main(void)
{
    systick_init();
    flash_init();
    eep_init();         //读取EEPROM内的数据
    addr_init();        //MODBUS地址初始化
    led_init();         //LED初始化
    tm1640_init();
    uart_init();
    input_init();
    MMA845x_init();     //加速度传感器初始化
    
    while (1)
    {
        err_cd_dd_handle();     //冲顶、蹲低故障判断、对应的指示灯两灭判断

        if (time_1ms_flag == 1)
        {
            //1ms进入一次
            time_1ms_flag = 0;

            err_base_handle();          //基层判断，或者是基层对应指示灯两灭判断
            uart_clear();               //串口清除函数
            yxsj_handle();              //累计时间函数
        }

        if(time_10ms_flag == 1)
        {
            //10ms进入一次
            time_10ms_flag = 0;

            led_state_handle();
            led_handle();           //LED状态10ms更新一次
            err_kmzt_handle();      //开门走梯故障判断
            err_cs_handle();        //超速故障判断
            err_yxcs_handle();      //运行超时故障判断
        }

        if(time_100ms_flag == 1)
        {
            //100ms进入一次
            time_100ms_flag = 0;

            hum_door_check_handle();    //厢门检测，人体红外检测
            err_mqwtt_handle();         //门区外停梯故障判断
            seg_handle();               //数码管刷新显示
            MMA845x_read();             //加速度读取
            door_num_handle();          //开门次数计算
        }

        if(time_1s_flag == 1)
        {
            //1s执行一次
            time_1s_flag = 0;

            err_pckr_handle();          //平层困人故障判断
            err_fpckr_handle();         //非平层困人故障判断
            err_mqwkm_handle();         //门区外开门故障判断
            err_csjkm_handle();         //长时间开门故障检测
            err_ffkgm_handle();         //反复开关门故障检测
        }

        modbus_send();                  //MODBUS发送准备
        eep_wr_handle();                //累计开门次数、累计运行楼层写入函数
    }
}



#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
