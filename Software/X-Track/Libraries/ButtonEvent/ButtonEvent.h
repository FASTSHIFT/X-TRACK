/**
  ******************************************************************************
  * @file    ButtonEvent.h
  * @author  FASTSHIFT
  * @version V1.6.0
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
  * @Upgrade 2021.5.12  添加EventType.inc,更优雅的 枚举+字符串 自动生成方式
                        FuncCallBack_t -> FuncCallback_t
  ******************************************************************************
  * @attention
  * 需要提供一个精确到毫秒级的系统时钟，用户需要在ButtonEvent.cpp里定义GET_TIKC()
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BUTTON_EVENT_H
#define __BUTTON_EVENT_H

#include <stdint.h>

class ButtonEvent
{
private:
    typedef void(*FuncCallback_t)(ButtonEvent* btn, int event);

public:
    typedef enum
    {
#       define EVENT_DEF(evt) evt
#       include "EventType.inc"
#       undef EVENT_DEF
        _EVENT_LAST
    } Event_t;

public:
    bool IsPressed;
    bool IsClicked;
    bool IsLongPressed;

public:
    ButtonEvent(
        uint16_t longPressTime = 500,
        uint16_t longPressTimeRepeat = 200,
        uint16_t doubleClickTime = 200
    );
    void EventAttach(FuncCallback_t function);
    void EventMonitor(bool isPress);

    const char* GetEventString(uint16_t event)
    {
        const char* eventStr[_EVENT_LAST] =
        {
#         define EVENT_DEF(evt) #evt
#         include "EventType.inc"
#         undef EVENT_DEF
        };

        return (event < _EVENT_LAST) ? eventStr[event] : "EVENT_NOT_FOUND";
    }

    inline uint16_t GetClickCnt()
    {
        uint16_t cnt = priv.clickCnt + 1;
        priv.clickCnt = 0;
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
        return priv.nowState;
    };


private:
    typedef enum
    {
        STATE_NO_PRESS,
        STATE_PRESS,
        STATE_LONG_PRESS
    } State_t;

private:
    struct
    {
        State_t nowState;
        uint16_t longPressTimeCfg;
        uint16_t longPressRepeatTimeCfg;
        uint16_t doubleClickTimeCfg;
        uint32_t lastLongPressTime;
        uint32_t lastPressTime;
        uint32_t lastClickTime;
        uint16_t clickCnt;
        bool isLongPressed;
        FuncCallback_t eventCallback;
    } priv;

private:
    uint32_t GetTickElaps(uint32_t prevTick);
};

#endif
