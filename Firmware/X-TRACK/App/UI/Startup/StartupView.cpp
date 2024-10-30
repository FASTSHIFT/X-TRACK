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
#include "StartupView.h"
#include "Utils/lv_ext/lv_anim_timeline_wrapper.h"
#include "Utils/lv_ext/lv_ext_func.h"
#include "Version.h"

using namespace Page;

#define COLOR_ORANGE lv_color_hex(0xff931e)

StartupView::StartupView(EventListener* listener, lv_obj_t* root)
    : _listener(listener)
    , _font(36, "medium")
{
    static_assert(sizeof(StartupView) >= (size_t)MSG_ID::_LAST, "Large MSG_ID");
    int32_t contWidth = 0;
    int32_t contHeight = 0;

    lv_obj_t* contBottom = lv_obj_create(root);
    {
        lv_obj_remove_style_all(contBottom);
        lv_obj_clear_flag(contBottom, LV_OBJ_FLAG_SCROLLABLE);

        lv_obj_t* label = lv_label_create(contBottom);
        lv_obj_set_style_text_font(label, _font, 0);
        lv_label_set_text_static(label, VERSION_FIRMWARE_NAME);
        lv_obj_update_layout(label);

        contWidth = lv_obj_get_width(label);
        contHeight = lv_obj_get_height(label);
        lv_obj_set_size(contBottom, contWidth, contHeight);
        lv_obj_align_to(contBottom, root, LV_ALIGN_CENTER, 0, 0);
    }

    lv_obj_t* contTop = lv_obj_create(root);
    {
        lv_obj_remove_style_all(contTop);
        lv_obj_set_style_bg_color(contTop, COLOR_ORANGE, 0);
        lv_obj_set_style_bg_opa(contTop, LV_OPA_COVER, 0);
        lv_obj_set_size(contTop, contWidth, contHeight);
        lv_obj_clear_flag(contTop, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_align_to(contTop, root, LV_ALIGN_CENTER, 0, 0);
    }

    lv_obj_t* labelTop = lv_label_create(contTop);
    {
        lv_obj_set_style_text_font(labelTop, _font, 0);
        lv_obj_set_style_text_color(labelTop, lv_color_black(), 0);
        lv_obj_set_align(labelTop, LV_ALIGN_LEFT_MID);
        lv_label_set_text_static(labelTop, VERSION_FIRMWARE_NAME);
    }

    auto set_trans_x = [](void* obj, int32_t x) {
        lv_obj_set_style_translate_x((lv_obj_t*)obj, x, 0);
    };

    /* Since lv_anim checks exec_cb, set_w here is split into two functions */
    auto set_w = [](void* obj, int32_t w) {
        lv_obj_set_width((lv_obj_t*)obj, w);
    };

    _anim_timeline = lv_anim_timeline_create();

    auto anim_path = lv_anim_path_ease_out;

    /* LV_ANIM_EXEC is used to distinguish two function pointers
     * and bypass lv_anim's duplication check on exec_cb.
     */

    lv_anim_timeline_wrapper_t wrapper[] = {
        { 0, contTop, set_w, 0, contWidth, 500, anim_path, false },
        { 500, contTop, LV_ANIM_EXEC(width), contWidth, 0, 500, anim_path, false },
        { 500, contTop, set_trans_x, 0, contWidth, 500, anim_path, false },
        { 500, labelTop, set_trans_x, 0, -contWidth, 500, anim_path, false },
        { 500, contBottom, set_w, 0, contWidth, 500, anim_path, true },
        LV_ANIM_TIMELINE_WRAPPER_END
    };

    lv_anim_timeline_add_wrapper(_anim_timeline, wrapper);

    subscribe(MSG_ID::SHOW, contTop,
        [](lv_event_t* e) {
            auto self = (StartupView*)lv_event_get_user_data(e);
            auto msg = lv_event_get_msg(e);

            if (lv_msg_get_id(msg) != self->msgID(MSG_ID::SHOW)) {
                return;
            }

            auto show = (bool)(lv_uintptr_t)lv_msg_get_payload(msg);

            lv_anim_timeline_set_reverse(self->_anim_timeline, !show);
            lv_anim_timeline_start(self->_anim_timeline);
        });

    subscribe(MSG_ID::FADE_OUT, contBottom,
        [](lv_event_t* e) {
            auto self = (StartupView*)lv_event_get_user_data(e);
            auto obj = lv_event_get_current_target_obj(e);
            auto msg = lv_event_get_msg(e);

            if (lv_msg_get_id(msg) != self->msgID(MSG_ID::FADE_OUT)) {
                return;
            }

            auto param = (const FadeOutParam_t*)lv_msg_get_payload(msg);

            lv_obj_fade_out(obj, param->time, param->delay);
        });
}

StartupView::~StartupView()
{
    lv_anim_timeline_delete(_anim_timeline);
}

lv_uintptr_t StartupView::msgID(MSG_ID id)
{
    return (lv_uintptr_t)this + (lv_uintptr_t)id;
}

void StartupView::publish(MSG_ID id, const void* payload)
{
    lv_msg_send(msgID(id), payload);
}

void StartupView::subscribe(MSG_ID id, lv_obj_t* obj, lv_event_cb_t event_cb)
{
    lv_msg_subscribe_obj(msgID(id), obj, this);
    lv_obj_add_event(obj, event_cb, LV_EVENT_MSG_RECEIVED, this);
}
