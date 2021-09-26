#include "flash.h"

/* Base address of the Flash sectors */
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base @ of Sector 0, 16 Kbytes */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base @ of Sector 1, 16 Kbytes */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base @ of Sector 2, 16 Kbytes */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base @ of Sector 3, 16 Kbytes */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base @ of Sector 4, 64 Kbytes */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base @ of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base @ of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base @ of Sector 7, 128 Kbytes */
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) /* Base @ of Sector 8, 128 Kbytes */
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) /* Base @ of Sector 9, 128 Kbytes */
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) /* Base @ of Sector 10, 128 Kbytes */
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) /* Base @ of Sector 11, 128 Kbytes */

#define USER_FLASH_BASE ADDR_FLASH_SECTOR_9
#define USER_DATA_BUF_LENGTH 100

uint16_t UserFlashBuffer[USER_DATA_BUF_LENGTH] = {0};

static uint32_t GetSector(uint32_t Address)
{
    uint32_t sector = 0;

    if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
    {
        sector = FLASH_Sector_0;
    }
    else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
    {
        sector = FLASH_Sector_1;
    }
    else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
    {
        sector = FLASH_Sector_2;
    }
    else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
    {
        sector = FLASH_Sector_3;
    }
    else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
    {
        sector = FLASH_Sector_4;
    }
    else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
    {
        sector = FLASH_Sector_5;
    }
    else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
    {
        sector = FLASH_Sector_6;
    }
    else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
    {
        sector = FLASH_Sector_7;
    }
    else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
    {
        sector = FLASH_Sector_8;
    }
    else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
    {
        sector = FLASH_Sector_9;
    }
    else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
    {
        sector = FLASH_Sector_10;
    }
    else/*(Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_11))*/
    {
        sector = FLASH_Sector_11;
    }
    return sector;
}

static uint8_t Flash_WriteHalfWord_Buf(uint16_t WriteAddr, uint16_t *pBuffer, uint16_t NumToWrite)
{
    FLASH_Status status = FLASH_COMPLETE;
    uint32_t addrx = 0;
    uint32_t endaddr = 0;
    uint8_t retval = 1;

    uint32_t writeAddr = USER_FLASH_BASE + (uint32_t)WriteAddr * 2;

    FLASH_Unlock();//解锁
    FLASH_DataCacheCmd(DISABLE);//FLASH擦除期间，必须禁止数据缓存

    addrx = writeAddr;              //写入起始地址
    endaddr = writeAddr + (uint32_t)NumToWrite * sizeof(uint16_t); //写入结束地址
    if(addrx < 0X1FFF0000)          //是否为主存储区
    {
        while(addrx < endaddr)
        {
            if(Flash_ReadHalfWord(addrx) != 0XFFFF) //对非0xFFFF 的地方，先擦除
            {
                status = FLASH_EraseSector(GetSector(addrx), VoltageRange_3); //VCC=2.7~3.6V之间！！
                if(status != FLASH_COMPLETE)
                {
                    retval = 0;
                    break;  //异常
                }
            }
            else addrx += sizeof(uint16_t);
        }
    }

    if(status == FLASH_COMPLETE)
    {
        while(writeAddr < endaddr) //写数据
        {
            if(FLASH_ProgramHalfWord(writeAddr, *pBuffer) != FLASH_COMPLETE) //写入数据
            {
                retval = 0;
                break;  //写入异常
            }
            writeAddr += sizeof(uint16_t);
            pBuffer++;
        }
    }
    FLASH_DataCacheCmd(ENABLE);//FLASH擦除结束，开启数据缓存
    FLASH_Lock();//上锁
    return retval;
}

uint16_t Flash_ReadHalfWord(uint16_t Addr)
{
    uint32_t addr = 0;
    if(Addr >= USER_DATA_BUF_LENGTH)
        return 0;
    
    addr = USER_FLASH_BASE + Addr * sizeof(uint16_t);
    return *(volatile uint16_t*)addr;
}

static void Flash_ReadHalfWord_Buf(uint16_t ReadAddr, uint16_t *pBuffer, uint16_t NumToRead)
{
    uint16_t i;
    for(i = 0; i < NumToRead; i++)
    {
        pBuffer[i] = Flash_ReadHalfWord(ReadAddr);
        ReadAddr++;
    }
}

uint8_t Flash_WriteHalfWord(uint16_t Addr, uint16_t Data)
{
    static uint8_t first = 0;
    if(Addr >= USER_DATA_BUF_LENGTH)
        return 0;

    if(!first)
    {
        /*备份数据到缓冲区*/
        Flash_ReadHalfWord_Buf(0, UserFlashBuffer, USER_DATA_BUF_LENGTH);
        first = 1;
    }

    /*更新数到缓冲区*/
    UserFlashBuffer[Addr] = Data;

    /*缓冲区重新写入FLASH*/
    return Flash_WriteHalfWord_Buf(0, UserFlashBuffer, USER_DATA_BUF_LENGTH);
}
