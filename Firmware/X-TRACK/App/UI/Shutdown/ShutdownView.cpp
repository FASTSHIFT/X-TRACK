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
#include "ShutdownView.h"

using namespace Page;

ShutdownView::ShutdownView(EventListener* listener, lv_obj_t* root)
    : _listener(listener)
{
    lv_obj_add_event(
        root,
        [](lv_event_t* e) {
            auto self = (ShutdownView*)lv_event_get_user_data(e);
            self->_listener->onViewEvent(EVENT_ID::EXIT);
        },
        LV_EVENT_CLICKED,
        this);

    /* cont */
    lv_obj_t* cont = lv_obj_create(root);
    {
        lv_obj_set_size(cont, 60, lv_pct(50));

        lv_obj_center(cont);
        lv_obj_set_style_radius(cont, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_pad_hor(cont, 0, 0);
        lv_obj_set_style_pad_ver(cont, 28, 0);
        lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);

        lv_obj_t* label = lv_label_create(cont);
        lv_label_set_text_static(label, LV_SYMBOL_POWER);
        lv_obj_set_style_text_color(label, lv_palette_main(LV_PALETTE_RED), 0);
        lv_obj_align(label, LV_ALIGN_TOP_MID, 0, -10);
    }

    /* label */
    {
        lv_obj_t* label = lv_label_create(root);

        lv_label_set_text_static(label, _("POWER_OFF"));
        lv_obj_set_style_text_font(label, lv_theme_get_font_large(label), 0);
        lv_obj_align_to(label, cont, LV_ALIGN_OUT_TOP_MID, 0, -10);
    }

    /* slider */
    lv_obj_t* slider = lv_slider_create(cont);
    {
        lv_obj_set_style_bg_opa(slider, LV_OPA_TRANSP, 0);
        lv_obj_set_style_bg_opa(slider, LV_OPA_TRANSP, LV_PART_INDICATOR);
        lv_obj_set_size(slider, lv_pct(50), lv_pct(100));
        lv_obj_center(slider);

        lv_obj_set_style_anim_time(slider, 300, 0);
        lv_obj_set_style_bg_img_src(slider, LV_SYMBOL_UP, LV_PART_KNOB);
        lv_obj_set_style_text_color(slider, lv_color_white(), LV_PART_KNOB);

        lv_obj_add_event(
            slider,
            [](lv_event_t* e) {
                auto self = (ShutdownView*)lv_event_get_user_data(e);
                auto obj = lv_event_get_current_target_obj(e);
                auto value = lv_slider_get_value(obj);

                if (value == lv_slider_get_max_value(obj)) {
                    self->_listener->onViewEvent(EVENT_ID::SHUTDOWN, &value);
                } else {
                    lv_slider_set_value(obj, lv_slider_get_min_value(obj), LV_ANIM_ON);
                }
            },
            LV_EVENT_RELEASED, this);

        lv_obj_add_event(
            slider,
            [](lv_event_t* e) {
                auto self = (ShutdownView*)lv_event_get_user_data(e);
                auto obj = lv_event_get_current_target_obj(e);
                auto value = lv_slider_get_value(obj);

                if (value == lv_slider_get_max_value(obj)) {
                    self->_listener->onViewEvent(EVENT_ID::READY, &value);
                }
            },
            LV_EVENT_VALUE_CHANGED, this);
    }
}

ShutdownView::~ShutdownView()
{
}

lv_uintptr_t ShutdownView::msgID(MSG_ID id)
{
    return (lv_uintptr_t)this + (lv_uintptr_t)id;
}

void ShutdownView::publish(MSG_ID id, const void* payload)
{
    lv_msg_send(msgID(id), payload);
}

void ShutdownView::subscribe(MSG_ID id, lv_obj_t* obj, lv_event_cb_t event_cb)
{
    lv_msg_subscribe_obj(msgID(id), obj, this);
    lv_obj_add_event(obj, event_cb, LV_EVENT_MSG_RECEIVED, this);
}
