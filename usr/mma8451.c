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



//��Ӧ��(����ack:SDA=0��no_ack:SDA=0)
void i2c_ack_main(uint8_t ack_main)
{
    MMA_SCL_L();
    
    if(ack_main)
    {
        MMA_SDA_L(); //ack��Ӧ��
    }
    else 
    {
        MMA_SDA_H(); //no_ack����Ӧ��
    }
    
    delay_us(10);     
    MMA_SCL_H();
    delay_us(10);     
    MMA_SCL_L();
}

//*************************************************
//�ֽڷ��ͳ���
//����c(����������Ҳ���ǵ�ַ)���������մ�Ӧ��
//�����Ǵ�Ӧ��λ
void send_byte(uint8_t c)
{
    uint8_t i;
    
    for(i = 0; i < 8; i ++)
    {
        MMA_SCL_L();
        
        if((c << i) & 0x80)
        {
            MMA_SDA_H(); //�жϷ���λ
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
    MMA_SDA_H();            //������8bit���ͷ�����׼������Ӧ��λ
    delay_us(10);     
    MMA_SCL_H();
    delay_us(10);             //sda�����ݼ��Ǵ�Ӧ��λ
    MMA_SCL_L();            //�����Ǵ�Ӧ��λ|��Ҫ���ƺ�ʱ��
}

//**************************************************
//�ֽڽ��ճ���
//�����������������ݣ��˳���Ӧ���|��Ӧ����|IIC_ack_main()ʹ��
//return: uchar��1�ֽ�
uint8_t read_byte(void)
{
    uint8_t i;
    uint8_t c;
    
    c = 0;
    MMA_SCL_L();
    delay_us(10); 
    MMA_SDA_H();                    //��������Ϊ���뷽ʽ
    MMA_SDA_IN();
    
    for(i = 0; i < 8; i ++)
    {
        delay_us(10); 
        MMA_SCL_L();                //��ʱ����Ϊ�ͣ�׼����������λ
        delay_us(10); 
        MMA_SCL_H();                //��ʱ����Ϊ�ߣ�ʹ��������������Ч
        delay_us(10); 
        c <<= 1;
        
        if(MMA_SDA_READ())
        {
            c += 1;             //������λ�������յ����ݴ�c
        }
        
    }
    
    MMA_SCL_L();
    MMA_SDA_OUT();                  //SDAΪ���
    
    return c;
}




//***************************************************
//�����ӵ�ַ�������͵��ֽ�����
void send_to_byte(uint8_t ad_main, uint8_t c)
{
    i2c_start();
    send_byte(ad_main);     //����������ַ
    send_byte(c);           //��������c
    i2c_stop();
}


//***************************************************
//�����ӵ�ַ���������ֽ�����
//function:������ַ���������ݴ��ڽ��ջ�������ǰ�ֽ�
void read_from_byte(uint8_t ad_main, uint8_t *buf)
{
    i2c_start();
    send_byte(ad_main); //����������ַ
    *buf = read_byte();
    i2c_ack_main(NACK); //����Ӧ��<no_ack=0>
    i2c_stop();
}


uint8_t MMA845x_readbyte(uint8_t address)
{
    uint8_t ret = 100;
    
    i2c_start();                            //����
    send_byte(MMA845x_IIC_ADDRESS);         //д���豸ID��д�ź�
    send_byte(address);                     //X��ַ
    i2c_start();                            //���·��Ϳ�ʼ
    send_byte(MMA845x_IIC_ADDRESS + 1);     //д���豸ID������
    ret = read_byte();                      //��ȡһ�ֽ�
    i2c_stop();

    return ret;
}

//д��
void MMA845x_writebyte(uint8_t address, uint8_t thedata)
{
    i2c_start();                            //����
    send_byte(MMA845x_IIC_ADDRESS);         //д���豸ID��д�ź�
    send_byte(address);                     //X��ַ
    send_byte(thedata);                     //д���豸ID������
    i2c_stop();
}

//��ʼ��
//��ʼ��Ϊָ��ģʽ

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
    MMA845x_writebyte(CTRL_REG1, ACTIVE_MASK);          //����״̬
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
        //����ɼ�
        if(cal_step == 0)
        {
            cal_step = 1;
            tmp = MMA845x_readbyte(OUT_X_MSB_REG);

            if(x_s == 0)
            {
                //��һ�βɼ�
                x_s = 1;
                xmax = tmp;
                xmin = tmp;
            }

            if(tmp >= 0)
            {
                //��ʱֵΪ����ʱ�����������Сֵ�Ƚ�
                if(xmax >= 0)
                {
                    //����ʱ�����ֵ�Ƚ��滻
                    if(tmp > xmax)
                    {
                        xmax = tmp;
                    }
                }
                else
                {
                    //����ʱ��ֱ���滻
                    xmax = tmp;
                }

                if(xmin >= 0)
                {
                    //������ʱ�򣬽�Сֵ�Ƚ��滻
                    if(tmp < xmin)
                    {
                        xmin = tmp;
                    }
                }
                else
                {   
                    //������ʱ��ֱ�ӷ���
                }
            }
            else
            {
                //��ʱֵΪ����ʱ���������ֵ����Сֵ�Ƚ�
                if(xmax >= 0)
                {
                    //Ϊ����ʱֱ�ӷ���
                }
                else
                {
                    //Ϊ������ʱ��ѡ��ϴ�
                    if((-tmp) < (-xmax))
                    {
                        xmax = tmp;
                    }
                }

                if(xmin >= 0)
                {
                    //Ϊ������ֱ���滻
                    xmin = tmp;
                }
                else
                {
                    //Ϊ����ʱ�򣬱ȽϽ�Сֵ
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
                //��һ�βɼ�
                y_s = 1;
                ymax = tmp;
                ymin = tmp;
            }
            
            if(tmp >= 0)
            {
                //��ʱֵΪ����ʱ�����������Сֵ�Ƚ�
                if(ymax >= 0)
                {
                    //����ʱ�����ֵ�Ƚ��滻
                    if(tmp > ymax)
                    {
                        ymax = tmp;
                    }
                }
                else
                {
                    //����ʱ��ֱ���滻
                    ymax = tmp;
                }
            
                if(ymin >= 0)
                {
                    //������ʱ�򣬽�Сֵ�Ƚ��滻
                    if(tmp < ymin)
                    {
                        ymin = tmp;
                    }
                }
                else
                {   
                    //������ʱ��ֱ�ӷ���
                }
            }
            else
            {
                //��ʱֵΪ����ʱ���������ֵ����Сֵ�Ƚ�
                if(ymax >= 0)
                {
                    //Ϊ����ʱֱ�ӷ���
                }
                else
                {
                    //Ϊ������ʱ��ѡ��ϴ�
                    if((-tmp) < (-ymax))
                    {
                        ymax = tmp;
                    }
                }
            
                if(ymin >= 0)
                {
                    //Ϊ������ֱ���滻
                    ymin = tmp;
                }
                else
                {
                    //Ϊ����ʱ�򣬱ȽϽ�Сֵ
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
                //��һ�βɼ�
                z_s = 1;
                zmax = tmp;
                zmin = tmp;
            }
            
            if(tmp >= 0)
            {
                //��ʱֵΪ����ʱ�����������Сֵ�Ƚ�
                if(zmax >= 0)
                {
                    //����ʱ�����ֵ�Ƚ��滻
                    if(tmp > zmax)
                    {
                        zmax = tmp;
                    }
                }
                else
                {
                    //����ʱ��ֱ���滻
                    zmax = tmp;
                }
            
                if(zmin >= 0)
                {
                    //������ʱ�򣬽�Сֵ�Ƚ��滻
                    if(tmp < zmin)
                    {
                        zmin = tmp;
                    }
                }
                else
                {   
                    //������ʱ��ֱ�ӷ���
                }
            }
            else
            {
                //��ʱֵΪ����ʱ���������ֵ����Сֵ�Ƚ�
                if(zmax >= 0)
                {
                    //Ϊ����ʱֱ�ӷ���
                }
                else
                {
                    //Ϊ������ʱ��ѡ��ϴ�
                    if((-tmp) < (-zmax))
                    {
                        zmax = tmp;
                    }
                }
            
                if(zmin >= 0)
                {
                    //Ϊ������ֱ���滻
                    zmin = tmp;
                }
                else
                {
                    //Ϊ����ʱ�򣬱ȽϽ�Сֵ
                    if((-tmp) > (-zmin))
                    {
                        zmin = tmp;
                    }
                }
            }
        }
    }
}


