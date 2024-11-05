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
#include "Service/HAL/HAL_Def.h"
#include "Utils/Geo/Geo.h"
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
    lv_obj_set_style_pad_all(root, 0, 0);

    lv_obj_t* topInfoCont = topInfoCreate(root);
    lv_obj_t* bottomInfoCont = bottomInfoCreate(root);
    lv_obj_t* btnGrpCont = btnGroupCreate(root);
    lv_obj_move_foreground(topInfoCont);

    _anim_timeline = lv_anim_timeline_create();

#define ANIM_DEF(start_time, obj, attr, start, end)                                       \
    {                                                                                     \
        start_time, obj, LV_ANIM_EXEC(attr), start, end, 500, lv_anim_path_ease_out, true \
    }

    auto lv_obj_set_opa = [](void* obj, int32_t v) {
        lv_obj_set_style_opa((lv_obj_t*)obj, v, 0);
    };

    auto lv_obj_set_trans_y = [](void* obj, int32_t v) {
        lv_obj_set_style_translate_y((lv_obj_t*)obj, v, 0);
    };

    auto lv_obj_set_trans_h = [](void* obj, int32_t v) {
        lv_obj_set_style_transform_height((lv_obj_t*)obj, v, 0);
    };

    auto lv_obj_set_text_opa = [](void* obj, int32_t v) {
        lv_obj_set_style_text_opa((lv_obj_t*)obj, v, 0);
    };

    lv_obj_update_layout(root);
    lv_coord_t btn_h = lv_obj_get_height(lv_obj_get_child(btnGrpCont, 0));

    lv_anim_timeline_wrapper_t wrapper[] = {
        ANIM_DEF(0, topInfoCont, trans_y, -lv_obj_get_height(topInfoCont), 0),
        ANIM_DEF(200, bottomInfoCont, trans_y, -lv_obj_get_height(bottomInfoCont), 0),
        ANIM_DEF(200, bottomInfoCont, opa, LV_OPA_TRANSP, LV_OPA_COVER),

        ANIM_DEF(500, lv_obj_get_child(btnGrpCont, 0), trans_h, -btn_h, 0),
        ANIM_DEF(600, lv_obj_get_child(btnGrpCont, 1), trans_h, -btn_h, 0),
        ANIM_DEF(700, lv_obj_get_child(btnGrpCont, 2), trans_h, -btn_h, 0),
        ANIM_DEF(900, btnGrpCont, text_opa, LV_OPA_TRANSP, LV_OPA_COVER),
        LV_ANIM_TIMELINE_WRAPPER_END
    };
    lv_anim_timeline_add_wrapper(_anim_timeline, wrapper);
    lv_anim_timeline_set_progress(_anim_timeline, 0);

    subscribe(
        MSG_ID::ANIM_START,
        root,
        [](lv_event_t* e) {
            auto self = (DashboardView*)lv_event_get_user_data(e);
            auto msg = lv_event_get_msg(e);
            if (lv_msg_get_id(msg) != self->msgID(MSG_ID::ANIM_START)) {
                return;
            }

            lv_anim_timeline_start(self->_anim_timeline);
        },
        this);
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

lv_obj_t* DashboardView::topInfoCreate(lv_obj_t* par)
{
    lv_obj_t* cont = lv_obj_create(par);
    {
        lv_obj_remove_style_all(cont);
        lv_obj_set_size(cont, lv_pct(100), 135);

        lv_obj_set_style_bg_opa(cont, LV_OPA_COVER, 0);
        lv_obj_set_style_bg_color(cont, lv_color_hex(0x333333), 0);

        lv_obj_set_style_radius(cont, 27, 0);
        lv_obj_align(cont, LV_ALIGN_TOP_MID, 0, -27);

        lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(
            cont,
            LV_FLEX_ALIGN_END,
            LV_FLEX_ALIGN_CENTER,
            LV_FLEX_ALIGN_CENTER);
    }

    {
        lv_obj_t* label = lv_label_create(cont);
        lv_obj_set_style_text_font(label, _fontLarge, 0);
        lv_obj_set_style_text_color(label, lv_color_white(), 0);
        lv_obj_set_style_translate_y(label, 10, 0);
        lv_label_set_text_static(label, "00");

        subscribe(
            MSG_ID::SPORT_STATUS,
            label,
            [](lv_event_t* e) {
                auto msg = lv_event_get_msg(e);
                auto info = (const DataProc::SportStatus_Info_t*)lv_msg_get_payload(msg);
                auto obj = lv_event_get_current_target_obj(e);
                lv_label_set_text_fmt(obj, "%02d", (int)info->speedKph);
            });
    }

    {
        lv_obj_t* label = lv_label_create(cont);
        lv_obj_set_style_text_color(label, lv_color_white(), 0);
        lv_obj_set_style_translate_y(label, -5, 0);
        lv_label_set_text_static(label, "km/h");
    }

    return cont;
}

