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

#define ITEM_HEIGHT_MIN 100

SystemInfosView::SystemInfosView(EventListener* listener, lv_obj_t* root)
    : _listener(listener)
    , _fontAwesome(40, "awesome")
#define BINDING_DEF(name, type) , _binding##name((Binding<type, SystemInfosModel>*)getBinding(BINDING_TYPE::name))
#include "BindingDef.inc"
#undef BINDING_DEF
{
    /* Ensure that MSG_ID is unique */
    static_assert(sizeof(SystemInfosView) >= (size_t)MSG_ID::_LAST, "Large MSG_ID");

    rootInit(root);

    styleInit();

    itemGroupCreate(root);

    onRootScroll(root);
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

void SystemInfosView::rootInit(lv_obj_t* root)
{
    lv_obj_update_layout(root);
    lv_obj_set_style_pad_ver(root, ((lv_obj_get_height(root) - ITEM_HEIGHT_MIN) / 2), 0);

    lv_obj_set_flex_flow(root, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(
        root,
        LV_FLEX_ALIGN_START,
        LV_FLEX_ALIGN_START,
        LV_FLEX_ALIGN_CENTER);

    lv_obj_add_event_cb(
        root,
        [](lv_event_t* e) {
            onRootScroll(lv_event_get_current_target_obj(e));
        },
        LV_EVENT_SCROLL,
        this);

    lv_obj_add_flag(root, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scroll_snap_y(root, LV_SCROLL_SNAP_CENTER);
    lv_obj_set_scrollbar_mode(root, LV_SCROLLBAR_MODE_OFF);
}

void SystemInfosView::onRootScroll(lv_obj_t* obj)
{
    auto child_cnt = lv_obj_get_child_count(obj);
    auto obj_height = lv_obj_get_height(obj);
    auto obj_pad_top = lv_obj_get_style_pad_top(obj, 0);
    auto scroll_y = lv_obj_get_scroll_y(obj);

    for (uint32_t i = 0; i < child_cnt; i++) {
        auto cont = lv_obj_get_child(obj, i);

        /* skip 2 label obj */
        auto icon = lv_obj_get_child(cont, 2);

        auto cont_height = lv_obj_get_height(cont);
        auto cont_y = lv_obj_get_y(cont);
        auto offset_y = (obj_height / 2.0f + scroll_y) - (obj_pad_top + cont_y + cont_height / 2.0f);

        lv_anim_t a;
        lv_anim_init(&a);
        a.duration = cont_height;
        a.act_time = LV_ABS(offset_y);
        a.start_value = 70;
        a.end_value = 220;
        auto width = lv_anim_path_ease_out(&a);

        a.duration = cont_height / 2;
        a.start_value = LV_OPA_COVER;
        a.end_value = LV_OPA_TRANSP;
        auto opa = lv_anim_path_ease_out(&a);

        // LV_LOG_USER("index: %d, icon: %p, offset_y: %d, width: %d", i, icon, (int)offset_y, width);
        lv_obj_set_width(icon, width);
        lv_obj_set_style_border_opa(icon, opa, 0);
    }
}

void SystemInfosView::styleInit()
{
    lv_style_set_pad_all(&_style.icon, 0);
    lv_style_set_border_width(&_style.icon, 0);
    lv_style_set_radius(&_style.icon, 0);
    lv_style_set_width(&_style.icon, 220);
    lv_style_set_border_side(&_style.icon, LV_BORDER_SIDE_RIGHT);
    lv_style_set_border_width(&_style.icon, 2);
    lv_style_set_border_color(&_style.icon, lv_color_hex(0xff931e));
    lv_style_set_text_font(&_style.icon, lv_theme_get_font_normal(nullptr));
    lv_style_set_text_color(&_style.icon, lv_color_white());

    lv_style_set_text_font(&_style.info, lv_theme_get_font_small(nullptr));
}

void SystemInfosView::itemGroupCreate(lv_obj_t* par)
{
    char infoBuf[128];

    /* Sport */
    {
        lv_snprintf(infoBuf, sizeof(infoBuf),
            "%s\n%s\n%s",
            _("TOTAL_TRIP"), _("TOTAL_TIME"), _("MAX_SPEED"));
        lv_obj_t* label = itemCreate(
            par,
            _("SPORT"),
            LV_SYMBOL_EXT_BICYCLE,
            infoBuf);

        subscribe(
            MSG_ID::SPORT_STATUS,
            label,
            [](lv_event_t* e) {
                auto obj = lv_event_get_current_target_obj(e);
                auto msg = lv_event_get_msg(e);
                auto info = (const DataProc::SportStatus_Info_t*)lv_msg_get_payload(msg);
                lv_label_set_text_fmt(
                    obj,
                    "%0.2fkm\n"
                    "%s\n"
                    "%0.1fkm/h",
                    info->totalDistance / 1000.0f,
                    makeTimeString(info->totalTime),
                    info->speedMaxKph);
            });
    }

    {
        lv_snprintf(infoBuf, sizeof(infoBuf),
            "%s\n%s\n%s\n%s\n\n%s\n%s",
            _("LATITUDE"),
            _("LONGITUDE"),
            _("ALTITUDE"),
            _("UTC_TIME"),
            _("COURSE"),
            _("SPEED"));
        lv_obj_t* label = itemCreate(
            par,
            _("GNSS"),
            LV_SYMBOL_EXT_SATELLITE,
            infoBuf);

        subscribe(
            MSG_ID::GNSS,
            label,
            [](lv_event_t* e) {
                auto obj = lv_event_get_current_target_obj(e);
                auto msg = lv_event_get_msg(e);
                auto info = (const HAL::GNSS_Info_t*)lv_msg_get_payload(msg);
                lv_label_set_text_fmt(
                    obj,
                    "%0.6f\n"
                    "%0.6f\n"
                    "%0.2fm\n"
                    "%s\n"
                    "%0.1f deg\n"
                    "%0.1fkm/h",
                    info->latitude,
                    info->longitude,
                    info->altitude,
                    makeTimeString(&info->clock),
                    info->course,
                    info->speed);
            });
    }

    {
        lv_snprintf(infoBuf, sizeof(infoBuf),
            "%s\n%s",
            _("DATE"),
            _("TIME"));
        lv_obj_t* label = itemCreate(
            par,
            _("TIME"),
            LV_SYMBOL_EXT_CLOCK,
            infoBuf);

        subscribe(
            MSG_ID::CLOCK,
            label,
            [](lv_event_t* e) {
                auto obj = lv_event_get_current_target_obj(e);
                auto msg = lv_event_get_msg(e);
                auto info = (const HAL::Clock_Info_t*)lv_msg_get_payload(msg);
                lv_label_set_text_fmt(
                    obj,
                    "%s",
                    makeTimeString(info));
            });
    }

    {
        lv_snprintf(infoBuf, sizeof(infoBuf),
            "%s\n%s\n%s",
            _("USAGE"),
            _("VOLTAGE"),
            _("STATUS"));
        lv_obj_t* label = itemCreate(
            par,
            _("BATTERY"),
            LV_SYMBOL_EXT_BATTERY_THREE_QUARTERS,
            infoBuf);

        subscribe(
            MSG_ID::POWER,
            label,
            [](lv_event_t* e) {
                auto obj = lv_event_get_current_target_obj(e);
                auto msg = lv_event_get_msg(e);
                auto info = (const DataProc::Power_Info_t*)lv_msg_get_payload(msg);
                lv_label_set_text_fmt(
                    obj,
                    "%d%%\n"
                    "%0.2fV\n"
                    "%s",
                    info->level,
                    info->voltage / 1000.0f,
                    info->isCharging ? _("CHARGE") : _("DISCHARGE"));
            });
    }

    {
        lv_snprintf(infoBuf, sizeof(infoBuf),
            "%s\n%s\n%s\n%s\n%s",
            _("NAME"),
            _("AUTHOR"),
            _("GRAPHICS"),
            _("COMPILER"),
            _("BUILD_DATE"));
        lv_obj_t* label = itemCreate(
            par,
            _("ABOUT"),
            LV_SYMBOL_EXT_ADDRESS_CARD,
            infoBuf);

        auto info = _bindingVerison->get();

        lv_label_set_text_fmt(
            label,
            "%s\n"
            "%s\n"
            "%s\n"
            "%s\n"
            "%s",
            info.name,
            info.author,
            info.graphics,
            info.compiler,
            info.buildDate);
    }
}

lv_obj_t* SystemInfosView::itemCreate(
    lv_obj_t* par,
    const char* name,
    const char* symbol,
    const char* infos)
{
    lv_obj_t* cont = lv_obj_create(par);
    lv_obj_enable_style_refresh(false);
    lv_obj_remove_style_all(cont);
    lv_obj_set_width(cont, 220);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);

    /* icon */
    lv_obj_t* icon = lv_obj_create(cont);
    lv_obj_enable_style_refresh(false);
    lv_obj_clear_flag(icon, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(icon, LV_OBJ_FLAG_SCROLL_ON_FOCUS);

    lv_obj_add_style(icon, &_style.icon, 0);
    lv_obj_set_style_align(icon, LV_ALIGN_LEFT_MID, 0);

    lv_obj_set_flex_flow(icon, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(
        icon,
        LV_FLEX_ALIGN_SPACE_AROUND,
        LV_FLEX_ALIGN_CENTER,
        LV_FLEX_ALIGN_CENTER);

    {
        lv_obj_t* label = lv_label_create(icon);
        lv_obj_enable_style_refresh(false);
        lv_obj_set_style_text_font(label, _fontAwesome, 0);
        lv_label_set_text_static(label, symbol);
    }

    {
        lv_obj_t* label = lv_label_create(icon);
        lv_obj_enable_style_refresh(false);
        lv_label_set_text(label, name);
    }

    /* infos */
    lv_obj_t* labelInfo = lv_label_create(cont);
    {
        lv_obj_enable_style_refresh(false);
        lv_label_set_text(labelInfo, infos);
        lv_obj_add_style(labelInfo, &_style.info, 0);
        lv_obj_align(labelInfo, LV_ALIGN_LEFT_MID, 75, 0);
    }

    /* datas */
    lv_obj_t* labelData = lv_label_create(cont);
    {
        lv_obj_enable_style_refresh(false);
        lv_label_set_text(labelData, "-");
        lv_obj_add_style(labelData, &_style.info, 0);
        lv_obj_align(labelData, LV_ALIGN_CENTER, 60, 0);
    }

    lv_obj_move_foreground(icon);
    lv_obj_enable_style_refresh(true);

    /* get real max height */
    lv_obj_update_layout(labelInfo);
    lv_coord_t height = lv_obj_get_height(labelInfo);
    height = LV_MAX(height, ITEM_HEIGHT_MIN);
    lv_obj_set_height(cont, height);
    lv_obj_set_height(icon, height);

    return labelData;
}

const char* SystemInfosView::makeTimeString(uint64_t ms)
{
    static char buf[16];
    uint64_t ss = ms / 1000;
    uint64_t mm = ss / 60;
    uint32_t hh = (uint32_t)(mm / 60);

    lv_snprintf(
        buf, sizeof(buf),
        "%d:%02d:%02d",
        (int)hh,
        (int)(mm % 60),
        (int)(ss % 60));

    return buf;
}

const char* SystemInfosView::makeTimeString(const HAL::Clock_Info_t* info)
{
    static char buf[32];
    lv_snprintf(
        buf,
        sizeof(buf),
        "%d-%d-%d\n%02d:%02d:%02d",
        info->year,
        info->month,
        info->day,
        info->hour,
        info->minute,
        info->second);

    return buf;
}
