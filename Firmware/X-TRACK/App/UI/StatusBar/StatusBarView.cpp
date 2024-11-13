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
#include "StatusBarView.h"
#include "Service/DataProc/Def/DP_Power.h"
#include "Utils/lv_anim_label/lv_anim_label.h"

using namespace Page;

StatusBarView::StatusBarView(EventListener* listener, lv_obj_t* par)
    : _listener(listener)
    , _fontMedium(15, "medium")
    , _fontAwesome(15, "awesome")
    , _fontHandle { 0 }
    , _curState { 0 }
{
    /* Ensure that MSG_ID is unique */
    static_assert(sizeof(StatusBarView) >= (size_t)MSG_ID::_LAST, "Large MSG_ID");

    _curState.satellitesNum = -1;
    _curState.battLevel = -1;

    _fontHandle = *_fontMedium;
    _fontHandle.fallback = _fontAwesome;
    lv_obj_set_style_text_font(par, &_fontHandle, 0);

    lv_obj_t* cont = contCreate(par);
    satelliteCreate(cont);
    clockCreate(cont);
    recCreate(cont);
    batteryCreate(cont);

    lv_obj_update_layout(cont);
    int height = lv_obj_get_height(cont);
    lv_obj_set_y(cont, -height);
    lv_obj_set_style_translate_y(cont, height, LV_STATE_DEFAULT);
    lv_obj_set_style_translate_y(cont, 0, LV_STATE_DISABLED);
    _listener->onViewEvent(EVENT_ID::HEIGHT_UPDATE, &height);
}

StatusBarView::~StatusBarView()
{
}

lv_obj_t* StatusBarView::contCreate(lv_obj_t* par)
{
    lv_obj_t* cont = lv_obj_create(par);
    {
        lv_obj_remove_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_size(cont, lv_pct(100), LV_SIZE_CONTENT);
        lv_obj_set_style_pad_all(cont, 2, 0);
        lv_obj_set_style_radius(cont, 0, 0);
        lv_obj_set_style_border_width(cont, 0, 0);
        lv_obj_add_state(cont, LV_STATE_DISABLED);
    }

    /* style trans & cover */
    {
        auto dark_state = LV_STATE_USER_1;

        /* style trans */
        lv_obj_set_style_bg_opa(cont, LV_OPA_TRANSP, LV_STATE_DEFAULT);

        /* style cover */
        lv_obj_set_style_bg_color(cont, lv_color_black(), dark_state);
        lv_obj_set_style_bg_opa(cont, LV_OPA_60, dark_state);
        lv_obj_set_style_shadow_color(cont, lv_color_black(), dark_state);
        lv_obj_set_style_shadow_width(cont, 10, dark_state);

        static lv_style_transition_dsc_t tran;
        static const lv_style_prop_t prop[] = {
            LV_STYLE_SHADOW_WIDTH,
            LV_STYLE_BG_OPA,
            LV_STYLE_PROP_INV
        };
        lv_style_transition_dsc_init(
            &tran,
            prop,
            lv_anim_path_ease_out,
            200,
            0,
            nullptr);
        lv_obj_set_style_transition(cont, &tran, dark_state);
        lv_obj_set_style_transition(cont, &tran, LV_STATE_DEFAULT);

        subscribe(MSG_ID::STYLE_CHANGE, cont, [](lv_event_t* e) {
            auto obj = lv_event_get_current_target_obj(e);
            auto msg = lv_event_get_msg(e);
            auto self = (StatusBarView*)lv_msg_get_user_data(msg);

            if (lv_msg_get_id(msg) != self->msgID(MSG_ID::STYLE_CHANGE)) {
                return;
            }

            auto style = *(STYLE_ID*)lv_msg_get_payload(msg);
            if (style == STYLE_ID::LIGHT) {
                lv_obj_add_state(obj, LV_STATE_USER_1);
            } else {
                lv_obj_clear_state(obj, LV_STATE_USER_1);
            }
        });
    }

    /* style show & hide */
    {
        static lv_style_transition_dsc_t tran;
        static const lv_style_prop_t prop[] = {
            LV_STYLE_TRANSLATE_Y,
            LV_STYLE_PROP_INV
        };
        lv_style_transition_dsc_init(
            &tran,
            prop,
            lv_anim_path_ease_out,
            300,
            0,
            nullptr);
        lv_obj_set_style_transition(cont, &tran, LV_STATE_DEFAULT);
        lv_obj_set_style_transition(cont, &tran, LV_STATE_DISABLED);

        subscribe(MSG_ID::SHOW, cont, [](lv_event_t* e) {
            auto obj = lv_event_get_current_target_obj(e);
            auto msg = lv_event_get_msg(e);
            auto self = (StatusBarView*)lv_msg_get_user_data(msg);

            if (lv_msg_get_id(msg) != self->msgID(MSG_ID::SHOW)) {
                return;
            }

            auto show = *(bool*)lv_msg_get_payload(msg);
            if (show) {
                lv_obj_clear_state(obj, LV_STATE_DISABLED);
            } else {
                lv_obj_add_state(obj, LV_STATE_DISABLED);
            }
        });
    }

    return cont;
}

