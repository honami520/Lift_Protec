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
//故障判断
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
    //地址固定为0xfe，前面的保留用于扩展
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

    //PC13，PC14，PC15中断配置
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

//平层困人判断
//1秒钟判断一次
void err_pckr_handle(void)
{
    if((UP_READ() == 1) && (DOWN_READ() == 1) && (DOOR_READ() == 0))
    {
        //上平有，下平有，门关闭，查询T1时间内是否有3次人出现
        pckr_num ++;

        //判断人体红外出现次数
        if(pckr_step == 0)
        {
            if(HUM_READ() == 0)
            {
                //无人，则等待
            }
            else
            {
                //有人，则计数，等待无人
                pckr_r_num ++;
                pckr_step = 1;
            }
        }
        else
        {
            if(HUM_READ() == 0)
            {
                //变成无人，则进入再次判断有人
                pckr_step = 0;
            }
            else
            {
                //有人，忽略，等待
            }           
        }

        //判断时间是否达到
        if(pckr_num >= mod_buf_read8(PS_TM_PCKR))
        {
            //判断人体出现次数
            if(pckr_r_num > 3)
            {
                //出现平层困人
                mod_buf[PS_FL_ERR] |= B_PCKR;
            }
            else
            {
                //平层困人消除
                mod_buf[PS_FL_ERR] &= ~(B_PCKR);
            }

            //中间量消除，重新开始检测
            pckr_num = 0;
            pckr_r_num = 0;
            pckr_step = 0;
        }
        
    }
    else
    {
        //其他情况，则清除计数
        pckr_num = 0;
        pckr_r_num = 0;
        pckr_step = 0;
        mod_buf[PS_FL_ERR] &= ~(B_PCKR);
    }
}


