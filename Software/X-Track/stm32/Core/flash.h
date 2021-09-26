#ifndef __FLASH_H
#define __FLASH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mcu_type.h"

uint8_t Flash_WriteHalfWord(uint16_t Addr, uint16_t Data);
uint16_t Flash_ReadHalfWord(uint16_t Addr);
    
#ifdef __cplusplus
}
#endif

#endif
