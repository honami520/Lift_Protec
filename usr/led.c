#include "led.h"
#include "tm1640.h"
#include "uart.h"


extern uint8_t led_buf[8];
extern uint8_t err_flag;
extern uint16_t mod_buf[MODBUS_SIZE];
extern uint8_t seg_back[2];
extern uint8_t seg_buf[16];
extern uint8_t seg_dat[29];

extern int16_t floor_now;


void led_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_1;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    led_clk_h();
    led_lch_h();
    led_sdi_h();

    led_send(0x00);
}

void led_send(uint32_t value)
{
    uint8_t i;
    
    led_clk_l();
    led_lch_l();

    value = ~value;
    
    for(i = 0 ; i < 8 ; i ++)
    {
        if ((value & 0x80) != 0)
        {
            led_sdi_h();
        }
        else
        {
            led_sdi_l();
        }
        value <<= 1;
        led_clk_l();
        led_clk_h();
    }
    
    led_lch_l();
    led_lch_h();
}





//100ms进入一次，故障后1秒钟更换一次故障状态
void seg_con_handle(void)
{
    uint8_t i, size = 0;
    static int8_t step = -1;
    static uint8_t num = 0;
    uint8_t n_max = 0;
    
    if(err_flag == 0)
    {
        if(floor_now >= 0)
        {
            //正面楼层
            seg_back[0] = floor_now / 10;
            seg_back[1] = floor_now % 10;
        }
        else
        {
            //下面楼层
            seg_back[0] = SEG_HANG;
            seg_back[1] = -floor_now;
        }
    }
    else
    {
        //故障状态，循环
        if((num == 0) || (num == 10))
        {
            //替换故障
            num = 1;

            //计算故障个数
            for(i = 0; i < 11; i ++)
            {
                if((mod_buf[PS_FL_ERR] & (1 << i)) != 0)
                {
                    size ++;
                    //保留最大值
                    n_max = i;
                }
            }

            if(size == 1)
            {
                //只有一个故障，则显示第一个故障
                for(i = 0; i < 11; i ++)
                {
                    if((mod_buf[PS_FL_ERR] & (1 << i)) != 0)
                    {
                        seg_back[0] = SEG_E;
                        seg_back[1] = i;
                        step = i;
                        break;
                    }
                }               
            }
            else
            {
                //显示比当前更高位的故障
                if(step == n_max)
                {
                    //已经是最高位，则寻找第一个故障
                    for(i = 0; i < 11; i ++)
                    {
                        if((mod_buf[PS_FL_ERR] & (1 << i)) != 0)
                        {
                            seg_back[0] = SEG_E;
                            seg_back[1] = i;
                            step = i;
                            break;
                        }
                    }                       
                }
                else
                {
                    //不是最高位，则寻找比当前大的第一个故障
                    for(i = 0; i < 11; i ++)
                    {
                        if(((mod_buf[PS_FL_ERR] & (1 << i)) != 0) && (step < i))
                        {
                            seg_back[0] = SEG_E;
                            seg_back[1] = i;
                            step = i;
                            break;
                        }
                    }
                }
            }

        }
        else
        {
            num ++;
        }
        
    }
}


void led_handle(void)
{
    uint8_t sd = 0;
    uint8_t i;

    for(i = 0; i < 8; i ++)
    {
        if(led_buf[i] == 1)
        {
            sd |= (1 << i);
        }
    }

    led_send(sd);
}



void seg_handle(void)
{
    seg_con_handle();

    if(err_flag == 0)
    {
        //无故障时候显示楼层
        seg_buf[0] = seg_dat[seg_back[0]];
        seg_buf[1] = seg_dat[seg_back[1]];

        //LED显示楼层
        led_buf[LED_FLOOR] = 1;
        led_buf[LED_ERR] = 0;
    }
    else if(err_flag == 1)
    {
        //有故障时显示错误代码
        seg_buf[0] = seg_dat[seg_back[0]];
        seg_buf[1] = seg_dat[seg_back[1]];

        //LED显示故障
        led_buf[LED_FLOOR] = 0;
        led_buf[LED_ERR] = 1;
    }

    sg_write();
}


