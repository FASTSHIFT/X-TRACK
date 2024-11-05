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

    lv_obj_set_style_pad_all(root, 0, 0);

    lv_obj_t* cont = lv_obj_create(root);
    lv_obj_remove_style_all(cont);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_border_color(cont, COLOR_ORANGE, 0);
    lv_obj_set_style_border_side(cont, LV_BORDER_SIDE_BOTTOM, 0);
    lv_obj_set_style_border_width(cont, 3, 0);
    lv_obj_set_style_border_post(cont, true, 0);
    lv_obj_center(cont);

    lv_obj_t* label = lv_label_create(cont);
    lv_obj_set_style_text_font(label, _font, 0);
    lv_label_set_text_static(label, VERSION_FIRMWARE_NAME);

    lv_obj_update_layout(label);
    int32_t contWidth = lv_obj_get_width(label);
    int32_t contHeight = lv_obj_get_height(label);
    lv_obj_set_size(cont, contWidth, contHeight);

    _anim_timeline = lv_anim_timeline_create();

    auto lv_obj_set_opa = [](void* obj, int32_t v) {
        lv_obj_set_style_opa((lv_obj_t*)obj, v, 0);
    };

#define ANIM_DEF(start_time, obj, attr, start, end) \
    { start_time, obj, LV_ANIM_EXEC(attr), start, end, 500, lv_anim_path_ease_out, true }

    lv_anim_timeline_wrapper_t wrapper[] = {
        ANIM_DEF(0, cont, width, 0, contWidth),
        ANIM_DEF(500, label, y, contHeight, lv_obj_get_y(label)),
        ANIM_DEF(1500, cont, opa, LV_OPA_COVER, LV_OPA_TRANSP),
        LV_ANIM_TIMELINE_WRAPPER_END
    };

    lv_anim_timeline_add_wrapper(_anim_timeline, wrapper);

    subscribe(MSG_ID::SHOW, cont,
        [](lv_event_t* e) {
            auto self = (StartupView*)lv_event_get_user_data(e);
            auto msg = lv_event_get_msg(e);

            if (lv_msg_get_id(msg) != self->msgID(MSG_ID::SHOW)) {
                return;
            }

            auto show = (bool)(lv_uintptr_t)lv_msg_get_payload(msg);

            lv_anim_timeline_set_reverse(self->_anim_timeline, !show);
            if (!show) {
                /* set progress to max to hide the view */
                lv_anim_timeline_set_progress(self->_anim_timeline, LV_ANIM_TIMELINE_PROGRESS_MAX);
            }
            lv_anim_timeline_start(self->_anim_timeline);
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
