/*
 * MIT License
 * Copyright (c) 2023 - 2024 _VIFEXTech
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
#ifndef __DASHBOARD_VIEW_H
#define __DASHBOARD_VIEW_H

#include "../Page.h"
#include "Utils/Binding/Binding.h"

namespace Page {

class DashboardModel;

class DashboardView {
public:
    enum class EVENT_ID {
        GET_BINDING, /* Param: Binding_Info_t */
        NAVI_TO_PAGE, /* Param: const char* */
        _LAST,
    };

    enum class MSG_ID {
        NONE,
        SPORT_STATUS, /* param: SportStatus_Info_t */
        RECORDER_STATUS, /* param: Recorder_Info_t */
        GNSS_STATUS, /* param: GNSS_Info_t */
        ANIM_START, /* param: None */
        _LAST
    };

    enum class BINDING_TYPE {
#define BINDING_DEF(name, type) name,
#include "BindingDef.inc"
#undef BINDING_DEF
    };

    typedef struct {
        BINDING_TYPE type;
        void* binding;
    } Binding_Info_t;

    class EventListener {
    public:
        virtual void onViewEvent(EVENT_ID id, const void* param = nullptr) = 0;
    };

public:
    DashboardView(EventListener* listener, lv_obj_t* root);
    ~DashboardView();
    void publish(MSG_ID id, const void* payload = nullptr);

private:
    EventListener* _listener;
    ResourcePool::Font _fontLarge;
    lv_anim_timeline_t* _anim_timeline;

#define BINDING_DEF(name, type) Binding<type, DashboardModel>* _binding##name;
#include "BindingDef.inc"
#undef BINDING_DEF

private:
    lv_uintptr_t msgID(MSG_ID id);
    void subscribe(MSG_ID id, lv_obj_t* obj, lv_event_cb_t cb, void* user_data = nullptr);
    void* getBinding(BINDING_TYPE type);

    lv_obj_t* topInfoCreate(lv_obj_t* par);

    lv_obj_t* bottomInfoCreate(lv_obj_t* par);
    lv_obj_t* infoItemCreate(lv_obj_t* par, const char* title);

    lv_obj_t* btnGroupCreate(lv_obj_t* par);
    lv_obj_t* btnCreate(lv_obj_t* par, const char* symbol, const char* pageID = nullptr);

    static const char* makeTimeString(uint64_t ms);
};

}

#endif /* __DASHBOARD_VIEW_H */
