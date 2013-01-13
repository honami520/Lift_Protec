#include "uart.h"
#include "crc16.h"
#include "flash_eep.h"


extern uint8_t mb_send_flag;
extern uint8_t mb_send_num;
extern uint8_t mb_send_size;


extern uint16_t mod_buf[MODBUS_SIZE];
extern uint8_t send_buf[50];
extern uint8_t send_num;
extern uint8_t rec_buf[10];
extern uint8_t rec_num;
extern uint8_t mb_step;
extern uint8_t *buf_head;

extern uint8_t send_flag;       //���ͱ�־
extern uint8_t clear_flag;
extern uint16_t clear_num;

extern uint8_t dev_addr;

extern uint32_t time_lj;
extern uint32_t num_b;


//�����������
//�����յ����ҵ����ݣ������յ���������ָ�
//�������������Ӱ���´ν���
void uart_clear(void)
{
    uint8_t i;

    if (clear_flag == 1)
    {
        //������ʱ����
        clear_num++;

        if (clear_num >= 100)
        {
            //��ʱ����ִ���������
            clear_num = 0;
            clear_flag = 0;

            rec_num = 0;
            mb_step = 0;

            //������ڽ���
            for (i = 0; i < 10; i++)
            {
                rec_buf[i] = 0;
            }
        }
    }
}


void uart_init(void)
{
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    //����GPIOA,UART1ʱ��
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_DMA1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_1);

    /* Configure USART Tx and Rx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* USART1 TX DMA1 Channel (triggered by USART1 Tx event) Config */
    DMA_DeInit(DMA1_Channel2);
    DMA_InitStructure.DMA_PeripheralBaseAddr = 0x40013828;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)send_buf;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = 8;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel2, &DMA_InitStructure);
    DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);

    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART1, &USART_InitStructure);
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
    USART_Cmd(USART1, ENABLE);

    DMA_ClearITPendingBit(DMA1_IT_TC2);

    //ʹ��TX DMA�жϣ����ȼ�Ϊ1
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel2_3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}


void buf_add(uint8_t val)
{
    *buf_head = val;
    buf_head ++;
    send_num++;
}


