#include "misc.h"
#include "led.h"
#include "uart.h"


extern uint8_t time_b_flag;
extern uint32_t time_b;

extern uint16_t door_num;
extern uint8_t door_flag;


extern uint8_t dev_addr;
extern uint32_t time_lj;
extern uint32_t num_b;
extern uint8_t err_flag;
//�����ж�
extern uint16_t pckr_num;
extern uint8_t pckr_r_num;
extern uint8_t pckr_step;

extern uint16_t fpckr_num;
extern uint8_t fpckr_r_num;
extern uint8_t fpckr_step;

extern uint16_t mqwkm_num;

extern uint16_t mqwtt_num;

extern uint16_t csjkm_num;

extern uint16_t kmzt_num;

extern uint16_t ffkgm_num;
extern uint8_t ffkgm_step;

extern uint32_t cs_dat;
extern uint8_t cs_step;

extern uint32_t yxcs_num;
extern uint16_t yxcs_num1;
extern uint8_t yxcs_step;

extern uint8_t floor_base_flag;
extern int16_t floor_now;
extern uint8_t floor_step;
extern uint8_t floor_base_num;

extern int8_t xmax, xmin;
extern int8_t ymax, ymin;
extern int8_t zmax, zmin;
extern uint8_t cal_flag;
extern uint8_t cal_step;
extern uint8_t x_s, y_s, z_s;

extern uint16_t mod_buf[MODBUS_SIZE];
extern uint8_t led_buf[8];



void addr_init(void)
{
    //��ַ�̶�Ϊ0xfe��ǰ��ı���������չ
    dev_addr = 0xf7;
}


