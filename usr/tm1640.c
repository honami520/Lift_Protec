#include "tm1640.h"



extern uint8_t seg_dat[29];
extern uint8_t seg_buf[16];
extern uint8_t seg_back[2];



void udelay(uint16_t i)
{
    uint16_t j;

    for (; i > 0; i --)
    {
        j = 8;
        while(j > 0)
        {
            j --;
        }
    }
}

void mdelay(uint16_t i)
{
    uint16_t j;
    for(; i > 0; i --)
    {
        j = 8000;
        while(j > 0)
        {
            j --;
        }
    }
}


void tm1640_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF, ENABLE);

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_1;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOF, &GPIO_InitStruct);
}


void tm1640_start(void)
{
    SDA_H();
    SCL_H();
    udelay(1);
    SDA_L();
    udelay(1);
    SCL_L();    //À­µÍ
    udelay(1);
}

void tm1640_stop(void)
{
    SDA_L();
    SCL_H();
    udelay(1);
    SDA_H();
    udelay(1);   
    SCL_L();
    udelay(1);
}

void tm1640_write_byte(uint8_t dat)
{
    uint8_t i, temp;

    for(i = 0; i < 8; i ++)
    {
        temp = dat & 0x01;
        if(temp)
        {
            SDA_H();
        }
        else
        {
            SDA_L();
        }
        SCL_H();
        udelay(1);
        SCL_L();
        udelay(1);
        dat >>= 1;
    }   
        
    udelay(1);
}


void write_cmd(uint8_t cmd)
{
    tm1640_start();
    tm1640_write_byte(cmd);
    tm1640_stop();      
}

void write_dat(uint8_t cmd, uint8_t *buf)
{
   uint8_t i;
        
   tm1640_start();
   tm1640_write_byte(cmd);

   for (i = 0; i < 16; i ++)
   {
      tm1640_write_byte(*(buf + i));
   }

   tm1640_stop();
}


void sg_write(void)
{ 
   write_cmd(0x40);
   write_dat(0xc0, seg_buf);
   write_cmd(0x88);
}





