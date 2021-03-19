/**
  ******************************************************************************
  * File   : RTC/Calendar/rtc.h
  * Version: V1.2.3
  * Date   : 2020-08-15
  * Brief  : This file contains the headers of the calendar API.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RTC_H
#define __RTC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "mcu_type.h"

/* Exported types ------------------------------------------------------------*/
typedef struct
{
    __IO uint8_t hour;
    __IO uint8_t min;
    __IO uint8_t sec;
    __IO uint16_t w_year;
    __IO uint8_t  w_month;
    __IO uint8_t  w_date;
    __IO uint8_t  week;
} _calendar_obj;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
extern _calendar_obj calendar;
extern uint8_t const mon_table[12];
void RTC_Init(void);
void RTC_Get(void);
uint8_t Is_Leap_Year(uint16_t year);
uint8_t RTC_Alarm_Set(uint16_t syear, uint8_t smon, uint8_t sday, uint8_t hour, uint8_t min, uint8_t sec);
uint8_t RTC_Get_Week(uint16_t year, uint8_t month, uint8_t day);
uint8_t RTC_Set(uint16_t syear, uint8_t smon, uint8_t sday, uint8_t hour, uint8_t min, uint8_t sec);

#ifdef __cplusplus
}
#endif

#endif

/******************* (C) COPYRIGHT 2018 ArteryTek *****END OF FILE****/