void modbus_rec(uint8_t val)
{
    uint16_t add = 0, num = 0;
    uint8_t i;
    uint16_t *pt = mod_buf;
    
    rec_buf[rec_num] = val;
    clear_flag = 1;
    clear_num = 0;
    
    if (mb_step== 0)
    {
        if (rec_num == 0)
        {
            //�жϵ�ַ�Ƿ�һ�£����������
            if (rec_buf[rec_num] == dev_addr)
            {
                //��ַһ�£������
                rec_num ++;
            }
            else
            {
                //��ַ��һ�£������
                rec_buf[0] = 0;
            }
        }
        else if (rec_num == 1)
        {
            //�������ж�
            if (rec_buf[1] == 0x03)
            {
                //������
                mb_step = 1;
                rec_num ++;
            }
            else if (rec_buf[1] == 0x06)
            {
                //д����
                mb_step = 2;
                rec_num ++;
            }
            else
            {
                //��֧�ֵ����������
                rec_num = 0;
                rec_buf[0] = 0;
                rec_buf[1] = 0;
            }
        }       
    }
    else if(mb_step == 1)
    {
        rec_num ++;
        
        if (rec_num == 8)
        {
            //�õ�8�����ݺ���
            add = (rec_buf[2] << 8) + rec_buf[3];
            num = (rec_buf[4] << 8) + rec_buf[5];
            
            if ((add >= 0x10) && (add < (0x10 + MODBUS_SIZE)) && (num < (MODBUS_SIZE + 1)) && (num >= 1))
            {
                //��ȥƫ����
                add -= 0x10;
                
                if (num > ((MODBUS_SIZE + 1) - add))
                {
                    //��֤����ȡ�������Чλ
                    num = (MODBUS_SIZE + 1) - add;
                }
                
                //�ƶ���ʼλ��
                pt += add;
                
                buf_add(dev_addr);
                buf_add(0x03);
                buf_add(2 * num);
                
                for (i = 0; i < num; i ++)
                {
                    //��ȡnum * 2�����ݼ��뷢�ͻ���
                    buf_add((uint8_t)(*(pt + i) >> 8));     //���ֽ���ǰ
                    buf_add((uint8_t)(*(pt + i) & 0xff));   //���ֽ��ں�
                }
                
                send_flag = 1;
            }
            
            for (num = 0; num < 10; num ++)
            {
                rec_buf[num] = 0;
            }
            
            num = 0;
            add = 0;
            rec_num= 0;
            mb_step = 0;
        }               
    }
    else if(mb_step == 2)
    {
        rec_num ++;

        if(rec_num == 8)
        {
            add = (rec_buf[2] << 8) + rec_buf[3];       
            num = (rec_buf[4] << 8) + rec_buf[5];       //�������

            if((add < (0x10 + MODBUS_SIZE)) && (add >= 0x10)) 
            {
                //��ȥƫ����
                add -= 0x10;

                switch(add)
                {
                    case 0:
                        //���ݹ��ϣ�ֻ����������
                        break;
                
                    case 1:
                        //����״̬�����������ϣ�ֻ����������
                        break;
                
                    case 2:
                        //¥�㣬�����ٶȣ�ֻ����������
                        break;
                
                    case 3:
                        //�����ۼƿ��Ŵ�����λ����д������
                        if(mod_buf_read16(PS_LF_TIMEH) != num)
                        {
                            //���ݸı䣬��д��
                            mod_buf_write16(PS_LF_TIMEH, num);
                            time_lj = mod_buf_read32(PS_LF_TIMEH);
                    
                            //����д��EEPROM
                            eep_write32(REG_TIME, time_lj);                     
                        }

                        break;
                
                    case 4:
                        //�����ۼƿ��Ŵ�����λ����д������
                        if(mod_buf_read16(PS_LF_TIMEL) != num)
                        {
                            //�����иı䣬��д��
                            mod_buf_write16(PS_LF_TIMEL, num);
                            time_lj = mod_buf_read32(PS_LF_TIMEH);
                    
                            //����д��EEPROM
                            eep_write32(REG_TIME, time_lj);                     
                        }       
                        break;
                
                    case 5:
                        //�����ۼ����в�����λ����д������
                        if(mod_buf_read16(PS_LF_NUMH) != num)
                        {
                            //�����иı䣬��д��
                            mod_buf_write16(PS_LF_NUMH, num);
                            num_b = mod_buf_read32(PS_LF_NUMH);
                    
                            //����д��EEPROM
                            eep_write32(REG_NUM, num_b);                            
                        }               
                        break;
                
                    case 6:
                        //�����ۼ����в�����λ����д������
                        if(mod_buf_read16(PS_LF_NUML) != num)
                        {
                            //�����иı䣬��д��
                            mod_buf_write16(PS_LF_NUML, num);
                            num_b = mod_buf_read32(PS_LF_NUMH);
                    
                            //����д��EEPROM
                            eep_write32(REG_NUM, num_b);                            
                        }               
                        break;      

                    case 7:
                        break;

                    case 8:
                        break;

                    case 9:
                        break;

                    case 10:
                        break;

                    case 11:
                        break;

                    case 12:
                        break;
                
                    case 13:
                        //����¥�����͵���¥�������ɶ�д������
                        if(mod_buf_read16(PS_FL_UP) != num)
                        {
                            //�����иı䣬��д��
                            mod_buf_write16(PS_FL_UP, num);
                    
                            //������㡢���²�д��EEPROM
                            eep_write16(REG_UP, mod_buf_read8(PS_FL_UP));
                            eep_write16(REG_DOWN, mod_buf_read8(PS_FL_DOWN));                           
                        }
                        break;

                    case 14:
                        //��վ�㣬¥��ƽ�����
                        if(mod_buf_read16(PS_FL_BASE) != num)
                        {
                            //�����иı䣬��д��
                            mod_buf_write16(PS_FL_BASE, num);

                            //����վ�㣬¥��ƽ�����д��EEPROM
                            eep_write16(REG_BASE, mod_buf_read8(PS_FL_BASE));
                            eep_write16(REG_LCPJJJ, mod_buf_read8(PS_NM_LCPJJJ));
                        }
                        break;
                
                    case 15:
                        //���٣��ɶ�д������
                        if(mod_buf_read16(PS_TM_CS) != num)
                        {
                            //�����иı䣬��д��
                            mod_buf_write16(PS_TM_CS, num);
                    
                            //������д��EEPROM
                            eep_write16(REG_CS, mod_buf_read8(PS_TM_CS));                       
                        }       
                        break;
                
                    case 16:
                        //ƽ�����ˣ���ƽ������ʱ��
                        if(mod_buf_read16(PS_TM_PCKR) != num)
                        {
                            //�����иı䣬��д��
                            mod_buf_write16(PS_TM_PCKR, num);
                    
                            //��ֵд��EEPROM
                            eep_write16(REG_PCKR, mod_buf_read8(PS_TM_PCKR));
                            eep_write16(REG_FPCKR, mod_buf_read8(PS_TM_FPCKR));                         
                        }           
                        break;
                
                    case 17:
                        //������ͣ�ݡ����г�ʱ����д������
                        if(mod_buf_read16(PS_TM_MQWTT) != num)
                        {
                            //�����иı䣬��д��
                            mod_buf_write16(PS_TM_MQWTT, num);
                    
                            //��ֵд��EEPROM
                            eep_write16(REG_MQWTT, mod_buf_read8(PS_TM_MQWTT));
                            eep_write16(REG_YXCS, mod_buf_read8(PS_TM_YXCS));                       
                        }           
                        break;
                
                    case 18:
                        //��ʱ�俪�š����������ţ���д������
                        if(mod_buf_read16(PS_TM_CSJKM) != num)
                        {
                            //�����иı䣬��д��
                            mod_buf_write16(PS_TM_CSJKM, num);
                    
                            //��ֵд��EEPROM
                            eep_write16(REG_CSJKM, mod_buf_read8(PS_TM_CSJKM));
                            eep_write16(REG_FFKGM, mod_buf_read8(PS_NM_FFKGM));
                        }       
                        break;
                        
                }

                
                //����Ӧ��
                buf_add(dev_addr);
                buf_add(0x06);
                buf_add(rec_buf[2]);
                buf_add(rec_buf[3]);
                buf_add(rec_buf[4]);
                buf_add(rec_buf[5]);
                
                send_flag = 1;              
            }
            
            for (num = 0; num < 10; num ++)
            {
                rec_buf[num] = 0;
            }
            
            num = 0;
            add = 0;
            rec_num= 0;
            mb_step = 0;
        }
    }

}




