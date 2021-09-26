#include "rtc.h"

static void RTCx_GetWeekDay(RTC_DateTypeDef *RTC_DateStruct);
static ErrorStatus RTCx_SetTime(uint8_t hour,uint8_t min,uint8_t sec,uint8_t ampm);
static ErrorStatus RTCx_SetDate(uint8_t year,uint8_t month,uint8_t date,uint8_t week);


/**
  * @brief  RTC Init.
  * @param  None
  * @retval None
  */
void RTCx_Init(void)
{
    /* Enable PWR and BKP clocks */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);//使能PWR时钟
    /* Allow access to BKP Domain */
    PWR_BackupAccessCmd(ENABLE);    //使能后备寄存器访问

    /* Check Backup data registers is correct*/
    if (RTC_ReadBackupRegister(RTC_BKP_DR1) != 0x5051)
    {
#if defined(CFG_ENABLE_RTC_CLKSRC_HSE)
        RCC_LSEConfig(RCC_LSE_OFF);
        // RCC_HSEConfig(RCC_HSE_ON);
        // while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET); //检查指定的RCC标志位设置与否,等待低速晶振就绪
        // Use HSE as RTC clock source
    #if defined(HSE_VALUE) && HSE_VALUE == 25000000
        RCC_RTCCLKConfig(RCC_RTCCLKSource_HSE_Div25);
    #elif defined(HSE_VALUE) && HSE_VALUE == 12000000
        RCC_RTCCLKConfig(RCC_RTCCLKSource_HSE_Div12);
    #elif defined(HSE_VALUE) && HSE_VALUE == 8000000
        RCC_RTCCLKConfig(RCC_RTCCLKSource_HSE_Div8);
    #else
        #warning "incorrect HSE_VALUE defined, RTC may inaccurate"
        RCC_RTCCLKConfig(RCC_RTCCLKSource_HSE_Div16);
    #endif

#else
        // by default, LSE will be used.
        RCC_LSEConfig(RCC_LSE_ON);//LSE 开启
        while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET); //检查指定的RCC标志位设置与否,等待低速晶振就绪
        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);     //设置RTC时钟(RTCCLK),选择LSE作为RTC时钟
#endif

        // Enable RTC Clock
        RCC_RTCCLKCmd(ENABLE);  //使能RTC时钟

        // Init RTC
        RTC_InitTypeDef RTC_InitStructure;
        RTC_StructInit(&RTC_InitStructure);
#if defined(CFG_ENABLE_RTC_CLKSRC_HSE)
        RTC_InitStructure.RTC_AsynchPrediv = 124U;
        RTC_InitStructure.RTC_SynchPrediv  = 7999U;
#else
        RTC_InitStructure.RTC_AsynchPrediv = 0x7F;//RTC异步分频系数(1~0X7F)
        RTC_InitStructure.RTC_SynchPrediv  = 0xFF;//RTC同步分频系数(0~7FFF)
#endif
        RTC_InitStructure.RTC_HourFormat   = RTC_HourFormat_24;//RTC设置为,24小时格式

        ErrorStatus ret;
        ret = RTC_Init(&RTC_InitStructure);
        if (ret != SUCCESS) {
            // TRACE("RTC Init failed");
            return;
        }

        // Set date time
        RTCx_SetTime(00, 00, 00, RTC_H12_AM); //设置时间
        RTCx_SetDate(00, 1, 1, 1);  //设置日期

        /* Writes data to Backup Register */
        RTC_WriteBackupRegister(RTC_BKP_DR1, 0x5051);
    }
}

/**
  * @brief  RTC时间设置
  * @param  hour:小时
  * @param  min:分钟
  * @param  sec:秒钟
  * @param  ampm:@RTC_AM_PM_Definitions  :RTC_H12_AM/RTC_H12_PM
  * @retval SUCCESS(1),成功 ERROR(0),进入初始化模式失败
  */
