/*
 * MIT License
 * Copyright (c) 2017 - 2022 _VIFEXTech
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "rtc.h"

static const uint8_t table_week[12] = {0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5};            //Monthly correction data sheet.
static const uint8_t mon_table[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}; //Month data table of Pingnian

/**
  * @brief  RTC Init.
  * @param  None
  * @retval None
  */
void RTC_Init(void)
{
    /* Enable PWR and BKP clocks */
    RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_PWR | RCC_APB1PERIPH_BKP, ENABLE);

    /* Allow access to BKP Domain */
    PWR_BackupAccessCtrl(ENABLE);

    /* Check Backup data registers is correct*/
    if (BKP_ReadBackupReg(BKP_DT1) != 0x5051)
    {
        /* Reset Backup Domain */
        BKP_Reset();

        /* Enable the LSI OSC */
        RCC_LSEConfig(RCC_LSE_ENABLE);
        /* Wait till LSI is ready */
        while(RCC_GetFlagStatus(RCC_FLAG_LSESTBL) == RESET);
        /* Select the RTC Clock Source */
        RCC_RTCCLKConfig(RCC_RTCCLKSelection_LSE);

        /* Enable RTC Clock */
        RCC_RTCCLKCmd(ENABLE);
        /* Wait for RTC registers synchronization */
        RTC_WaitForSynchro();
        /* Wait until last write operation on RTC registers has finished */
        RTC_WaitForLastTask();

        /* Set RTC prescaler: set RTC period to 1sec */
        RTC_SetDIV(32767);
        /* Wait until last write operation on RTC registers has finished */
        RTC_WaitForLastTask();

        /* Set the RTC time */
        RTC_SetTime(2018, 8, 8, 8, 8, 0);
        /* Wait until last write operation on RTC registers has finished */
        RTC_WaitForLastTask();

        /* Writes data to Backup Register */
        BKP_WriteBackupReg(BKP_DT1, 0x5051);
    }
    else
    {
        /* Wait for RTC registers synchronization */
        RTC_WaitForSynchro();
        /* Wait until last write operation on RTC registers has finished */
        RTC_WaitForLastTask();
        /* Clear RTC pending flag */
        RTC_ClearFlag(RTC_FLAG_PACE);
        /* Wait until last write operation on RTC registers has finished */
        RTC_WaitForLastTask();
    }
}

/**
  * @brief  Judeg the Leap year or Pingnian.
  *         Month      1  2  3  4  5  6  7  8  9  10 11 12
  *         Leap year  31 29 31 30 31 30 31 31 30 31 30 31
  *         Pingnian   31 28 31 30 31 30 31 31 30 31 30 31
  * @param  year
  * @retval 1: Leap year
            2: Pingnian
  */
static uint8_t Is_Leap_Year(uint16_t year)
{
    if(year % 4 == 0)
    {
        if(year % 100 == 0)
        {
            if(year % 400 == 0) return 1;
            else return 0;
        }
        else return 1;
    }
    else return 0;
}

/**
  * @brief  Set time. Convert the input clock to a second.
  *         The time basic : 1970.1.1
  *         legitimate year: 1970 ~ 2099
  * @param  syear: Year
  *         smon : Month
  *         sday : Day
  *         hour
  *         min
  *         sec
  * @retval 0: Set time right.
  *         1: Set time failed.
  */
uint8_t RTC_SetTime(uint16_t syear, uint8_t smon, uint8_t sday, uint8_t hour, uint8_t min, uint8_t sec)
{
    uint32_t t;
    uint32_t seccount = 0;

    if(syear < 1970 || syear > 2099)
        return 1;

    for(t = 1970; t < syear; t++)
    {
        if(Is_Leap_Year(t))seccount += 31622400;
        else seccount += 31536000;
    }
    smon -= 1;
    for(t = 0; t < smon; t++)
    {
        seccount += (uint8_t)mon_table[t] * 86400;
        if(Is_Leap_Year(syear) && t == 1)seccount += 86400;
    }
    seccount += (uint8_t)(sday - 1) * 86400;
    seccount += (uint8_t)hour * 3600;
    seccount += (uint8_t)min * 60;
    seccount += sec;

    /* Enable PWR and BKP clock */
    RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_PWR | RCC_APB1PERIPH_BKP, ENABLE);

    /* Enable write access to Backup domain */
    PWR_BackupAccessCtrl(ENABLE);

    /* Set the RTC counter value */
    RTC_SetCounter(seccount);
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();

    return 0;
}

