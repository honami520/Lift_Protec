#ifndef _UART_H__
#define _UART_H__
#include "stm32f0xx.h"


#define MODBUS_SIZE     19

/***********************************
0:  ���ݹ���
0:  ���ݹ���
1:  ����״̬
1:  ����������
2:  ¥��
2:  �����ٶ�*10
3:  �����ۼƿ��Ŵ���3λ
3:  �����ۼƿ��Ŵ���2λ
4:  �����ۼƿ��Ŵ���1λ
4:  �����ۼƿ��Ŵ���0λ
5:  �����ۼ�����¥��3λ
5:  �����ۼ�����¥��2λ
6:  �����ۼ�����¥��1λ
6:  �����ۼ�����¥��0λ  
7:  ƽ�����ٶ�*10
7:  ƽ�����ٶ�*10
8:  ���������ٶ�*10
9:  �ƶ������ٶ�*10
10: ��������ʱ��
10: �������о���
11: Z����ֵ
11: Y����ֵ
12: X����ֵ
12: ����ֵ
13: �������
13: ���²���
14: ��վ��¥��
14: ¥��ƽ�����*10
15: ����
15: Ԥ��
16: ƽ������ʱ��
16: ��ƽ������ʱ��
17: ������ͣ��ʱ��
17: ���г�ʱʱ��
18: ��ʱ�俪��ʱ��
18: ���������Ŵ���
***********************************/
#define PS_FL_ERR       0
#define PS_FL_STATE     2
#define PS_SS_ERR       3
#define PS_FL_DAT       4
#define PS_NM_SPEED     5
#define PS_LF_NUMH      6
#define PS_LF_NUML      8
#define PS_LF_TIMEH     10
#define PS_LF_TIMEL     12
#define PS_NM_PJJSD     14
#define PS_NM_PJZSD     15
#define PS_NM_QDZDJSD   16
#define PS_NM_ZDZDZSD   17
#define PS_TM_DCJSSJ    18
#define PS_TM_DCZSSJ    19
#define PS_TM_DCYXSJ    20
#define PS_NM_DCYXJL    21
#define PS_NM_Z         22
#define PS_NM_Y         23
#define PS_NM_X         24
#define PS_NM_ZY        25
#define PS_FL_UP        26
#define PS_FL_DOWN      27
#define PS_FL_BASE      28
#define PS_NM_LCPJJJ    29  
#define PS_TM_CS        30
#define PS_NONE         31
#define PS_TM_PCKR      32
#define PS_TM_FPCKR     33
#define PS_TM_MQWTT     34
#define PS_TM_YXCS      35
#define PS_TM_CSJKM     36
#define PS_NM_FFKGM     37



//��������״̬
#define B_DT_UP         0x0100
#define B_DT_DOWN       0x0200
#define B_DT_PING       0x0400
#define B_DOOR          0x0800
#define B_HUM           0x1000
#define B_CHECK         0x2000      //�޹�
#define B_KEY_ALARM     0x4000      //�޹�



//���ݹ���
#define B_PCKR          0x0001
#define B_FPCKR         0x0002
#define B_MQWKM         0x0004
#define B_CD            0x0008
#define B_DD            0x0010
#define B_MQWTT         0x0020
#define B_KMZT          0x0040
#define B_CS            0x0080
#define B_YXCS          0x0100
#define B_FFKGM         0x0200
#define B_CSJKM         0x0400
#define B_DTTD          0x0800      //�޹�









void uart_clear(void);
void uart_init(void);
void modbus_rec(uint8_t val);
void buf_add(uint8_t val);
void modbus_send(void);
void mod_buf_write8(uint8_t reg, uint8_t dat);
void mod_buf_write16(uint8_t reg, uint16_t dat);
void mod_buf_write32(uint8_t reg, uint32_t dat);
uint8_t mod_buf_read8(uint8_t reg);
uint16_t mod_buf_read16(uint8_t reg);
uint32_t mod_buf_read32(uint8_t reg);






#endif


