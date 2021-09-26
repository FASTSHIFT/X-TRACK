#ifndef __RTC_H
#define __RTC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mcu_type.h"

void RTCx_Init(void);

void RTCx_Get(uint16_t* year, uint8_t* month, uint8_t* day, uint8_t* week, uint8_t* hour, uint8_t* min, uint8_t* sec);
uint8_t RTCx_Set(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec);

void RTCx_SetAlarmA(uint8_t week,uint8_t hour,uint8_t min,uint8_t sec);
void RTCx_SetWakeUp(uint32_t wksel,uint16_t cnt);

#ifdef __cplusplus
}
#endif

#endif

/******************* (C) COPYRIGHT 2018 ArteryTek *****END OF FILE****/
