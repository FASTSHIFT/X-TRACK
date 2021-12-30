/*
 * MIT License
 * Copyright (c) 2019-2021 _VIFEXTech
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

/* select the ertc clock source */
#define ERTC_CLOCK_SOURCE_LEXT           /* select lext as the ertc clock */
//#define ERTC_CLOCK_SOURCE_LICK         /* select lick as the ertc clock */

static volatile uint16_t ertc_clk_div_a = 0;
static volatile uint16_t ertc_clk_div_b = 0;


/**
  * @brief  configure the ertc peripheral by selecting the clock source.
  * @param  none
  * @retval none
  */
static void ertc_config(void)
{
    /* enable the pwc clock interface */
    crm_periph_clock_enable(CRM_PWC_PERIPH_CLOCK, TRUE);

    /* allow access to ertc */
    pwc_battery_powered_domain_access(TRUE);

    /* reset ertc domain */
    crm_battery_powered_domain_reset(TRUE);
    crm_battery_powered_domain_reset(FALSE);

#if defined (ERTC_CLOCK_SOURCE_LICK)
    /* enable the lick osc */
    crm_clock_source_enable(CRM_CLOCK_SOURCE_LICK, TRUE);

    /* wait till lick is ready */
    while(crm_flag_get(CRM_LICK_STABLE_FLAG) == RESET)
    {
    }

    /* select the ertc clock source */
    crm_ertc_clock_select(CRM_ERTC_CLOCK_LICK);

    /* ertc second(1hz) = ertc_clk(lick) / (ertc_clk_div_a + 1) * (ertc_clk_div_b + 1) */
    ertc_clk_div_b = 255;
    ertc_clk_div_a = 127;
#elif defined (ERTC_CLOCK_SOURCE_LEXT)
    /* enable the lext osc */
    crm_clock_source_enable(CRM_CLOCK_SOURCE_LEXT, TRUE);

    /* wait till lext is ready */
    while(crm_flag_get(CRM_LEXT_STABLE_FLAG) == RESET)
    {
    }

    /* select the ertc clock source */
    crm_ertc_clock_select(CRM_ERTC_CLOCK_LEXT);

    /* ertc second(1hz) = ertc_clk / (ertc_clk_div_a + 1) * (ertc_clk_div_b + 1) */
    ertc_clk_div_b = 255;
    ertc_clk_div_a = 127;
#endif

    /* enable the ertc clock */
    crm_ertc_clock_enable(TRUE);

    /* deinitializes the ertc registers */
    ertc_reset();

    /* wait for ertc registers update */
    ertc_wait_update();

    /* configure the ertc divider */
    ertc_divider_set(ertc_clk_div_a, ertc_clk_div_b);

    /* configure the ertc hour mode */
    ertc_hour_mode_set(ERTC_HOUR_MODE_24);

    /* set date: 2020-01-01 */
    ertc_date_set(20, 1, 1, 3);

    /* set time: 12:00:00 */
    ertc_time_set(12, 0, 0, ERTC_AM);

//    /* set the alarm 12:00:10 */
//    ertc_alarm_mask_set(ERTC_ALA, ERTC_ALARM_MASK_DATE_WEEK);
//    ertc_alarm_week_date_select(ERTC_ALA, ERTC_SLECT_DATE);
//    ertc_alarm_set(ERTC_ALA, 1, 12, 0, 10, ERTC_AM);

//    /* enable ertc alarm a interrupt */
//    ertc_interrupt_enable(ERTC_ALA_INT, TRUE);

//    /* enable the alarm */
//    ertc_alarm_enable(ERTC_ALA, TRUE);

//    ertc_flag_clear(ERTC_ALAF_FLAG);

    /* indicator for the ertc configuration */
    ertc_bpr_data_write(ERTC_DT1, 0x1234);
}

/**
  * @brief  RTC Init.
  * @param  None
  * @retval None
  */
void RTC_Init(void)
{
    /* enable the pwc clock interface */
    crm_periph_clock_enable(CRM_PWC_PERIPH_CLOCK, TRUE);

    /* allow access to ertc */
    pwc_battery_powered_domain_access(TRUE);

    if (ertc_bpr_data_read(ERTC_DT1) != 0x1234)
    {
        /* ertc configuration */
        ertc_config();
    }
    else
    {
        /* wait for ertc registers update */
        ertc_wait_update();

//        /* clear the ertc alarm flag */
//        ertc_flag_clear(ERTC_ALAF_FLAG);

//        /* clear the exint line 17 pending bit */
//        exint_flag_clear(EXINT_LINE_17);
    }
}

bool RTC_SetTime(uint16_t year, uint8_t mon, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec)
{
    ertc_date_set(year - 2000, mon, day, RTC_GetWeek(year, mon, day));
    ertc_time_set(hour, min, sec, ERTC_AM);
    return true;
}


bool RTC_SetAlarm(uint16_t year, uint8_t mon, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec)
{
    return false;
}

void RTC_GetCalendar(RTC_Calendar_TypeDef* calendar)
{
    ertc_time_type ertc_time;

    /* get the current time */
    ertc_calendar_get(&ertc_time);
    calendar->year = ertc_time.year + 2000;
    calendar->month = ertc_time.month;
    calendar->day = ertc_time.day;
    calendar->week = ertc_time.week;
    calendar->hour = ertc_time.hour;
    calendar->min = ertc_time.min;
    calendar->sec = ertc_time.sec;
}

uint8_t RTC_GetWeek(uint16_t year, uint8_t month, uint8_t day)
{
    uint16_t temp2;
    uint8_t yearH, yearL;
    static const uint8_t table_week[12] = {0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5};            //Monthly correction data sheet.

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