static ErrorStatus RTCx_SetTime(uint8_t hour, uint8_t min, uint8_t sec, uint8_t ampm)
{
    RTC_TimeTypeDef RTC_TimeTypeInitStructure;
    RTC_TimeTypeInitStructure.RTC_Hours = hour;
    RTC_TimeTypeInitStructure.RTC_Minutes = min;
    RTC_TimeTypeInitStructure.RTC_Seconds = sec;
    RTC_TimeTypeInitStructure.RTC_H12 = ampm;

    return RTC_SetTime(RTC_Format_BIN, &RTC_TimeTypeInitStructure);
}

/**
  * @brief  RTC日期设置
  * @param  year:年(0~99)
  * @param  month:月(1~12)
  * @param  date:日(0~31)
  * @param  week:星期(1~7,0:非法!)
  * @retval SUCCESS(1),成功 ERROR(0),进入初始化模式失败
  */
static ErrorStatus RTCx_SetDate(uint8_t year, uint8_t month, uint8_t date, uint8_t week)
{
    RTC_DateTypeDef RTC_DateStruct;
    RTC_DateStruct.RTC_Year = year;
    RTC_DateStruct.RTC_Month = month;
    RTC_DateStruct.RTC_Date = date;
    if (week == 0)
        RTCx_GetWeekDay(&RTC_DateStruct);
    else
        RTC_DateStruct.RTC_WeekDay = week;
    
    return RTC_SetDate(RTC_Format_BIN, &RTC_DateStruct);
}

/**
  * @brief  日期换算星期(1~7)
  * @param  RTC_DateStruct:日期结构体地址
  * @retval 无
  */
static void RTCx_GetWeekDay(RTC_DateTypeDef *RTC_DateStruct)
{
    int year = RTC_DateStruct->RTC_Year + 2000;
    int month = RTC_DateStruct->RTC_Month;
    int date = RTC_DateStruct->RTC_Date;
    if(month == 1) month = 13, year--;
    if(month == 2) month = 14, year--;
    int weekdey = (date + 2 * month + 3 * (month + 1) / 5 + year + year / 4 - year / 100 + year / 400) % 7;
    RTC_DateStruct->RTC_WeekDay = weekdey + 1;
}


/**
  * @brief  Set time. Convert the input clock to a second.
  *         legitimate year: 2000 ~ 2099
  * @param  syear: Year
  *         smon : Month
  *         sday : Day
  *         hour : 0 ~ 23
  *         min  : 0 ~ 59
  *         sec  : 0 ~ 59
  * @retval 0: Set time right.
  *         1: Set time failed.
  */
uint8_t RTCx_Set(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec)
{
    if(year < 2000 || year > 2099)
        return 1;

    ErrorStatus ret;

    ret = RTCx_SetDate(year - 2000, month, day, 0);
    if (ret != SUCCESS) {
        // TRACE("RTC set date error");
        return 1;
    }

    ret = RTCx_SetTime(hour, min, sec, RTC_H12_AM);
    if (ret != SUCCESS) {
        // TRACE("RTC set time error");
        return 1;
    }

    return 0;
}

/**
 * @brief 
 * 
 * @param year 
 * @param month 
 * @param day 
 * @param week 
 * @param hour 
 * @param min 
 * @param sec 
 * @return uint8_t 
 */
void RTCx_Get(uint16_t* year, uint8_t* month, uint8_t* day, uint8_t* week, uint8_t* hour, uint8_t* min, uint8_t* sec) {
    RTC_DateTypeDef date;
    RTC_GetDate(RTC_Format_BIN, &date);
    *year  = date.RTC_Year + 2000;
    *month = date.RTC_Month;
    *day   = date.RTC_Date;
    *week  = date.RTC_WeekDay;

    RTC_TimeTypeDef time;
    RTC_GetTime(RTC_Format_BIN, &time);
    *hour = time.RTC_Hours;
    *min  = time.RTC_Minutes;
    *sec  = time.RTC_Seconds;
}

