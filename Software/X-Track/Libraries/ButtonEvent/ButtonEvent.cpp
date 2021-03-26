#include "ButtonEvent.h"

#ifdef ARDUINO
#  include "Arduino.h"
#  define GET_TICK() millis()
#else
#  error "Please set the system clock used by ButtonEvent"
#endif

#ifndef UINT32_MAX
#  define UINT32_MAX  4294967295u
#endif

/**
  * @brief  按键事件构造函数
  * @param  LongPressTimeMs_Set: 按键长按触发超时设置
  * @param  LongPressTimeRepeatMs_Set: 长按重复触发时间
  * @retval 无
  */
ButtonEvent::ButtonEvent(
    uint16_t longPressTime,
    uint16_t longPressTimeRepeat,
    uint16_t doubleClickTime
)
{
    LongPressTimeCfg = longPressTime;
    LongPressRepeatTimeCfg = longPressTimeRepeat;
    DoubleClickTimeCfg = doubleClickTime;

    LastLongPressTime = LastClickTime = LastPressTime = 0;
    IS_LongPressed = false;
    NowState = STATE_NO_PRESS;

    EventCallbackFunc = NULL;
}

/**
  * @brief  获取与上次时间的时间差(带uint32溢出识别)
  * @param  prevTick: 上的时间戳
  * @retval 时间差
  */
uint32_t ButtonEvent::GetTickElaps(uint32_t prevTick)
{
    uint32_t actTime = GET_TICK();

    if(actTime >= prevTick)
    {
        prevTick = actTime - prevTick;
    }
    else
    {
        prevTick = UINT32_MAX - prevTick + 1;
        prevTick += actTime;
    }

    return prevTick;
}

/**
  * @brief  按键事件绑定
  * @param  function: 回调函数指针
  * @retval 无
  */
void ButtonEvent::EventAttach(FuncCallBack_t function)
{
    EventCallbackFunc = function;
}

/**
  * @brief  监控事件，建议扫描周期10ms
  * @param  nowState: 当前按键状态
  * @retval 无
  */
void ButtonEvent::EventMonitor(bool isPress)
{
    if(EventCallbackFunc == NULL)
        return;

    if (isPress && NowState == STATE_NO_PRESS)
    {
        NowState = STATE_PRESS;
        
        IsPressed = true;
        LastPressTime = GET_TICK();

        EventCallbackFunc(this, EVENT_PRESSED);
        EventCallbackFunc(this, EVENT_CHANGED);
    }
    
    if(NowState == STATE_NO_PRESS)
    {
        return;
    }
    
    if(isPress)
    {
        EventCallbackFunc(this, EVENT_PRESSING);
    }
    
    if (isPress && GetTickElaps(LastPressTime) >= LongPressTimeCfg)
    {
        NowState = STATE_LONG_PRESS;
        
        if(!IS_LongPressed)
        {
            EventCallbackFunc(this, EVENT_LONG_PRESSED);
            LastLongPressTime = GET_TICK();
            IsLongPressed = IS_LongPressed = true;
        }
        else if(GetTickElaps(LastLongPressTime) >= LongPressRepeatTimeCfg)
        {
            LastLongPressTime = GET_TICK();
            EventCallbackFunc(this, EVENT_LONG_PRESSED_REPEAT);
        }
    }
    else if (!isPress)
    {
        NowState = STATE_NO_PRESS;
        
        if(GetTickElaps(LastClickTime) < DoubleClickTimeCfg)
        {
            ClickCnt++;
            EventCallbackFunc(this, EVENT_DOUBLE_CLICKED);
        }
        
        IS_LongPressed = false;
        IsClicked = true;
        LastClickTime = GET_TICK();

        if(GetTickElaps(LastPressTime) < LongPressTimeCfg)
        {
            EventCallbackFunc(this, EVENT_SHORT_CLICKED);
        }
        EventCallbackFunc(this, EVENT_CLICKED);
        EventCallbackFunc(this, EVENT_RELEASED);
        EventCallbackFunc(this, EVENT_CHANGED);
    }
}

