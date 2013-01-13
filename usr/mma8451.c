#include "mma8451.h"
#include <stdlib.h>
#include "uart.h"

extern uint16_t mod_buf[MODBUS_SIZE];
extern int8_t xmax, xmin;
extern int8_t ymax, ymin;
extern int8_t zmax, zmin;
extern uint8_t cal_flag;
extern uint8_t cal_step;
extern uint8_t x_s, y_s, z_s;


void delay_us(uint16_t i)
{
    for (; i > 0; i --);
}



void i2c_start(void)
{
    MMA_SCL_L();
    MMA_SDA_H();
    delay_us(10); 
    MMA_SCL_H();
    delay_us(10); 
    MMA_SDA_L();
    delay_us(10); 
    MMA_SCL_L();
    delay_us(10); 
}


void i2c_stop(void)
{
    MMA_SCL_L();
    delay_us(10); 
    MMA_SDA_L();
    delay_us(10);     
    MMA_SCL_H();
    delay_us(10);     
    MMA_SDA_H();
    delay_us(10); 
    MMA_SCL_L();
}



//主应答(包含ack:SDA=0和no_ack:SDA=0)
void i2c_ack_main(uint8_t ack_main)
{
    MMA_SCL_L();
    
    if(ack_main)
    {
        MMA_SDA_L(); //ack主应答
    }
    else 
    {
        MMA_SDA_H(); //no_ack无需应答
    }
    
    delay_us(10);     
    MMA_SCL_H();
    delay_us(10);     
    MMA_SCL_L();
}

//*************************************************
//字节发送程序
//发送c(可以是数据也可是地址)，送完后接收从应答
//不考虑从应答位
void send_byte(uint8_t c)
{
    uint8_t i;
    
    for(i = 0; i < 8; i ++)
    {
        MMA_SCL_L();
        
        if((c << i) & 0x80)
        {
            MMA_SDA_H(); //判断发送位
        }
        else 
        {
            MMA_SDA_L();
        }
        
        delay_us(10);     
        MMA_SCL_H();
        delay_us(10); 
        MMA_SCL_L();
    }
    
    delay_us(10);     
    MMA_SDA_H();            //发送完8bit，释放总线准备接收应答位
    delay_us(10);     
    MMA_SCL_H();
    delay_us(10);             //sda上数据即是从应答位
    MMA_SCL_L();            //不考虑从应答位|但要控制好时序
}

//**************************************************
//字节接收程序
//接收器件传来的数据，此程序应配合|主应答函数|IIC_ack_main()使用
//return: uchar型1字节
uint8_t read_byte(void)
{
    uint8_t i;
    uint8_t c;
    
    c = 0;
    MMA_SCL_L();
    delay_us(10); 
    MMA_SDA_H();                    //置数据线为输入方式
    MMA_SDA_IN();
    
    for(i = 0; i < 8; i ++)
    {
        delay_us(10); 
        MMA_SCL_L();                //置时钟线为低，准备接收数据位
        delay_us(10); 
        MMA_SCL_H();                //置时钟线为高，使数据线上数据有效
        delay_us(10); 
        c <<= 1;
        
        if(MMA_SDA_READ())
        {
            c += 1;             //读数据位，将接收的数据存c
        }
        
    }
    
    MMA_SCL_L();
    MMA_SDA_OUT();                  //SDA为输出
    
    return c;
}




//***************************************************
//向无子地址器件发送单字节数据
void send_to_byte(uint8_t ad_main, uint8_t c)
{
    i2c_start();
    send_byte(ad_main);     //发送器件地址
    send_byte(c);           //发送数据c
    i2c_stop();
}


//***************************************************
//从无子地址器件读单字节数据
//function:器件地址，所读数据存在接收缓冲区当前字节
void read_from_byte(uint8_t ad_main, uint8_t *buf)
{
    i2c_start();
    send_byte(ad_main); //发送器件地址
    *buf = read_byte();
    i2c_ack_main(NACK); //无需应答<no_ack=0>
    i2c_stop();
}


uint8_t MMA845x_readbyte(uint8_t address)
{
    uint8_t ret = 100;
    
    i2c_start();                            //启动
    send_byte(MMA845x_IIC_ADDRESS);         //写入设备ID及写信号
    send_byte(address);                     //X地址
    i2c_start();                            //重新发送开始
    send_byte(MMA845x_IIC_ADDRESS + 1);     //写入设备ID及读信
    ret = read_byte();                      //读取一字节
    i2c_stop();

    return ret;
}

//写入
void MMA845x_writebyte(uint8_t address, uint8_t thedata)
{
    i2c_start();                            //启动
    send_byte(MMA845x_IIC_ADDRESS);         //写入设备ID及写信号
    send_byte(address);                     //X地址
    send_byte(thedata);                     //写入设备ID及读信
    i2c_stop();
}

//初始化
//初始化为指定模式

void MMA485x_IO_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_1;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
}