void StatusBarView::satelliteCreate(lv_obj_t* par)
{
    {
        lv_obj_t* label = lv_label_create(par);
        lv_label_set_text_static(label, LV_SYMBOL_EXT_SATELLITE);
        lv_obj_align(label, LV_ALIGN_LEFT_MID, 5, 0);
    }

    {
        lv_obj_t* label = lv_label_create(par);
        lv_obj_align(label, LV_ALIGN_LEFT_MID, 25, 0);
        lv_label_set_text(label, "00");

        subscribe(MSG_ID::SAT_NUM, label, [](lv_event_t* e) {
            auto obj = lv_event_get_current_target_obj(e);
            auto msg = lv_event_get_msg(e);
            auto self = (StatusBarView*)lv_msg_get_user_data(msg);
            auto satnum = *(int16_t*)lv_msg_get_payload(msg);

            if (satnum == self->_curState.satellitesNum) {
                return;
            }

            lv_label_set_text_fmt(obj, "%02d", satnum);
            self->_curState.satellitesNum = satnum;
        });
    }

    int16_t satnum = 0;
    publish(MSG_ID::SAT_NUM, &satnum);
}

void StatusBarView::clockCreate(lv_obj_t* par)
{
    lv_obj_t* label = lv_label_create(par);
    {
        lv_obj_center(label);
        lv_label_set_text(label, "00:00");
    }

    subscribe(MSG_ID::CLOCK, label, [](lv_event_t* e) {
        auto obj = lv_event_get_current_target_obj(e);
        auto msg = lv_event_get_msg(e);
        auto hhmm = *(uint16_t*)lv_msg_get_payload(msg);
        auto self = (StatusBarView*)lv_msg_get_user_data(msg);

        int hour = hhmm >> 8;
        int min = hhmm & 0xFF;
        int sum = hour + min;

        if (sum == self->_curState.timeCheckSum) {
            return;
        }

        lv_label_set_text_fmt(obj, "%02d:%02d", hour, min);
        self->_curState.timeCheckSum = sum;
    });
}