lv_obj_t* DashboardView::bottomInfoCreate(lv_obj_t* par)
{
    lv_obj_t* cont = lv_obj_create(par);
    lv_obj_remove_style_all(cont);
    lv_obj_set_size(cont, lv_pct(100), 160);
    lv_obj_align(cont, LV_ALIGN_TOP_MID, 0, 110);

    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(
        cont,
        LV_FLEX_ALIGN_SPACE_EVENLY,
        LV_FLEX_ALIGN_CENTER,
        LV_FLEX_ALIGN_SPACE_EVENLY);

    {
        lv_obj_t* label = infoItemCreate(cont, _("AVG_SPEED"));
        subscribe(
            MSG_ID::SPORT_STATUS,
            label,
            [](lv_event_t* e) {
                auto msg = lv_event_get_msg(e);
                auto info = (const DataProc::SportStatus_Info_t*)lv_msg_get_payload(msg);
                auto obj = lv_event_get_current_target_obj(e);
                lv_label_set_text_fmt(obj, "%0.1f km/h", info->speedAvgKph);
            });
    }

    {
        lv_obj_t* label = infoItemCreate(cont, _("TIME"));
        subscribe(
            MSG_ID::SPORT_STATUS,
            label,
            [](lv_event_t* e) {
                auto msg = lv_event_get_msg(e);
                auto info = (const DataProc::SportStatus_Info_t*)lv_msg_get_payload(msg);
                auto obj = lv_event_get_current_target_obj(e);
                lv_label_set_text_fmt(obj, "%s", makeTimeString(info->singleTime));
            });
    }

    {
        lv_obj_t* label = infoItemCreate(cont, _("DISTANCE"));
        subscribe(
            MSG_ID::SPORT_STATUS,
            label,
            [](lv_event_t* e) {
                auto msg = lv_event_get_msg(e);
                auto info = (const DataProc::SportStatus_Info_t*)lv_msg_get_payload(msg);
                auto obj = lv_event_get_current_target_obj(e);
                lv_label_set_text_fmt(obj, "%0.1f km", info->singleDistance / 1000.0f);
            });
    }

    {
        lv_obj_t* label = infoItemCreate(cont, _("ALTITUDE"));
        subscribe(
            MSG_ID::SPORT_STATUS,
            label,
            [](lv_event_t* e) {
                auto msg = lv_event_get_msg(e);
                auto info = (const DataProc::SportStatus_Info_t*)lv_msg_get_payload(msg);
                auto obj = lv_event_get_current_target_obj(e);
                lv_label_set_text_fmt(obj, "%0.1f m", info->altitude);
            });
    }

    {
        lv_obj_t* label = infoItemCreate(cont, _("CALORIES"));
        subscribe(
            MSG_ID::SPORT_STATUS,
            label,
            [](lv_event_t* e) {
                auto msg = lv_event_get_msg(e);
                auto info = (const DataProc::SportStatus_Info_t*)lv_msg_get_payload(msg);
                auto obj = lv_event_get_current_target_obj(e);
                lv_label_set_text_fmt(obj, "%d k", (int)info->singleCalorie);
            });
    }

    {
        lv_obj_t* label = infoItemCreate(cont, _("COURSE"));
        subscribe(
            MSG_ID::GNSS_STATUS,
            label,
            [](lv_event_t* e) {
                auto msg = lv_event_get_msg(e);
                auto info = (const HAL::GNSS_Info_t*)lv_msg_get_payload(msg);
                auto obj = lv_event_get_current_target_obj(e);
                lv_label_set_text_fmt(obj, "%s", Geo::cardinal(info->course));
            });
    }

    return cont;
}