void input_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    EXTI_InitTypeDef EXTI_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOC, ENABLE);

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_1;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    //PC13��PC14��PC15�ж�����
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    /* Connect EXTI13 Line to PC13 pin */
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource13);

    /* Connect EXTI14 Line to PC14 pin */
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource14);

    /* Connect EXTI15 Line to PC15 pin */
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource15);

    /* Configure EXTI13 line */
    EXTI_InitStructure.EXTI_Line = EXTI_Line13;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* Configure EXTI14 line */
    EXTI_InitStructure.EXTI_Line = EXTI_Line14;
    EXTI_Init(&EXTI_InitStructure);

    /* Configure EXTI15 line */
    EXTI_InitStructure.EXTI_Line = EXTI_Line15;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_Init(&EXTI_InitStructure);

    /* Enable and set EXTI4_15 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = EXTI4_15_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

//ƽ�������ж�
//1�����ж�һ��
void err_pckr_handle(void)
{
    if((UP_READ() == 1) && (DOWN_READ() == 1) && (DOOR_READ() == 0))
    {
        //��ƽ�У���ƽ�У��Źرգ���ѯT1ʱ�����Ƿ���3���˳���
        pckr_num ++;

        //�ж����������ִ���
        if(pckr_step == 0)
        {
            if(HUM_READ() == 0)
            {
                //���ˣ���ȴ�
            }
            else
            {
                //���ˣ���������ȴ�����
                pckr_r_num ++;
                pckr_step = 1;
            }
        }
        else
        {
            if(HUM_READ() == 0)
            {
                //������ˣ�������ٴ��ж�����
                pckr_step = 0;
            }
            else
            {
                //���ˣ����ԣ��ȴ�
            }           
        }

        //�ж�ʱ���Ƿ�ﵽ
        if(pckr_num >= mod_buf_read8(PS_TM_PCKR))
        {
            //�ж�������ִ���
            if(pckr_r_num > 3)
            {
                //����ƽ������
                mod_buf[PS_FL_ERR] |= B_PCKR;
            }
            else
            {
                //ƽ����������
                mod_buf[PS_FL_ERR] &= ~(B_PCKR);
            }

            //�м������������¿�ʼ���
            pckr_num = 0;
            pckr_r_num = 0;
            pckr_step = 0;
        }
        
    }
    else
    {
        //������������������
        pckr_num = 0;
        pckr_r_num = 0;
        pckr_step = 0;
        mod_buf[PS_FL_ERR] &= ~(B_PCKR);
    }
}


//��ƽ�������ж�
//1�����ж�һ��
void err_fpckr_handle(void)
{
    if((UP_READ() == 0) && (DOWN_READ() == 0))
    {
        //��ƽ�ޣ���ƽ�ޣ�T3ʱ�����Ƿ���1���˳���
        fpckr_num ++;

        //�ж����������ִ���
        if(fpckr_step == 0)
        {
            if(HUM_READ() == 0)
            {
                //���ˣ���ȴ�
            }
            else
            {
                //���ˣ���������ȴ�����
                fpckr_r_num ++;
                fpckr_step = 1;
            }
        }
        else
        {
            if(HUM_READ() == 0)
            {
                //������ˣ�������ٴ��ж�����
                fpckr_step = 0;
            }
            else
            {
                //���ˣ����ԣ��ȴ�
            }           
        }

        //�ж�ʱ���Ƿ�ﵽ
        if(fpckr_num >= mod_buf_read8(PS_TM_FPCKR))
        {
            //�ж�������ִ���
            if(fpckr_r_num > 1)
            {
                //���ַ�ƽ������
                mod_buf[PS_FL_ERR] |= B_FPCKR;
            }
            else
            {
                //��ƽ����������
                mod_buf[PS_FL_ERR] &= ~(B_FPCKR);
            }

            //�м������������¿�ʼ���
            fpckr_num = 0;
            fpckr_r_num = 0;
            fpckr_step = 0;
        }
        
    }
    else
    {
        fpckr_num = 0;
        fpckr_r_num = 0;
        fpckr_step = 0;

        mod_buf[PS_FL_ERR] &= ~(B_FPCKR);
    }
}




//������ͣ�ݼ��
//100������1��
void err_mqwtt_handle(void)
{
    if((UP_READ() == 0) && (DOWN_READ() == 0) && (DOOR_READ() == 0)) 
    {
        //��ƽ�ޣ���ƽ�ޣ��Źر�
        mqwtt_num ++;

        //�ж�ʱ���Ƿ�ﵽ
        if(mqwtt_num >= (10 * mod_buf_read8(PS_TM_MQWTT)))
        {
            //����������ͣ��
            mod_buf[PS_FL_ERR] |= B_MQWTT;
            
            //�м������������¿�ʼ���
            mqwtt_num = 0;
        }       
    }
    else
    {
        mqwtt_num = 0;
        //���������ͣ��
        mod_buf[PS_FL_ERR] &= ~(B_MQWTT);
    }
}




//�����⿪��
//1���Ӽ��1��
void err_mqwkm_handle(void)
{
    if((UP_READ() == 0) && (DOWN_READ() == 0) && (DOOR_READ() == 1)) 
    {
        //��ƽ�ޣ���ƽ�ޣ��Ŵ�
        mqwkm_num ++;

        //�ж�ʱ���Ƿ�ﵽ
        if(mqwkm_num >= 2)
        {
            //���������⿪��
            mod_buf[PS_FL_ERR] |= B_MQWKM;
            
            //�м������������¿�ʼ���
            mqwkm_num = 0;
        }       
    }
    else
    {
        mqwkm_num = 0;
        //��������⿪��
        mod_buf[PS_FL_ERR] &= ~(B_MQWKM);
    }   
}




//��ʱ�俪��
//1���Ӽ��1��
void err_csjkm_handle(void)
{
    if(DOOR_READ() == 1)
    {
        //�Ŵ�
        csjkm_num ++;

        if(csjkm_num >= (mod_buf_read8(PS_TM_CSJKM) * 60))
        {
            //��ʱ�俪��
            mod_buf[PS_FL_ERR] |= B_CSJKM;
            csjkm_num = 0;
        }
    }
    else
    {
        csjkm_num = 0;
        mod_buf[PS_FL_ERR] &= ~(B_CSJKM);
    }
}



//����������
//1���Ӽ��1��
void err_ffkgm_handle(void)
{
    if((UP_READ() == 1) && (DOWN_READ() == 1))
    {
        //һ��ͣ�ݿ����Ŵ���
        if(ffkgm_step == 0)
        {
            if(DOOR_READ() == 0)
            {
                //�Źرգ��ȴ��Ŵ�
                ffkgm_step = 1;
            }
            else
            {
                //�Ŵ򿪣��ȴ��Źر�
                ffkgm_step = 2;
            }
        }
        else if(ffkgm_step == 1)
        {
            //�ȴ��Ŵ�
            if(DOOR_READ() == 1)
            {
                //���Ѿ��򿪣��ȴ��Źر�
                ffkgm_num ++;
                ffkgm_step = 2;
            }
        }
        else if(ffkgm_step == 2)
        {
            //�ȴ��Źر�
            if(DOOR_READ() == 0)
            {
                //���Ѿ��رգ��ȴ��Ŵ�
                ffkgm_num ++;
                ffkgm_step = 1;
            }
        }

        //�жϿ����Ŵ����Ƿ񳬱�
        if(ffkgm_num >= mod_buf_read8(PS_NM_FFKGM))
        {
            //���������ų���
            mod_buf[PS_FL_ERR] |= B_FFKGM;
            ffkgm_num = 0;
            ffkgm_step = 0;
        }
    }
    else
    {
        ffkgm_num = 0;
        ffkgm_step = 0;
        mod_buf[PS_FL_ERR] &= ~(B_FFKGM);
    }
}




//��������
//10ms���һ��
void err_kmzt_handle(void)
{
    if((DOOR_READ() == 1) && (((UP_READ() == 1) && (DOWN_READ() == 0)) || ((UP_READ() == 0) && (DOWN_READ() == 1))))
    {
        //��������
        kmzt_num ++;

        if(kmzt_num >= 2)
        {
            //�������ݳ���
            mod_buf[PS_FL_ERR] |= B_KMZT;
            kmzt_num = 0;
        }
    }
    else if((DOOR_READ() == 0) && (((UP_READ() == 1) && (DOWN_READ() == 0)) || ((UP_READ() == 0) && (DOWN_READ() == 1))))
    {
        //�������ݣ����
        mod_buf[PS_FL_ERR] &= ~(B_KMZT);
        kmzt_num = 0;
    }
    else
    {
        kmzt_num = 0;
    }
}


//���ټ���
//10ms���һ��
void err_cs_handle(void)
{
    uint16_t tmp = 0;
    uint16_t tmp1 = 0;
    
    if(cs_step == 0)
    {
        //�����ʱ�򣬵ȴ�����ĳһ��
        if((UP_READ() == 1) && (DOWN_READ() == 1))
        {
            //��ƽ�У���ƽ��
            cs_step = 1;
        }       
    }
    else if(cs_step == 1)
    {
        //�ȴ��Ƴ���ǰ¥��
        if((UP_READ() == 0) && (DOWN_READ() == 0))
        {
            //�Ƴ�ĳ¥�㣬ʱ������
            cs_step = 2;
            cs_dat = 0;
        }
    }
    else if(cs_step == 2)
    {
        //�ȴ���ĳ��¥��
        //�ۼ�ʱ������
        cs_dat ++;
        
        if((UP_READ() == 1) && (DOWN_READ() == 1))
        {
            //�����Ƿ���
            tmp = mod_buf_read8(PS_NM_LCPJJJ);
            tmp1 = mod_buf_read8(PS_TM_CS);
            
            if(((10 * tmp) / cs_dat) > tmp1)
            {
                //��������
                mod_buf[PS_FL_ERR] |= B_CS;
            }
            else
            {
                //�ǳ������У����
                mod_buf[PS_FL_ERR] &= ~(B_CS);
            }

            //������ȴ���һ�μ��
            cs_step = 0;
            cs_dat = 0;
        }
    }

}



//���г�ʱ���
//10ms���һ��
void err_yxcs_handle(void)
{
    uint16_t tmp = 0;
    
    if(yxcs_step == 0)
    {
        //�����ʱ�򣬵ȴ�����ĳһ��
        if((UP_READ() == 1) && (DOWN_READ() == 1))
        {
            //��ƽ�У���ƽ��
            yxcs_step = 1;
        }       
    }
    else if(yxcs_step == 1)
    {
        //�ȴ��Ƴ���ǰ¥��
        if((UP_READ() == 0) && (DOWN_READ() == 0))
        {
            //ȡ���Ƴ�ĳ¥��
            yxcs_step = 2;

            //���ٶȼ�����ʼ
            if(cal_flag == 0)
            {
                //�������ٶȼ�����ֱ��ͣ��
                cal_flag = 1;
                cal_step = 0;
                xmax = 0;
                xmin = 0;
                ymax = 0;
                ymin = 0;
                zmax = 0;
                zmin = 0;
                x_s = 0;
                y_s = 0;
                z_s = 0;                
            }
        }
    }
    else if(yxcs_step == 2)
    {
        yxcs_num ++;
        
        //��������ʱ���Ƿ�ʱ
        if((mod_buf[PS_FL_ERR] & (B_MQWTT | B_MQWKM | B_FPCKR)) != 0)
        {
            //�����⿪�ţ�������ͣ�ݣ���ƽ������ʱ�����
            yxcs_num = 0;
            yxcs_num1 = 0;
            yxcs_step = 0;

            mod_buf[PS_FL_ERR] &= ~(B_YXCS);
        }

        //����Ƿ�ͣ��
        if((UP_READ() == 1) && (DOWN_READ() == 1))
        {
            //ͣ�ݴﵽ�㹻ʱ�䣬���
            yxcs_num1 ++;

            if(yxcs_num1 >= 200)
            {
                //ͣ����Ч�����
                yxcs_num1 = 0;
                yxcs_num = 0;
                yxcs_step = 0;

                mod_buf[PS_FL_ERR] &= ~(B_YXCS);

                //ƽ��
                mod_buf[PS_FL_STATE / 2] |= B_DT_PING;
                mod_buf[PS_FL_STATE / 2] &= ~(B_DT_DOWN);
                mod_buf[PS_FL_STATE / 2] &= ~(B_DT_UP);


                //���ٶȽ������߿���
                if(cal_flag == 1)
                {
                    //����������modbus
                    if(xmax >= 0)
                    {
                        if(xmin >= 0)
                        {
                            tmp = xmax - xmin;
                        }
                        else
                        {
                            tmp = xmax + (-xmin);
                        }
                    }
                    else
                    {
                        tmp = (-xmin) - (-xmax);
                    }

                    //������ٶ�10��
                    tmp = (tmp * 20) / 128;
                    mod_buf_write8(PS_NM_X, tmp);

                    if(ymax >= 0)
                    {
                        if(ymin >= 0)
                        {
                            tmp = ymax - ymin;
                        }
                        else
                        {
                            tmp = ymax + (-ymin);
                        }
                    }
                    else
                    {
                        tmp = (-ymin) - (-ymax);
                    }       

                    //������ٶ�10��
                    tmp = (tmp * 20) / 128;
                    mod_buf_write8(PS_NM_Y, tmp);

                    if(zmax >= 0)
                    {
                        if(zmin >= 0)
                        {
                            tmp = zmax - zmin;
                        }
                        else
                        {
                            tmp = zmax + (-zmin);
                        }
                    }
                    else
                    {
                        tmp = (-zmin) - (-zmax);
                    }

                    //������ٶ�10��
                    tmp = (tmp * 20) / 128;
                    mod_buf_write8(PS_NM_Z, tmp);

                    //��������
                    cal_flag = 0;
                    cal_step = 0;
                    xmax = 0;
                    xmin = 0;
                    ymax = 0;
                    ymin = 0;
                    zmax = 0;
                    zmin = 0;
                    x_s = 0;
                    y_s = 0;
                    z_s = 0;
                }

            }
        }
        else
        {
            yxcs_num1 = 0;
        }

        if(DOOR_READ() == 1)
        {
            //�Ŵ򿪣������
            yxcs_num = 0;
            yxcs_num1 = 0;
            yxcs_step = 0;
        }
        
        if((yxcs_num / 6000) >= mod_buf_read8(PS_TM_YXCS))
        {
            //���г�ʱ��Ч
            mod_buf[PS_FL_ERR] |= B_YXCS;
            yxcs_num = 0;
            yxcs_num1 = 0;
            yxcs_step = 0;
        }
    }

}




//1ms�ж�һ��
void err_cd_dd_handle(void)
{
    static uint16_t tmp = 0;
    
    if(floor_base_flag == 1)
    {
        //��֪����¥���£��жϳ嶥
        if(floor_step == 0)
        {
            if((UP_READ() == 1) && (DOWN_READ() == 1))
            {
                //ƽ�㿪ʼ����ִ���ж�1
                floor_step = 1;

                //ƽ�������嶥
                mod_buf[PS_FL_ERR] &= ~B_CD;
                mod_buf[PS_FL_ERR] &= ~B_DD;
            }
        }
        else if(floor_step == 1)
        {
            //��ƽ�������ж�
            if((UP_READ() == 0) && (DOWN_READ() == 1))
            {
                //��������
                floor_step = 2;

                mod_buf[PS_FL_STATE / 2] |= B_DT_UP;
                mod_buf[PS_FL_STATE / 2] &= ~(B_DT_DOWN);
                mod_buf[PS_FL_STATE / 2] &= ~(B_DT_PING);

                //LED
                led_buf[LED_BASE] = 0;


                //�嶥�ж�
                if(floor_now > 0)
                {
                    if(floor_now == mod_buf_read8(PS_FL_UP))
                    {
                        //�嶥
                        mod_buf[PS_FL_ERR] |= B_CD;
                    }               
                }
            }
            else if((UP_READ() == 1) && (DOWN_READ() == 0))
            {
                //��������
                floor_step = 3;

                mod_buf[PS_FL_STATE / 2] |= B_DT_DOWN;
                mod_buf[PS_FL_STATE / 2] &= ~(B_DT_UP);
                mod_buf[PS_FL_STATE / 2] &= ~(B_DT_PING);

                //LED
                led_buf[LED_BASE] = 0;


                //�׵��ж�
                if(floor_now < 0)
                {
                    if((-floor_now) == mod_buf_read8(PS_FL_DOWN))
                    {
                        //�׵�
                        mod_buf[PS_FL_ERR] |= B_DD;
                    }
                }
            }
        }
        else if(floor_step == 2)
        {
            //�ȴ��Ƴ�¥��
            if((UP_READ() == 0) && (DOWN_READ() == 0))
            {
                //����ȴ�����ƽ�У���ƽ��
                tmp ++;

                if(tmp > 2)
                {
                    tmp = 0;
                    floor_step = 4;
                }
            }
            else
            {
                tmp = 0;
            }
        }
        else if(floor_step == 3)
        {
            //�ȴ��Ƴ�¥��
            if((UP_READ() == 0) && (DOWN_READ() == 0))
            {
                //����ȴ�����ƽ�У���ƽ��
                tmp ++;

                if(tmp > 2)
                {
                    tmp = 0;
                    floor_step = 6;
                }
            }   
            else
            {
                tmp = 0;
            }
        }
        else if(floor_step == 4)
        {
            //�ȴ���ƽ�У���ƽ�޳��֣������˳�
            if((UP_READ() == 1) && (DOWN_READ() == 0))
            {
                //¥��ȴ�����
                floor_step = 5;
            }
            else if((UP_READ() == 0) && (DOWN_READ() == 1))
            {
                floor_step = 0;
            }
        }
        else if(floor_step == 5)
        {
            //�ȴ�ƽ�����
            if((UP_READ() == 1) && (DOWN_READ() == 1))
            {
                //ƽ�㣬¥������
                floor_now ++;

                if(floor_now > 0)
                {
                    if(floor_now > mod_buf_read8(PS_FL_UP))
                    {
                        floor_now = mod_buf_read8(PS_FL_UP);
                    }               
                }


                if(floor_now == 0)
                {
                    floor_now = 1;
                }

                if(floor_now >= 0)
                {
                    mod_buf_write8(PS_FL_DAT, floor_now);
                }
                else
                {
                    mod_buf_write8(PS_FL_DAT, (0xe0 + (-floor_now)));
                }

                floor_step = 0;

                //����嶥
                mod_buf[PS_FL_ERR] &= ~B_CD;
            }
            else if((UP_READ() == 0) && (DOWN_READ() == 0))
            {
                floor_step = 0;
            }
        }
        else if(floor_step == 6)
        {
            //�ȴ���ƽ�ޣ���ƽ�г��֣������˳�
            if((UP_READ() == 0) && (DOWN_READ() == 1))
            {
                //¥��ȴ�����
                floor_step = 7;
            }
            else if((UP_READ() == 1) && (DOWN_READ() == 0))
            {
                floor_step = 0;
            }
        }
        else if(floor_step == 7)
        {
            //�ȴ�ƽ��
            if((UP_READ() == 1) && (DOWN_READ() == 1))
            {
                //ƽ�㣬¥���С
                if(floor_now > 0)
                {
                    floor_now --;
                }
                else if(floor_now < 0)
                {
                    //����
                    if((-floor_now) < mod_buf_read8(PS_FL_DOWN))
                    {
                        floor_now --;
                    }
                }

                if(floor_now == 0)
                {
                    //����0��
                    floor_now = -1;
                }

                if(floor_now >= 0)
                {
                    mod_buf_write8(PS_FL_DAT, floor_now);
                }
                else
                {
                    mod_buf_write8(PS_FL_DAT, (0xe0 + (-floor_now)));
                }

                
                floor_step = 0;

                //����׵�
                mod_buf[PS_FL_ERR] &= ~B_DD;
            }  
            else if((UP_READ() == 0) && (DOWN_READ() == 0))
            {
                floor_step = 0;
            }
        }
    }
}


//�����ж�
//1ms����һ��
void err_base_handle(void)
{
    if(floor_base_flag == 0)
    {
        if(BASE_READ() == 0)
        {
            floor_base_num ++;

            if(floor_base_num >= 10)
            {
                floor_base_flag = 1;
                floor_now = mod_buf_read8(PS_FL_BASE);
                floor_base_num = 0;
            }
        }
        else
        {
            floor_base_num = 0;
        }
    }
    else
    {
        //�Ѿ������������ʾ����״̬������
        if(BASE_READ() == 0)
        {
            led_buf[LED_BASE] = 1;
            floor_now = mod_buf_read8(PS_FL_BASE);
            
            if(floor_now >= 0)
            {
                mod_buf_write8(PS_FL_DAT, floor_now);
            }
            else
            {
                mod_buf_write8(PS_FL_DAT, (0xe0 + (-floor_now)));
            }
        }
        else
        {
            led_buf[LED_BASE] = 0;
        }
    }
}



//�ſ��غͺ���״̬���
//100ms����1��
void hum_door_check_handle(void)
{
    if(DOOR_READ() == 1)
    {
        //�Ŵ�
        mod_buf[PS_FL_STATE / 2] |= B_DOOR;
        //��LED��
        led_buf[LED_DOLL] = 1;
    }
    else
    {
        //�Ź���
        mod_buf[PS_FL_STATE / 2] &= ~(B_DOOR);
        //��LED��
        led_buf[LED_DOLL] = 0;
    }

    if(HUM_READ() == 1)
    {
        //����
        mod_buf[PS_FL_STATE / 2] |= B_HUM;
        //����LED��
        led_buf[LED_HUM] = 1;
    }
    else
    {
        mod_buf[PS_FL_STATE / 2] &= ~(B_HUM);
        //����LED��
        led_buf[LED_HUM] = 0;
    }

    if(mod_buf[PS_FL_ERR] != 0)
    {
        led_buf[LED_ERR] = 1;
        led_buf[LED_FLOOR] = 0;
        err_flag = 1;
    }
    else
    {
        led_buf[LED_ERR] = 0;
        led_buf[LED_FLOOR] = 1;
        err_flag = 0;
    }
}



//100msִ��һ�Σ���¼�Ŵ򿪴���
void door_num_handle(void)
{
    if(door_flag == 0)
    {
        //�ȴ��Źر�
        if(DOOR_READ() == 0)
        {
            door_num ++;

            if(door_num > 10)
            {
                //�ȴ��Ŵ�
                door_num = 0;
                door_flag = 1;
            }
        }
        else
        {
            door_num = 0;
        }
    }
    else if(door_flag == 1)
    {
        //�ȴ��Ŵ�
        if(DOOR_READ() == 1)
        {
            door_num ++;

            if(door_num > 10)
            {
                door_num = 0;
                door_flag = 0;

                //���Ѿ���
                num_b ++;
                mod_buf_write32(PS_LF_NUMH, num_b);
            }
        }
        else
        {
            door_num = 0;
        }
    }
}

//���¹���ӦLED״̬
void led_state_handle(void)
{
    if(UP_READ() == 1)
    {
        led_buf[LED_UP] = 0;
    }
    else
    {
        led_buf[LED_UP] = 1;
    }
    
    if(DOWN_READ() == 1)
    {
        led_buf[LED_DOWN] = 0;
    }
    else
    {
        led_buf[LED_DOWN] = 1;
    }   
}

//����ʱ���ۼƺ�����1ms����һ��
void yxsj_handle(void)
{
    if((mod_buf[PS_FL_ERR] == 0) && (UP_READ() == 0) && (DOWN_READ() == 0))
    {
        time_b ++;
        
        if(time_b >= 60000)
        {
            time_b = 0;
            time_lj ++;
            mod_buf_write32(PS_LF_TIMEH, time_lj);
        }
    }
}






