#include "main.h"


void systick_init(void)
{
    if(SysTick_Config(SystemCoreClock / 1000))
    {
        while(1);
    }
}


//�ۼƿ��Ŵ������ۼ����в���д�뺯��
void eep_wr_handle(void)
{
    if(eep_wr_flag == 1)
    {
        //ϵͳ�ۼƿ��Ŵ���д��eeprom
        mod_buf_write32(PS_LF_NUMH, num_b);
        eep_write32(REG_NUM, num_b);

        //�ۼ�����ʱ��д��eeprom
        mod_buf_write32(PS_LF_TIMEH, time_lj);
        eep_write32(REG_TIME, time_lj);

        eep_wr_flag = 0;
    }
}


int main(void)
{
    systick_init();
    flash_init();
    eep_init();         //��ȡEEPROM�ڵ�����
    addr_init();        //MODBUS��ַ��ʼ��
    led_init();         //LED��ʼ��
    tm1640_init();
    uart_init();
    input_init();
    MMA845x_init();     //���ٶȴ�������ʼ��
    
    while (1)
    {
        err_cd_dd_handle();     //�嶥���׵͹����жϡ���Ӧ��ָʾ�������ж�

        if (time_1ms_flag == 1)
        {
            //1ms����һ��
            time_1ms_flag = 0;

            err_base_handle();          //�����жϣ������ǻ����Ӧָʾ�������ж�
            uart_clear();               //�����������
            yxsj_handle();              //�ۼ�ʱ�亯��
        }

        if(time_10ms_flag == 1)
        {
            //10ms����һ��
            time_10ms_flag = 0;

            led_state_handle();
            led_handle();           //LED״̬10ms����һ��
            err_kmzt_handle();      //�������ݹ����ж�
            err_cs_handle();        //���ٹ����ж�
            err_yxcs_handle();      //���г�ʱ�����ж�
        }

        if(time_100ms_flag == 1)
        {
            //100ms����һ��
            time_100ms_flag = 0;

            hum_door_check_handle();    //���ż�⣬���������
            err_mqwtt_handle();         //������ͣ�ݹ����ж�
            seg_handle();               //�����ˢ����ʾ
            MMA845x_read();             //���ٶȶ�ȡ
            door_num_handle();          //���Ŵ�������
        }

        if(time_1s_flag == 1)
        {
            //1sִ��һ��
            time_1s_flag = 0;

            err_pckr_handle();          //ƽ�����˹����ж�
            err_fpckr_handle();         //��ƽ�����˹����ж�
            err_mqwkm_handle();         //�����⿪�Ź����ж�
            err_csjkm_handle();         //��ʱ�俪�Ź��ϼ��
            err_ffkgm_handle();         //���������Ź��ϼ��
        }

        modbus_send();                  //MODBUS����׼��
        eep_wr_handle();                //�ۼƿ��Ŵ������ۼ�����¥��д�뺯��
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