//非平层困人判断
//1秒钟判断一次
void err_fpckr_handle(void)
{
    if((UP_READ() == 0) && (DOWN_READ() == 0))
    {
        //上平无，下平无，T3时间内是否有1次人出现
        fpckr_num ++;

        //判断人体红外出现次数
        if(fpckr_step == 0)
        {
            if(HUM_READ() == 0)
            {
                //无人，则等待
            }
            else
            {
                //有人，则计数，等待无人
                fpckr_r_num ++;
                fpckr_step = 1;
            }
        }
        else
        {
            if(HUM_READ() == 0)
            {
                //变成无人，则进入再次判断有人
                fpckr_step = 0;
            }
            else
            {
                //有人，忽略，等待
            }           
        }

        //判断时间是否达到
        if(fpckr_num >= mod_buf_read8(PS_TM_FPCKR))
        {
            //判断人体出现次数
            if(fpckr_r_num > 1)
            {
                //出现非平层困人
                mod_buf[PS_FL_ERR] |= B_FPCKR;
            }
            else
            {
                //非平层困人消除
                mod_buf[PS_FL_ERR] &= ~(B_FPCKR);
            }

            //中间量消除，重新开始检测
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




//门区外停梯检测
//100毫秒检测1次
void err_mqwtt_handle(void)
{
    if((UP_READ() == 0) && (DOWN_READ() == 0) && (DOOR_READ() == 0)) 
    {
        //上平无，下平无，门关闭
        mqwtt_num ++;

        //判断时间是否达到
        if(mqwtt_num >= (10 * mod_buf_read8(PS_TM_MQWTT)))
        {
            //出现门区外停梯
            mod_buf[PS_FL_ERR] |= B_MQWTT;
            
            //中间量消除，重新开始检测
            mqwtt_num = 0;
        }       
    }
    else
    {
        mqwtt_num = 0;
        //清除门区外停梯
        mod_buf[PS_FL_ERR] &= ~(B_MQWTT);
    }
}




//门区外开门
//1秒钟检测1次
void err_mqwkm_handle(void)
{
    if((UP_READ() == 0) && (DOWN_READ() == 0) && (DOOR_READ() == 1)) 
    {
        //上平无，下平无，门打开
        mqwkm_num ++;

        //判断时间是否达到
        if(mqwkm_num >= 2)
        {
            //出现门区外开门
            mod_buf[PS_FL_ERR] |= B_MQWKM;
            
            //中间量消除，重新开始检测
            mqwkm_num = 0;
        }       
    }
    else
    {
        mqwkm_num = 0;
        //清除门区外开门
        mod_buf[PS_FL_ERR] &= ~(B_MQWKM);
    }   
}




//长时间开门
//1秒钟检测1次
void err_csjkm_handle(void)
{
    if(DOOR_READ() == 1)
    {
        //门打开
        csjkm_num ++;

        if(csjkm_num >= (mod_buf_read8(PS_TM_CSJKM) * 60))
        {
            //长时间开门
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



//反复开关门
//1秒钟检测1次
void err_ffkgm_handle(void)
{
    if((UP_READ() == 1) && (DOWN_READ() == 1))
    {
        //一次停梯开关门次数
        if(ffkgm_step == 0)
        {
            if(DOOR_READ() == 0)
            {
                //门关闭，等待门打开
                ffkgm_step = 1;
            }
            else
            {
                //门打开，等待门关闭
                ffkgm_step = 2;
            }
        }
        else if(ffkgm_step == 1)
        {
            //等待门打开
            if(DOOR_READ() == 1)
            {
                //门已经打开，等待门关闭
                ffkgm_num ++;
                ffkgm_step = 2;
            }
        }
        else if(ffkgm_step == 2)
        {
            //等待门关闭
            if(DOOR_READ() == 0)
            {
                //门已经关闭，等待门打开
                ffkgm_num ++;
                ffkgm_step = 1;
            }
        }

        //判断开关门次数是否超标
        if(ffkgm_num >= mod_buf_read8(PS_NM_FFKGM))
        {
            //反复开关门成立
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




//开门走梯
//10ms检测一次
void err_kmzt_handle(void)
{
    if((DOOR_READ() == 1) && (((UP_READ() == 1) && (DOWN_READ() == 0)) || ((UP_READ() == 0) && (DOWN_READ() == 1))))
    {
        //开门走梯
        kmzt_num ++;

        if(kmzt_num >= 2)
        {
            //开门走梯出现
            mod_buf[PS_FL_ERR] |= B_KMZT;
            kmzt_num = 0;
        }
    }
    else if((DOOR_READ() == 0) && (((UP_READ() == 1) && (DOWN_READ() == 0)) || ((UP_READ() == 0) && (DOWN_READ() == 1))))
    {
        //关门走梯，清除
        mod_buf[PS_FL_ERR] &= ~(B_KMZT);
        kmzt_num = 0;
    }
    else
    {
        kmzt_num = 0;
    }
}


//超速计算
//10ms检测一次
void err_cs_handle(void)
{
    uint16_t tmp = 0;
    uint16_t tmp1 = 0;
    
    if(cs_step == 0)
    {
        //最初的时候，等待到达某一层
        if((UP_READ() == 1) && (DOWN_READ() == 1))
        {
            //上平有，下平有
            cs_step = 1;
        }       
    }
    else if(cs_step == 1)
    {
        //等待移出当前楼层
        if((UP_READ() == 0) && (DOWN_READ() == 0))
        {
            //移出某楼层，时间清零
            cs_step = 2;
            cs_dat = 0;
        }
    }
    else if(cs_step == 2)
    {
        //等待到某个楼层
        //累计时间增加
        cs_dat ++;
        
        if((UP_READ() == 1) && (DOWN_READ() == 1))
        {
            //计算是否超速
            tmp = mod_buf_read8(PS_NM_LCPJJJ);
            tmp1 = mod_buf_read8(PS_TM_CS);
            
            if(((10 * tmp) / cs_dat) > tmp1)
            {
                //超速运行
                mod_buf[PS_FL_ERR] |= B_CS;
            }
            else
            {
                //非超速运行，清除
                mod_buf[PS_FL_ERR] &= ~(B_CS);
            }

            //清除，等待下一次检测
            cs_step = 0;
            cs_dat = 0;
        }
    }

}



//运行超时检测
//10ms检测一次
void err_yxcs_handle(void)
{
    uint16_t tmp = 0;
    
    if(yxcs_step == 0)
    {
        //最初的时候，等待到达某一层
        if((UP_READ() == 1) && (DOWN_READ() == 1))
        {
            //上平有，下平有
            yxcs_step = 1;
        }       
    }
    else if(yxcs_step == 1)
    {
        //等待移出当前楼层
        if((UP_READ() == 0) && (DOWN_READ() == 0))
        {
            //取得移出某楼层
            yxcs_step = 2;

            //加速度计量开始
            if(cal_flag == 0)
            {
                //开启加速度计量，直到停梯
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
        
        //计算运行时间是否超时
        if((mod_buf[PS_FL_ERR] & (B_MQWTT | B_MQWKM | B_FPCKR)) != 0)
        {
            //门区外开门，门区外停梯，非平层困人时，清除
            yxcs_num = 0;
            yxcs_num1 = 0;
            yxcs_step = 0;

            mod_buf[PS_FL_ERR] &= ~(B_YXCS);
        }

        //检测是否停梯
        if((UP_READ() == 1) && (DOWN_READ() == 1))
        {
            //停梯达到足够时间，清除
            yxcs_num1 ++;

            if(yxcs_num1 >= 200)
            {
                //停梯有效，清除
                yxcs_num1 = 0;
                yxcs_num = 0;
                yxcs_step = 0;

                mod_buf[PS_FL_ERR] &= ~(B_YXCS);

                //平层
                mod_buf[PS_FL_STATE / 2] |= B_DT_PING;
                mod_buf[PS_FL_STATE / 2] &= ~(B_DT_DOWN);
                mod_buf[PS_FL_STATE / 2] &= ~(B_DT_UP);


                //加速度结束或者开启
                if(cal_flag == 1)
                {
                    //计算结果放入modbus
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

                    //计算加速度10倍
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

                    //计算加速度10倍
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

                    //计算加速度10倍
                    tmp = (tmp * 20) / 128;
                    mod_buf_write8(PS_NM_Z, tmp);

                    //变量清零
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
            //门打开，则清除
            yxcs_num = 0;
            yxcs_num1 = 0;
            yxcs_step = 0;
        }
        
        if((yxcs_num / 6000) >= mod_buf_read8(PS_TM_YXCS))
        {
            //运行超时有效
            mod_buf[PS_FL_ERR] |= B_YXCS;
            yxcs_num = 0;
            yxcs_num1 = 0;
            yxcs_step = 0;
        }
    }

}




//1ms判断一次
void err_cd_dd_handle(void)
{
    static uint16_t tmp = 0;
    
    if(floor_base_flag == 1)
    {
        //已知基础楼层下，判断冲顶
        if(floor_step == 0)
        {
            if((UP_READ() == 1) && (DOWN_READ() == 1))
            {
                //平层开始，则执行判断1
                floor_step = 1;

                //平层后，清除冲顶
                mod_buf[PS_FL_ERR] &= ~B_CD;
                mod_buf[PS_FL_ERR] &= ~B_DD;
            }
        }
        else if(floor_step == 1)
        {
            //非平层运行判断
            if((UP_READ() == 0) && (DOWN_READ() == 1))
            {
                //朝上运行
                floor_step = 2;

                mod_buf[PS_FL_STATE / 2] |= B_DT_UP;
                mod_buf[PS_FL_STATE / 2] &= ~(B_DT_DOWN);
                mod_buf[PS_FL_STATE / 2] &= ~(B_DT_PING);

                //LED
                led_buf[LED_BASE] = 0;


                //冲顶判断
                if(floor_now > 0)
                {
                    if(floor_now == mod_buf_read8(PS_FL_UP))
                    {
                        //冲顶
                        mod_buf[PS_FL_ERR] |= B_CD;
                    }               
                }
            }
            else if((UP_READ() == 1) && (DOWN_READ() == 0))
            {
                //朝下运行
                floor_step = 3;

                mod_buf[PS_FL_STATE / 2] |= B_DT_DOWN;
                mod_buf[PS_FL_STATE / 2] &= ~(B_DT_UP);
                mod_buf[PS_FL_STATE / 2] &= ~(B_DT_PING);

                //LED
                led_buf[LED_BASE] = 0;


                //蹲低判断
                if(floor_now < 0)
                {
                    if((-floor_now) == mod_buf_read8(PS_FL_DOWN))
                    {
                        //蹲底
                        mod_buf[PS_FL_ERR] |= B_DD;
                    }
                }
            }
        }
        else if(floor_step == 2)
        {
            //等待移出楼层
            if((UP_READ() == 0) && (DOWN_READ() == 0))
            {
                //进入等待上先平有，下平无
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
            //等待移出楼层
            if((UP_READ() == 0) && (DOWN_READ() == 0))
            {
                //进入等待上先平有，下平无
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
            //等待上平有，下平无出现，否则退出
            if((UP_READ() == 1) && (DOWN_READ() == 0))
            {
                //楼层等待增加
                floor_step = 5;
            }
            else if((UP_READ() == 0) && (DOWN_READ() == 1))
            {
                floor_step = 0;
            }
        }
        else if(floor_step == 5)
        {
            //等待平层出现
            if((UP_READ() == 1) && (DOWN_READ() == 1))
            {
                //平层，楼层增加
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

                //清除冲顶
                mod_buf[PS_FL_ERR] &= ~B_CD;
            }
            else if((UP_READ() == 0) && (DOWN_READ() == 0))
            {
                floor_step = 0;
            }
        }
        else if(floor_step == 6)
        {
            //等待上平无，下平有出现，否则退出
            if((UP_READ() == 0) && (DOWN_READ() == 1))
            {
                //楼层等待减少
                floor_step = 7;
            }
            else if((UP_READ() == 1) && (DOWN_READ() == 0))
            {
                floor_step = 0;
            }
        }
        else if(floor_step == 7)
        {
            //等待平层
            if((UP_READ() == 1) && (DOWN_READ() == 1))
            {
                //平层，楼层减小
                if(floor_now > 0)
                {
                    floor_now --;
                }
                else if(floor_now < 0)
                {
                    //负层
                    if((-floor_now) < mod_buf_read8(PS_FL_DOWN))
                    {
                        floor_now --;
                    }
                }

                if(floor_now == 0)
                {
                    //忽略0层
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

                //清除蹲低
                mod_buf[PS_FL_ERR] &= ~B_DD;
            }  
            else if((UP_READ() == 0) && (DOWN_READ() == 0))
            {
                floor_step = 0;
            }
        }
    }
}


//基层判断
//1ms进行一次
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
        //已经检测出基层后，显示基层状态灯亮灭
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



//门开关和红外状态检测
//100ms进行1次
void hum_door_check_handle(void)
{
    if(DOOR_READ() == 1)
    {
        //门打开
        mod_buf[PS_FL_STATE / 2] |= B_DOOR;
        //门LED亮
        led_buf[LED_DOLL] = 1;
    }
    else
    {
        //门关上
        mod_buf[PS_FL_STATE / 2] &= ~(B_DOOR);
        //门LED灭
        led_buf[LED_DOLL] = 0;
    }

    if(HUM_READ() == 1)
    {
        //有人
        mod_buf[PS_FL_STATE / 2] |= B_HUM;
        //人体LED亮
        led_buf[LED_HUM] = 1;
    }
    else
    {
        mod_buf[PS_FL_STATE / 2] &= ~(B_HUM);
        //人体LED灭
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



//100ms执行一次，记录门打开次数
void door_num_handle(void)
{
    if(door_flag == 0)
    {
        //等待门关闭
        if(DOOR_READ() == 0)
        {
            door_num ++;

            if(door_num > 10)
            {
                //等待门打开
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
        //等待门打开
        if(DOOR_READ() == 1)
        {
            door_num ++;

            if(door_num > 10)
            {
                door_num = 0;
                door_flag = 0;

                //门已经打开
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

//上下光电对应LED状态
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

//运行时间累计函数，1ms进入一次
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