/**
  * @brief  设置闹钟时间(按星期闹铃,24小时制)
  * @param  week:星期几(1~7) @ref  RTC_Alarm_Definitions
  * @param  hour:小时
  * @param  min:分钟
  * @param  sec:秒钟
  * @retval 0:初始化成功; 1:LSE开启失败; 2:进入初始化模式失败;
  */
void RTCx_SetAlarmA(uint8_t week, uint8_t hour, uint8_t min, uint8_t sec)
{
    RTC_AlarmTypeDef RTC_AlarmTypeInitStructure;
    RTC_TimeTypeDef RTC_TimeTypeInitStructure;
    NVIC_InitTypeDef  NVIC_InitStructure;

    RTC_AlarmCmd(RTC_Alarm_A, DISABLE); //关闭闹钟A

    RTC_TimeTypeInitStructure.RTC_Hours = hour; //小时
    RTC_TimeTypeInitStructure.RTC_Minutes = min; //分钟
    RTC_TimeTypeInitStructure.RTC_Seconds = sec; //秒
    RTC_TimeTypeInitStructure.RTC_H12 = RTC_H12_AM;

    RTC_AlarmTypeInitStructure.RTC_AlarmDateWeekDay = week; //星期
    RTC_AlarmTypeInitStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_WeekDay; //按星期闹
    RTC_AlarmTypeInitStructure.RTC_AlarmMask = RTC_AlarmMask_None; //精确匹配星期，时分秒
    RTC_AlarmTypeInitStructure.RTC_AlarmTime = RTC_TimeTypeInitStructure;
    RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmTypeInitStructure);

    RTC_ClearITPendingBit(RTC_IT_ALRA);//清除RTC闹钟A的标志
    RTC_ITConfig(RTC_IT_ALRA, ENABLE); //开启闹钟A中断
    RTC_AlarmCmd(RTC_Alarm_A, ENABLE); //开启闹钟A

    NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;//抢占优先级1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//子优先级2
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
    NVIC_Init(&NVIC_InitStructure);//配置
}

/**
  * @brief  周期性唤醒定时器设置
  * @param  wksel:  @ref RTC_Wakeup_Timer_Definitions
  * @param  cnt:自动重装载值.减到0,产生中断.
  * @retval 无
  */
void RTCx_SetWakeUp(uint32_t wksel, uint16_t cnt)
{
    NVIC_InitTypeDef  NVIC_InitStructure;

    RTC_WakeUpCmd(DISABLE);//关闭WAKE UP
    RTC_WakeUpClockConfig(wksel);//唤醒时钟选择
    RTC_SetWakeUpCounter(cnt);//设置WAKE UP自动重装载寄存器
    RTC_ClearITPendingBit(RTC_IT_WUT); //清除RTC WAKE UP的标志

    RTC_ITConfig(RTC_IT_WUT, ENABLE); //开启WAKE UP 定时器中断
    RTC_WakeUpCmd( ENABLE);//开启WAKE UP 定时器　

    NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;//抢占优先级1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//子优先级2
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
    NVIC_Init(&NVIC_InitStructure);//配置
}

/**
  * @brief  RTC闹钟中断服务函数
  * @param  无
  * @retval 无
  */
void RTC_Alarm_IRQHandler(void)
{
    if(RTC_GetFlagStatus(RTC_FLAG_ALRAF) == SET) //ALARM A中断
    {
        RTC_ClearFlag(RTC_FLAG_ALRAF);//清除中断标志
    }
}

/**
  * @brief  RTC WAKE UP中断服务函数
  * @param  无
  * @retval 无
  */
void RTC_WKUP_IRQHandler(void)
{
    if(RTC_GetFlagStatus(RTC_FLAG_WUTF) == SET) //WK_UP中断
    {
        RTC_ClearFlag(RTC_FLAG_WUTF);   //清除中断标志
    }
}