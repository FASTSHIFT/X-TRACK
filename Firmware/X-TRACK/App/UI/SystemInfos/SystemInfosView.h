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
#ifndef __SYSTEMINFOS_VIEW_H
#define __SYSTEMINFOS_VIEW_H

#include "../Page.h"
#include "Service/DataProc/DataProc.h"
#include "Service/HAL/HAL.h"
#include "Utils/Binding/Binding.h"

namespace Page {

class SystemInfosModel;

class SystemInfosView {
public:
    enum class EVENT_ID {
        GET_BINDING, /* Param: Binding_Info_t */
        _LAST,
    };

    enum class MSG_ID {
        SPORT_STATUS, /* param: SportStatus_Info_t */
        GNSS, /* param: GNSS_Info_t */
        CLOCK, /* param: Clock_Info_t */
        POWER, /* param: Power_Info_t */
        _LAST,
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
    SystemInfosView(EventListener* listener, lv_obj_t* root);
    ~SystemInfosView();
    void publish(MSG_ID id, const void* payload = nullptr);

private:
    EventListener* _listener;

    struct STYLE {
        STYLE()
        {
            lv_style_init(&icon);
            lv_style_init(&info);
        }
        ~STYLE()
        {
            lv_style_reset(&icon);
            lv_style_reset(&info);
        }
        lv_style_t icon;
        lv_style_t info;
    } _style;

    ResourcePool::Font _fontAwesome;

#define BINDING_DEF(name, type) Binding<type, SystemInfosModel>* _binding##name;
#include "BindingDef.inc"
#undef BINDING_DEF

private:
    lv_uintptr_t msgID(MSG_ID id);
    void subscribe(MSG_ID id, lv_obj_t* obj, lv_event_cb_t event_cb);
    void* getBinding(BINDING_TYPE type);
    void rootInit(lv_obj_t* root);
    static void onRootScroll(lv_obj_t* obj);
    void styleInit();
    void itemGroupCreate(lv_obj_t* par);
    lv_obj_t* itemCreate(
        lv_obj_t* par,
        const char* name,
        const char* symbol,
        const char* infos);

    static const char* makeTimeString(uint64_t ms);
    static const char* makeTimeString(const HAL::Clock_Info_t* info);
};

}

#endif /* __SYSTEMINFOS_VIEW_H */