void StatusBarView::batteryCreate(lv_obj_t* par)
{
    lv_obj_t* batteryLabel = lv_label_create(par);
    {
        lv_label_set_text_static(batteryLabel, LV_SYMBOL_EXT_BATTERY_EMPTY);
        lv_obj_align(batteryLabel, LV_ALIGN_RIGHT_MID, -30, 0);
    }

    /* battery bg */
    {
        lv_obj_t* bg = lv_obj_create(batteryLabel);
        lv_obj_remove_style_all(bg);
        lv_obj_set_size(bg, 10, 3);
        lv_obj_align(bg, LV_ALIGN_LEFT_MID, 3, -1);
        lv_obj_set_style_bg_color(bg, lv_color_white(), 0);
        lv_obj_set_style_bg_opa(bg, LV_OPA_COVER, 0);

        subscribe(MSG_ID::POWER, bg, [](lv_event_t* e) {
            auto obj = lv_event_get_current_target_obj(e);
            auto msg = lv_event_get_msg(e);
            auto self = (StatusBarView*)lv_msg_get_user_data(msg);

            if (lv_msg_get_id(msg) != self->msgID(MSG_ID::POWER)) {
                return;
            }

            auto info = (const DataProc::Power_Info_t*)lv_msg_get_payload(msg);

            if (info->level != self->_curState.battLevel) {
                lv_obj_set_width(obj, lv_map(info->level, 0, 100, 0, 10));
            }
        });
    }

    lv_obj_t* label = lv_label_create(par);
    {
        lv_obj_align(label, LV_ALIGN_RIGHT_MID, -5, 0);
        lv_label_set_text(label, "-");

        subscribe(MSG_ID::POWER, label, [](lv_event_t* e) {
            auto obj = lv_event_get_current_target_obj(e);
            auto msg = lv_event_get_msg(e);
            auto self = (StatusBarView*)lv_msg_get_user_data(msg);

            if (lv_msg_get_id(msg) != self->msgID(MSG_ID::POWER)) {
                return;
            }

            auto info = (const DataProc::Power_Info_t*)lv_msg_get_payload(msg);

            if (info->level == self->_curState.battLevel) {
                return;
            }

            lv_label_set_text_fmt(obj, "%02d", info->level);
            self->_curState.battLevel = info->level;
        });
    }
}

void StatusBarView::recCreate(lv_obj_t* par)
{
    lv_obj_t* alabel = lv_anim_label_create(par);
    lv_obj_set_size(alabel, 40, 20);
    lv_anim_label_set_enter_dir(alabel, LV_DIR_TOP);
    lv_anim_label_set_exit_dir(alabel, LV_DIR_BOTTOM);
    lv_anim_label_set_path(alabel, lv_anim_path_ease_out);
    lv_anim_label_set_time(alabel, 500);

    lv_obj_align(alabel, LV_ALIGN_RIGHT_MID, -50, 0);
    // lv_obj_set_style_border_color(alabel, lv_color_white(), 0);
    // lv_obj_set_style_border_width(alabel, 1, 0);

    lv_anim_t a_enter;
    lv_anim_init(&a_enter);
    lv_anim_set_early_apply(&a_enter, true);
    lv_anim_set_values(&a_enter, LV_OPA_TRANSP, LV_OPA_COVER);
    lv_anim_set_exec_cb(&a_enter, [](void* var, int32_t v) {
        lv_obj_set_style_opa((lv_obj_t*)var, v, 0);
    });
    lv_anim_set_time(&a_enter, 300);

    lv_anim_t a_exit = a_enter;
    lv_anim_set_values(&a_exit, LV_OPA_COVER, LV_OPA_TRANSP);

    lv_anim_label_set_custom_enter_anim(alabel, &a_enter);
    lv_anim_label_set_custom_exit_anim(alabel, &a_exit);

    subscribe(MSG_ID::REC_CHANGE, alabel, [](lv_event_t* e) {
        auto obj = lv_event_get_current_target_obj(e);
        auto msg = lv_event_get_msg(e);
        auto str = (const char*)lv_msg_get_payload(msg);
        lv_anim_label_push_text(obj, str);
    });
}

lv_uintptr_t StatusBarView::msgID(MSG_ID id)
{
    return (lv_uintptr_t)this + (lv_uintptr_t)id;
}

void StatusBarView::publish(MSG_ID id, const void* payload)
{
    lv_msg_send(msgID(id), payload);
}

void StatusBarView::subscribe(MSG_ID id, lv_obj_t* obj, lv_event_cb_t event_cb)
{
    lv_msg_subscribe_obj(msgID(id), obj, this);
    lv_obj_add_event(obj, event_cb, LV_EVENT_MSG_RECEIVED, this);
}