void modbus_send(void)
{
    uint16_t crc = 0;

    if(send_flag == 1)
    {
        send_flag = 0;
        
        crc = crc16(0xffff, send_buf, send_num);
        buf_add((uint8_t)(crc & 0xff));
        buf_add((uint8_t)(crc >> 8));

        buf_head = send_buf;

        DMA_Cmd(DMA1_Channel2, DISABLE);
        DMA_SetCurrDataCounter(DMA1_Channel2, send_num);
        DMA_Cmd(DMA1_Channel2, ENABLE);

        buf_head = send_buf;
        send_num = 0;
    }
}





//��MOD_BUFд��һ���ֽڣ��Զ����ָߵ��ֽ�
void mod_buf_write8(uint8_t reg, uint8_t dat)
{
    uint8_t cs = 0, ys = 0;

    cs = reg / 2;
    ys = reg % 2;

    if(ys == 0)
    {
        //���ֽ�
        mod_buf[cs] &= 0x00ff;
        mod_buf[cs] |= (uint16_t)(dat << 8);
    }
    else
    {
        //���ֽ�
        mod_buf[cs] &= 0xff00;
        mod_buf[cs] |= dat;
    }
}


//��mod_bufд��һ��16λ��
void mod_buf_write16(uint8_t reg, uint16_t dat)
{
    mod_buf[reg / 2] = dat;
}


void mod_buf_write32(uint8_t reg, uint32_t dat)
{
    mod_buf[reg / 2] = (uint16_t)(dat >> 16);
    mod_buf[(reg / 2) + 1] = (uint16_t)(dat & 0xffff);
}


uint8_t mod_buf_read8(uint8_t reg)
{
    uint8_t cs = 0, ys = 0;
    uint8_t rt = 0;

    cs = reg / 2;
    ys = reg % 2;

    if(ys == 0)
    {
        //���ֽ�
        rt = (uint8_t)(mod_buf[cs] >> 8);
    }
    else
    {
        //���ֽ�
        rt = (uint8_t)(mod_buf[cs] & 0x00ff);
        
    }

    return rt;
}




uint16_t mod_buf_read16(uint8_t reg)
{
    return mod_buf[reg / 2];
}


uint32_t mod_buf_read32(uint8_t reg)
{
    uint32_t rt = 0;

    rt = mod_buf[reg / 2];
    rt <<= 16;
    rt += mod_buf[(reg / 2) + 1];

    return rt;
}