/**
  * @brief  Set RTC alarm.
  *         The time basic : 1970.1.1
  *         legitimate year: 1970 ~ 2099
  * @param  syear : Year
  *         smon : Month
  *         sday : Day
  *         hour
  *         min
  *         sec
  * @retval 0: Set Alarm right.
  *         1: Set Alarm failed.
  */
uint8_t RTC_SetAlarm(uint16_t syear, uint8_t smon, uint8_t sday, uint8_t hour, uint8_t min, uint8_t sec)
{
    uint16_t t;
    uint8_t seccount = 0;

    if(syear < 1970 || syear > 2099)
        return 1;

    for(t = 1970; t < syear; t++)
    {
        if(Is_Leap_Year(t))seccount += 31622400;
        else seccount += 31536000;
    }
    smon -= 1;
    for(t = 0; t < smon; t++)
    {
        seccount += (uint8_t)mon_table[t] * 86400;
        if(Is_Leap_Year(syear) && t == 1)seccount += 86400;
    }
    seccount += (uint8_t)(sday - 1) * 86400;
    seccount += (uint8_t)hour * 3600;
    seccount += (uint8_t)min * 60;
    seccount += sec;

    /* Enable PWR and BKP clock */
    RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_PWR | RCC_APB1PERIPH_BKP, ENABLE);

    /* Enable write access to Backup domain */
    PWR_BackupAccessCtrl(ENABLE);

    /* Set the RTC alarm value */
    RTC_SetAlarmValue(seccount);
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();

    return 0;
}

/**
  * @brief  Get current time.
  * @param  None.
  * @retval None.
  */
void RTC_GetCalendar(RTC_Calendar_TypeDef* calendar)
{
    static RTC_Calendar_TypeDef _calendar;
    static uint16_t daycnt = 0;
    uint32_t timecount = 0;
    uint32_t temp = 0;
    uint32_t temp1 = 0;

    timecount = RTC_GetCounter();
    temp = timecount / 86400;
    if(daycnt != temp)
    {
        daycnt = temp;
        temp1 = 1970;
        while(temp >= 365)
        {
            if(Is_Leap_Year(temp1))
            {
                if(temp >= 366)temp -= 366;
                else
                {
                    temp1++;
                    break;
                }
            }
            else temp -= 365;
            temp1++;
        }
        _calendar.year = temp1;
        temp1 = 0;
        while(temp >= 28)
        {
            if(Is_Leap_Year(_calendar.year) && temp1 == 1)
            {
                if(temp >= 29)temp -= 29;
                else break;
            }
            else
            {
                if(temp >= mon_table[temp1])temp -= mon_table[temp1];
                else break;
            }
            temp1++;
        }
        _calendar.month = temp1 + 1;
        _calendar.day = temp + 1;
    }
    temp = timecount % 86400;
    _calendar.hour = temp / 3600;
    _calendar.min = (temp % 3600) / 60;
    _calendar.sec = (temp % 3600) % 60;
    _calendar.week = RTC_GetWeek(_calendar.year, _calendar.month, _calendar.day);

    *calendar = _calendar;
}

/**
  * @brief  Get current week by Input Leap year\mouth\day.
  * @param  syear : Year
  *         smon  : Month
  *         sday  : Day
  * @retval week number.
  */
uint8_t RTC_GetWeek(uint16_t year, uint8_t month, uint8_t day)
{
    uint16_t temp2;
    uint8_t yearH, yearL;

    yearH = year / 100;
    yearL = year % 100;
    if (yearH > 19)yearL += 100;
    temp2 = yearL + yearL / 4;
    temp2 = temp2 % 7;
    temp2 = temp2 + day + table_week[month - 1];
    if (yearL % 4 == 0 && month < 3)
        temp2--;
    return(temp2 % 7);
}
