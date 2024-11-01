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
#include "SystemInfosView.h"

using namespace Page;

SystemInfosView::SystemInfosView(EventListener* listener, lv_obj_t* root)
    : _listener(listener)
#define BINDING_DEF(name, type) , _binding##name((Binding<type, SystemInfosModel>*)getBinding(BINDING_TYPE::name))
#include "BindingDef.inc"
#undef BINDING_DEF
{
    /* Ensure that MSG_ID is unique */
    static_assert(sizeof(SystemInfosView) >= (size_t)MSG_ID::_LAST, "Large MSG_ID");
}

SystemInfosView::~SystemInfosView()
{
}

lv_uintptr_t SystemInfosView::msgID(MSG_ID id)
{
    return (lv_uintptr_t)this + (lv_uintptr_t)id;
}

void SystemInfosView::publish(MSG_ID id, const void* payload)
{
    lv_msg_send(msgID(id), payload);
}

void SystemInfosView::subscribe(MSG_ID id, lv_obj_t* obj, lv_event_cb_t event_cb)
{
    lv_msg_subscribe_obj(msgID(id), obj, this);
    lv_obj_add_event(obj, event_cb, LV_EVENT_MSG_RECEIVED, this);
}

void* SystemInfosView::getBinding(BINDING_TYPE type)
{
    Binding_Info_t info;
    info.type = type;
    info.binding = nullptr;
    _listener->onViewEvent(EVENT_ID::GET_BINDING, &info);
    return info.binding;
}
