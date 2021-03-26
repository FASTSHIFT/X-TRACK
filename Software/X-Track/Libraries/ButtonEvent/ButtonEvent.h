/**
  ******************************************************************************
  * @file    ButtonEvent.h
  * @author  FASTSHIFT
  * @version V1.3.0
  * @date    2018-12-23
  * @brief   非阻塞式按键事件驱动库，支持短摁、长摁、释放、状态改变事件的识别。
  * @Upgrade 2019.6.18  添加按键双击事件、长按单次触发事件。
  * @Upgrade 2019.8.26  添加GetClicked、GetPressed、GetLongPressed非事件模式支持。
  * @Upgrade 2019.12.4  使用了新的事件回调设计，所有事件统一用EventAttach。
                        添加Click、LongPressed、LongPressRepeat事件。
  * @Upgrade 2020.6.12  合并ButtonEvent_Type至ButtonEvent类
  * @Upgrade 2021.3.22  添加EVENT_SHORT_CLICKED和EVENT_PRESSING
                        整理命名，优化不必要的标志位
                        EventMonitor()形参使用bool类型，去除NoPressState统一状态
  ******************************************************************************
  * @attention
  * 需要提供一个精确到毫秒级的系统时钟，用户需要在ButtonEvent.cpp里定义ButtonEvent_Millis
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BUTTON_EVENT_H
#define __BUTTON_EVENT_H

#include <stdint.h>

class ButtonEvent
{
    typedef void(*FuncCallBack_t)(ButtonEvent*, int);
public:
    ButtonEvent(
        uint16_t longPressTime = 500,
        uint16_t longPressTimeRepeat = 200,
        uint16_t doubleClickTime = 200
    );

    typedef enum
    {
        EVENT_NONE,
        EVENT_PRESSED,
        EVENT_PRESSING,
        EVENT_LONG_PRESSED,
        EVENT_LONG_PRESSED_REPEAT,
        EVENT_RELEASED,
        EVENT_CHANGED,
        EVENT_CLICKED,
        EVENT_SHORT_CLICKED,
        EVENT_DOUBLE_CLICKED,
        _EVENT_MAX
    } Event_t;

    const char* GetEventString(uint16_t event)
    {
        const char* eventStr[_EVENT_MAX] =
        {
            "EVENT_NONE",
            "EVENT_PRESSED",
            "EVENT_PRESSING",
            "EVENT_LONG_PRESSED",
            "EVENT_LONG_PRESSED_REPEAT",
            "EVENT_RELEASED",
            "EVENT_CHANGED",
            "EVENT_CLICKED",
            "EVENT_SHORT_CLICKED",
            "EVENT_DOUBLE_CLICKED"
        };
        
        return (event < _EVENT_MAX) ? eventStr[event] : "_EVENT_NOT_FOUND";
    }

    bool IsPressed;
    bool IsClicked;
    bool IsLongPressed;

    void EventAttach(FuncCallBack_t function);
    void EventMonitor(bool isPress);

    inline uint16_t GetClickCnt()
    {
        uint16_t cnt = ClickCnt + 1;
        ClickCnt = 0;
        return cnt;
    }
    inline bool GetClicked()
    {
        bool n = IsClicked;
        IsClicked = false;
        return n;
    }
    inline bool GetPressed()
    {
        bool n = IsPressed;
        IsPressed = false;
        return n;
    }
    inline bool GetLongPressed()
    {
        bool n = IsLongPressed;
        IsLongPressed = false;
        return n;
    }

    operator uint8_t()
    {
        return NowState;
    };

private:
    typedef enum
    {
        STATE_NO_PRESS,
        STATE_PRESS,
        STATE_LONG_PRESS
    } State_t;

    State_t NowState;
    uint16_t LongPressTimeCfg;
    uint16_t LongPressRepeatTimeCfg;
    uint16_t DoubleClickTimeCfg;
    uint32_t LastLongPressTime;
    uint32_t LastPressTime;
    uint32_t LastClickTime;
    uint16_t ClickCnt;
    bool IS_LongPressed;
    FuncCallBack_t EventCallbackFunc;

    uint32_t GetTickElaps(uint32_t prevTick);
};

#endif
