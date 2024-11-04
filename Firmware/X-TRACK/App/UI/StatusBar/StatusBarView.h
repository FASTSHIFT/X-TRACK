/*
 * MIT License
 * Copyright (c) 2021 - 2024 _VIFEXTech
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
#ifndef __STATUS_BAR_VIEW_H
#define __STATUS_BAR_VIEW_H

#include "../Page.h"

namespace Page {

class StatusBarView {
public:
    enum class STYLE_ID {
        DARK,
        LIGHT,
        _LAST
    };

    enum class MSG_ID {
        SHOW, /* param: bool */
        CLOCK, /* param: uint16_t (uint8_t)Hour << 8 | (uint8_t)Minute */
        SAT_NUM, /* param: int16_t */
        POWER, /* param: DataProc::Power_Info_t */
        STYLE_CHANGE, /* param: STYLE_ID */
        REC_CHANGE, /* param: const char* */
        _LAST,
    };

    enum class EVENT_ID {
        HEIGHT_UPDATE, /* param: int */
        _LAST,
    };

    class EventListener {
    public:
        virtual void onViewEvent(EVENT_ID id, const void* param = nullptr) = 0;
    };

public:
    StatusBarView(EventListener* listener, lv_obj_t* par);
    ~StatusBarView();
    void publish(MSG_ID id, const void* payload);

private:
    EventListener* _listener;

    ResourcePool::Font _fontMedium;
    ResourcePool::Font _fontAwesome;
    lv_font_t _fontHandle;

    struct {
        int16_t satellitesNum;
        uint8_t timeCheckSum;
        uint8_t battLevel;
    } _curState;

private:
    lv_uintptr_t msgID(MSG_ID id);
    void subscribe(MSG_ID id, lv_obj_t* obj, lv_event_cb_t event_cb);
    lv_obj_t* contCreate(lv_obj_t* par);
    void satelliteCreate(lv_obj_t* par);
    void clockCreate(lv_obj_t* par);
    void batteryCreate(lv_obj_t* par);
    void recCreate(lv_obj_t* par);
};

}

#endif /* __STATUS_BAR_VIEW_H */
