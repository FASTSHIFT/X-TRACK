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
#include "DashboardView.h"
#include "Service/DataProc/DataProc_Def.h"
#include "Utils/lv_anim_label/lv_anim_label.h"
#include "Utils/lv_ext/lv_anim_timeline_wrapper.h"
#include "Utils/lv_ext/lv_ext_func.h"
#include <stdarg.h>
#include <stdio.h>

using namespace Page;

DashboardView::DashboardView(EventListener* listener, lv_obj_t* root)
    : _listener(listener)
    , _fontLarge(65, "regular")
#define BINDING_DEF(name, type) , _binding##name((Binding<type, DashboardModel>*)getBinding(BINDING_TYPE::name))
#include "BindingDef.inc"
#undef BINDING_DEF
{
    topInfoCreate(root);
    bottomInfoCreate(root);
}

DashboardView::~DashboardView()
{
}

lv_uintptr_t DashboardView::msgID(MSG_ID id)
{
    return (lv_uintptr_t)this + (lv_uintptr_t)id;
}

void DashboardView::publish(MSG_ID id, const void* payload)
{
    lv_msg_send(msgID(id), payload);
}

void DashboardView::subscribe(MSG_ID id, lv_obj_t* obj, lv_event_cb_t event_cb, void* user_data)
{
    lv_msg_subscribe_obj(msgID(id), obj, this);
    lv_obj_add_event(obj, event_cb, LV_EVENT_MSG_RECEIVED, user_data);
}

void* DashboardView::getBinding(BINDING_TYPE type)
{
    Binding_Info_t info;
    info.type = type;
    info.binding = nullptr;
    _listener->onViewEvent(EVENT_ID::GET_BINDING, &info);
    return info.binding;
}

void DashboardView::topInfoCreate(lv_obj_t* par)
{
    lv_obj_t* cont = lv_obj_create(par);
    {
        lv_obj_remove_style_all(cont);
        lv_obj_set_size(cont, lv_pct(100), 142);

        lv_obj_set_style_bg_opa(cont, LV_OPA_COVER, 0);
        lv_obj_set_style_bg_color(cont, lv_color_hex(0x333333), 0);

        lv_obj_set_style_radius(cont, 27, 0);
        lv_obj_set_y(cont, -36);
    }

    lv_obj_t* labelSpeed = lv_label_create(cont);
    {
        lv_obj_set_style_text_font(labelSpeed, _fontLarge, 0);
        lv_obj_set_style_text_color(labelSpeed, lv_color_white(), 0);
        lv_label_set_text_static(labelSpeed, "00");
        lv_obj_align(labelSpeed, LV_ALIGN_TOP_MID, 0, 63);
    }

    lv_obj_t* labelUnit = lv_label_create(cont);
    {
        lv_obj_set_style_text_color(labelUnit, lv_color_white(), 0);
        lv_label_set_text_static(labelUnit, "km/h");
        lv_obj_align_to(labelUnit, labelSpeed, LV_ALIGN_OUT_BOTTOM_MID, 0, 8);
    }
}

void DashboardView::bottomInfoCreate(lv_obj_t* par)
{
}

void DashboardView::btnCreate(lv_obj_t* par, const void* src)
{
}
