#include "flash_eep.h"
#include "uart.h"

extern uint16_t mod_buf[21];
extern uint32_t time_lj;
extern uint32_t num_b;
extern uint16_t VirtAddVarTab[NB_OF_VAR];

void flash_init(void)
{
    FLASH_Unlock();
    EE_Init();
}


void eep_write16(uint8_t addr, uint16_t dat)
{
    EE_WriteVariable(0xaa00 + addr, dat);
}

void eep_write32(uint8_t addr, uint32_t dat)
{
    EE_WriteVariable(0xaa00 + addr, (uint16_t)(dat >> 16));
    EE_WriteVariable(0xaa00 + addr + 1, (uint16_t)(dat & 0x0000ffff));
}

uint16_t eep_read16(uint8_t addr)
{
    uint16_t rt = 0;

    EE_ReadVariable(0xaa00 + addr, &rt);

    return rt;
}

uint32_t eep_read32(uint8_t addr)
{
    uint32_t rt = 0;
    uint16_t tmp = 0;

    EE_ReadVariable(0xaa00 + addr, &tmp);
    rt = tmp << 16;
    EE_ReadVariable(0xaa00 + addr + 1, &tmp);
    rt |= tmp;

    return rt;
}



//将初始化数据从EEPROM中读出，或者写入初始化数据
void eep_init(void)
{
    uint32_t ps = 0;
    uint8_t i;

    //先把所有参数清零，再读取
    for(i = 0; i < MODBUS_SIZE; i ++)
    {
        mod_buf[i] = 0;
    }

    ps = eep_read32(REG_PASS);

    if(ps == PASS_WORD)
    {
        //已经有参数，则读出
        //开门次数
        time_lj = eep_read32(REG_TIME);
        mod_buf_write32(PS_LF_TIMEH, time_lj);

        //累计运行层数
        num_b = eep_read32(REG_NUM);
        mod_buf_write32(PS_LF_NUMH, num_b);

        //地面层、地下层
        mod_buf_write8(PS_FL_UP, eep_read16(REG_UP));
        mod_buf_write8(PS_FL_DOWN, eep_read16(REG_DOWN));

        //基层、楼层平均间距
        mod_buf_write8(PS_FL_BASE, eep_read16(REG_BASE));
        mod_buf_write8(PS_NM_LCPJJJ, eep_read16(REG_LCPJJJ));

        //超速速度
        mod_buf_write8(PS_TM_CS, eep_read16(REG_CS));

        //平层困人、非平层困人
        mod_buf_write8(PS_TM_PCKR, eep_read16(REG_PCKR));
        mod_buf_write8(PS_TM_FPCKR, eep_read16(REG_FPCKR));

        //门区外停梯、运行超时
        mod_buf_write8(PS_TM_MQWTT, eep_read16(REG_MQWTT));
        mod_buf_write8(PS_TM_YXCS, eep_read16(REG_YXCS));

        //长时间开门、反复开关门
        mod_buf_write8(PS_TM_CSJKM, eep_read16(REG_CSJKM));
        mod_buf_write8(PS_NM_FFKGM, eep_read16(REG_FFKGM));

    }
    else
    {
        //第一次运行，没有参数，写入
        //累计开门次数为0
        time_lj = 0;
        mod_buf_write32(PS_LF_TIMEH, time_lj);

        //累计运行层数为0
        num_b = 0;
        mod_buf_write32(PS_LF_NUMH, num_b);

        //地面层、地下层
        mod_buf_write8(PS_FL_UP, 8);
        mod_buf_write8(PS_FL_DOWN, 1);

        //基层、楼层平均间距、超速
        mod_buf_write8(PS_FL_BASE, 8);
        mod_buf_write8(PS_NM_LCPJJJ, 30);
        mod_buf_write8(PS_TM_CS, 3);

        //平层困人、非平层困人
        mod_buf_write8(PS_TM_PCKR, 90);
        mod_buf_write8(PS_TM_FPCKR, 15);

        //门区外停梯、运行超时
        mod_buf_write8(PS_TM_MQWTT, 15);
        mod_buf_write8(PS_TM_YXCS, 1);

        //长时间开门、反复开关门
        mod_buf_write8(PS_TM_CSJKM, 1);
        mod_buf_write8(PS_NM_FFKGM, 20);

        eep_write32(REG_TIME, time_lj);
        eep_write32(REG_NUM, num_b);
        eep_write16(REG_UP, 8);
        eep_write16(REG_DOWN, 1);
        eep_write16(REG_BASE, 8);
        eep_write16(REG_LCPJJJ, 30);
        eep_write16(REG_CS, 3);
        eep_write16(REG_PCKR, 90);
        eep_write16(REG_FPCKR, 15);
        eep_write16(REG_MQWTT, 15);
        eep_write16(REG_YXCS, 1);
        eep_write16(REG_CSJKM, 1);
        eep_write16(REG_FFKGM, 20);
        eep_write32(REG_PASS, PASS_WORD);
    }
}