void MMA845x_init(void)
{   
    uint8_t ver = 0;

    MMA485x_IO_init();
    MMA845x_writebyte(CTRL_REG1, ASLP_RATE_20MS + DATA_RATE_5MS);   
    delay_us(20);
    MMA845x_writebyte(XYZ_DATA_CFG_REG, FULL_SCALE_2G); //2G
    delay_us(20);
    MMA845x_writebyte(CTRL_REG1, ACTIVE_MASK);          //激活状态
    delay_us(20);

    ver = MMA845x_readbyte(WHO_AM_I_REG);

    if(ver == MMA8451Q_ID)
    {
        //
    }
}


void MMA845x_read(void)
{
    int8_t tmp = 0;
    
    if(cal_flag == 1)
    {
        //允许采集
        if(cal_step == 0)
        {
            cal_step = 1;
            tmp = MMA845x_readbyte(OUT_X_MSB_REG);

            if(x_s == 0)
            {
                //第一次采集
                x_s = 1;
                xmax = tmp;
                xmin = tmp;
            }

            if(tmp >= 0)
            {
                //临时值为正数时，和最大与最小值比较
                if(xmax >= 0)
                {
                    //正数时，最大值比较替换
                    if(tmp > xmax)
                    {
                        xmax = tmp;
                    }
                }
                else
                {
                    //负数时，直接替换
                    xmax = tmp;
                }

                if(xmin >= 0)
                {
                    //正数的时候，较小值比较替换
                    if(tmp < xmin)
                    {
                        xmin = tmp;
                    }
                }
                else
                {   
                    //负数的时候，直接放弃
                }
            }
            else
            {
                //临时值为负数时，进行最大值，最小值比较
                if(xmax >= 0)
                {
                    //为正数时直接放弃
                }
                else
                {
                    //为负数的时候，选择较大
                    if((-tmp) < (-xmax))
                    {
                        xmax = tmp;
                    }
                }

                if(xmin >= 0)
                {
                    //为正数，直接替换
                    xmin = tmp;
                }
                else
                {
                    //为负数时候，比较较小值
                    if((-tmp) > (-xmin))
                    {
                        xmin = tmp;
                    }
                }
            }
        }
        else if(cal_step == 1)
        {
            cal_step = 2;
            tmp = MMA845x_readbyte(OUT_Y_MSB_REG);

            if(y_s == 0)
            {
                //第一次采集
                y_s = 1;
                ymax = tmp;
                ymin = tmp;
            }
            
            if(tmp >= 0)
            {
                //临时值为正数时，和最大与最小值比较
                if(ymax >= 0)
                {
                    //正数时，最大值比较替换
                    if(tmp > ymax)
                    {
                        ymax = tmp;
                    }
                }
                else
                {
                    //负数时，直接替换
                    ymax = tmp;
                }
            
                if(ymin >= 0)
                {
                    //正数的时候，较小值比较替换
                    if(tmp < ymin)
                    {
                        ymin = tmp;
                    }
                }
                else
                {   
                    //负数的时候，直接放弃
                }
            }
            else
            {
                //临时值为负数时，进行最大值，最小值比较
                if(ymax >= 0)
                {
                    //为正数时直接放弃
                }
                else
                {
                    //为负数的时候，选择较大
                    if((-tmp) < (-ymax))
                    {
                        ymax = tmp;
                    }
                }
            
                if(ymin >= 0)
                {
                    //为正数，直接替换
                    ymin = tmp;
                }
                else
                {
                    //为负数时候，比较较小值
                    if((-tmp) > (-ymin))
                    {
                        ymin = tmp;
                    }
                }
            }
        }
        else if(cal_step == 2)
        {
            cal_step = 0;
            tmp = MMA845x_readbyte(OUT_Z_MSB_REG);

            if(z_s == 0)
            {
                //第一次采集
                z_s = 1;
                zmax = tmp;
                zmin = tmp;
            }
            
            if(tmp >= 0)
            {
                //临时值为正数时，和最大与最小值比较
                if(zmax >= 0)
                {
                    //正数时，最大值比较替换
                    if(tmp > zmax)
                    {
                        zmax = tmp;
                    }
                }
                else
                {
                    //负数时，直接替换
                    zmax = tmp;
                }
            
                if(zmin >= 0)
                {
                    //正数的时候，较小值比较替换
                    if(tmp < zmin)
                    {
                        zmin = tmp;
                    }
                }
                else
                {   
                    //负数的时候，直接放弃
                }
            }
            else
            {
                //临时值为负数时，进行最大值，最小值比较
                if(zmax >= 0)
                {
                    //为正数时直接放弃
                }
                else
                {
                    //为负数的时候，选择较大
                    if((-tmp) < (-zmax))
                    {
                        zmax = tmp;
                    }
                }
            
                if(zmin >= 0)
                {
                    //为正数，直接替换
                    zmin = tmp;
                }
                else
                {
                    //为负数时候，比较较小值
                    if((-tmp) > (-zmin))
                    {
                        zmin = tmp;
                    }
                }
            }
        }
    }
}


