/*
 * MIT License
 * Copyright (c) 2023 _VIFEXTech
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
#ifndef __STARTUP_VIEW_H
#define __STARTUP_VIEW_H

#include "../Page.h"

namespace Page {

class StartupView {
public:
    enum class EVENT_ID {
        _LAST,
    };

    enum class MSG_ID {
        SHOW, /* param: bool */
        _LAST,
    };

    class EventListener {
    public:
        virtual void onViewEvent(EVENT_ID id, const void* param = nullptr) = 0;
    };

public:
    StartupView(EventListener* listener, lv_obj_t* root);
    ~StartupView();
    void publish(MSG_ID id, const void* payload);

private:
    EventListener* _listener;
    ResourcePool::Font _font;
    lv_anim_timeline_t* _anim_timeline;

private:
    lv_uintptr_t msgID(MSG_ID id);
    void subscribe(MSG_ID id, lv_obj_t* obj, lv_event_cb_t event_cb);
};

}

#endif /* __STARTUP_VIEW_H */