lv_obj_t* DashboardView::infoItemCreate(lv_obj_t* par, const char* title)
{
    lv_obj_t* cont = lv_obj_create(par);
    lv_obj_remove_style_all(cont);
    lv_obj_remove_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(cont, 100, 39);

    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(
        cont,
        LV_FLEX_ALIGN_SPACE_AROUND,
        LV_FLEX_ALIGN_CENTER,
        LV_FLEX_ALIGN_CENTER);

    lv_obj_t* labelValue = lv_label_create(cont);
    lv_label_set_text_static(labelValue, "-");

    {
        lv_obj_t* label = lv_label_create(cont);
        lv_obj_set_style_text_color(label, lv_color_hex(0xb3b3b3), 0);
        lv_label_set_text(label, title);
    }

    return labelValue;
}

lv_obj_t* DashboardView::btnGroupCreate(lv_obj_t* par)
{
    lv_obj_t* cont = lv_obj_create(par);
    lv_obj_remove_style_all(cont);
    lv_obj_set_size(cont, lv_pct(100), 40);
    lv_obj_align(cont, LV_ALIGN_BOTTOM_MID, 0, -10);

    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(
        cont,
        LV_FLEX_ALIGN_SPACE_AROUND,
        LV_FLEX_ALIGN_CENTER,
        LV_FLEX_ALIGN_CENTER);

    btnCreate(cont, LV_SYMBOL_EXT_LOCATION_DOT, "LiveMap");
    btnCreate(cont, LV_SYMBOL_EXT_POWER_OFF, "Shutdown");
    btnCreate(cont, LV_SYMBOL_EXT_BARS, "SystemInfos");

    return cont;
}

lv_obj_t* DashboardView::btnCreate(lv_obj_t* par, const char* symbol, const char* pageID)
{
    lv_obj_t* btn = lv_obj_create(par);
    lv_obj_remove_style_all(btn);
    lv_obj_set_size(btn, 40, 31);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t* label = lv_label_create(btn);
    lv_label_set_text_static(label, symbol);
    lv_obj_center(label);

    lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, 0);
    lv_obj_set_style_transform_width(btn, 5, LV_STATE_PRESSED);
    lv_obj_set_style_transform_height(btn, -5, LV_STATE_PRESSED);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x666666), 0);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0xbbbbbb), LV_STATE_PRESSED);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0xff931e), LV_STATE_FOCUSED);
    lv_obj_set_style_radius(btn, 9, 0);

    static lv_style_transition_dsc_t tran;
    static const lv_style_prop_t prop[] = {
        LV_STYLE_TRANSFORM_WIDTH,
        LV_STYLE_TRANSFORM_HEIGHT,
        LV_STYLE_PROP_INV
    };
    lv_style_transition_dsc_init(
        &tran,
        prop,
        lv_anim_path_ease_out,
        200,
        0,
        nullptr);
    lv_obj_set_style_transition(btn, &tran, LV_STATE_PRESSED);
    lv_obj_set_style_transition(btn, &tran, LV_STATE_FOCUSED);

    if (pageID) {
        lv_obj_set_user_data(btn, (void*)pageID);
        lv_obj_add_event_cb(
            btn,
            [](lv_event_t* e) {
                auto self = (DashboardView*)lv_event_get_user_data(e);
                auto id = lv_obj_get_user_data(lv_event_get_current_target_obj(e));
                self->_listener->onViewEvent(EVENT_ID::NAVI_TO_PAGE, id);
            },
            LV_EVENT_CLICKED,
            this);
    }

    return btn;
}

const char* DashboardView::makeTimeString(uint64_t ms)
{
    static char buf[16];
    uint64_t ss = ms / 1000;
    uint64_t mm = ss / 60;
    uint32_t hh = (uint32_t)(mm / 60);

    if (hh < 100) {
        lv_snprintf(
            buf, sizeof(buf),
            "%d:%02d:%02d",
            (int)hh,
            (int)(mm % 60),
            (int)(ss % 60));
    } else {
        lv_snprintf(
            buf, sizeof(buf),
            "%d:%02d",
            (int)hh,
            (int)(mm % 60));
    }

    return buf;
}
