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
#include "LiveMapView.h"
#include "Service/DataProc/DataProc_Def.h"
#include "Utils/TrackView/TrackView.h"

#define MAP_MOVE_START_STATE LV_STATE_USER_1
#define SPEED_OVER_STATE LV_STATE_USER_1

using namespace Page;

LiveMapView::LiveMapView(EventListener* listener, lv_obj_t* root)
    : _listener(listener)
    , _fontSpeed(32, "medium")
    , _mapView(nullptr)
    , _roadMapView(nullptr)
    , _trackView(nullptr)
    , _mapConv(true, 16)
{
    lv_obj_set_style_pad_all(root, 0, 0);

    /* label loading */
    {
        lv_obj_t* label = lv_label_create(root);
        lv_obj_center(label);
        lv_label_set_text(label, _("LOADING..."));

        subscribe(MSG_ID::GEO_COORD_UPDATE, label, [](lv_event_t* e) {
            auto obj = lv_event_get_target_obj(e);
            lv_obj_del(obj);
        });
    }

    styleInit();
    mapViewCreate(root);
    sportInfoCreate(root);
}

LiveMapView::~LiveMapView()
{
    delete _roadMapView;
    delete _trackView;
    delete _mapView;
}

lv_uintptr_t LiveMapView::msgID(MSG_ID id)
{
    return (lv_uintptr_t)this + (lv_uintptr_t)id;
}

void LiveMapView::publish(MSG_ID id, const void* payload)
{
    lv_msg_send(msgID(id), payload);
}

void LiveMapView::subscribe(MSG_ID id, lv_obj_t* obj, lv_event_cb_t event_cb)
{
    lv_msg_subscribe_obj(msgID(id), obj, this);
    lv_obj_add_event(obj, event_cb, LV_EVENT_MSG_RECEIVED, this);
}

void LiveMapView::styleInit()
{
    /* cont style */
    lv_style_set_pad_all(&_style.cont, 0);
    lv_style_set_border_width(&_style.cont, 0);
    lv_style_set_bg_opa(&_style.cont, LV_OPA_60);
    lv_style_set_radius(&_style.cont, 6);
    lv_style_set_shadow_width(&_style.cont, 10);
    lv_style_set_shadow_color(&_style.cont, lv_color_black());

    /* line style */
    lv_style_set_line_color(&_style.line, lv_color_hex(0xff931e));
    lv_style_set_line_width(&_style.line, 5);
    lv_style_set_line_opa(&_style.line, LV_OPA_COVER);
    lv_style_set_line_rounded(&_style.line, true);
}

void LiveMapView::mapViewCreate(lv_obj_t* par)
{
    /* update view size */
    lv_obj_update_layout(par);
    lv_coord_t view_w = lv_obj_get_width(par);
    lv_coord_t view_h = lv_obj_get_height(par);

    /* map viewer */
    _mapView = new MapView(view_w, view_h, par, onMapViewEvent, this);
    _mapView->setScrollEanble(false);
    _mapView->setTileSrcEanble(false);
    lv_obj_t* viewCont = _mapView->getViewCont();

    /* geo coord update */
    subscribe(MSG_ID::GEO_COORD_UPDATE, viewCont, [](lv_event_t* e) {
        auto msg = lv_event_get_msg(e);
        auto self = (LiveMapView*)lv_event_get_user_data(e);

        if (self->msgID(MSG_ID::GEO_COORD_UPDATE) != lv_msg_get_id(msg)) {
            return;
        }

        auto geoCoord = (const MapView::GeoCoord_t*)lv_msg_get_payload(msg);
        self->_mapView->setGeoCoord(geoCoord);
    });

    /* set map config */
    subscribe(MSG_ID::MAP_CONFIG_UPDATE, viewCont, [](lv_event_t* e) {
        auto msg = lv_event_get_msg(e);
        auto self = (LiveMapView*)lv_event_get_user_data(e);

        if (self->msgID(MSG_ID::MAP_CONFIG_UPDATE) != lv_msg_get_id(msg)) {
            return;
        }

        auto mapConfig = (const MapView::MapConfig_t*)lv_msg_get_payload(msg);
        self->_mapView->setConfig(mapConfig);
    });

    /* set map level */
    subscribe(MSG_ID::MAP_SET_LEVEL, viewCont, [](lv_event_t* e) {
        auto msg = lv_event_get_msg(e);
        auto self = (LiveMapView*)lv_event_get_user_data(e);

        if (self->msgID(MSG_ID::MAP_SET_LEVEL) != lv_msg_get_id(msg)) {
            return;
        }

        auto level = (const int*)lv_msg_get_payload(msg);
        self->_mapView->setLevel(*level);
    });

    /* track */
    trackCreate(viewCont);

    /* img arrow */
    _mapView->addArrowImage(ResourcePool::getImage("navi_arrow_light"));
}

void LiveMapView::onMapViewEvent(MapView::EVENT_ID id, const void* param, void* userData)
{
    auto self = (LiveMapView*)userData;

    switch (id) {
    case MapView::EVENT_ID::LEVEL_CHANGED:
        self->_listener->onViewEvent(EVENT_ID::MAP_LEVEL_CHANGED, param);
        break;

    case MapView::EVENT_ID::TILE_RECT_CHANGED:
        self->publish(MSG_ID::MAP_TILE_RECT_CHANGED, param);
        break;

    default:
        break;
    }
}

void LiveMapView::sportInfoCreate(lv_obj_t* par)
{
    /* cont */
    lv_obj_t* cont = lv_obj_create(par);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(cont, &_style.cont, 0);
    lv_obj_set_size(cont, 160, 66);
    lv_obj_align(cont, LV_ALIGN_BOTTOM_LEFT, -10, 10);
    lv_obj_set_style_radius(cont, 10, 0);
    lv_obj_set_style_translate_y(cont, 180, MAP_MOVE_START_STATE);

    static const lv_style_prop_t prop[] = {
        LV_STYLE_TRANSLATE_Y,
        LV_STYLE_PROP_INV
    };
    static lv_style_transition_dsc_t tran;
    lv_style_transition_dsc_init(&tran, prop, lv_anim_path_ease_out, 300, 0, nullptr);
    lv_obj_set_style_transition(cont, &tran, LV_STATE_DEFAULT);

    /* speed */
    lv_obj_t* labelSpeed = lv_label_create(cont);
    {
        lv_label_set_text(labelSpeed, "00");
        lv_obj_set_style_text_font(labelSpeed, _fontSpeed, 0);
        lv_obj_align(labelSpeed, LV_ALIGN_LEFT_MID, 20, -10);
        lv_obj_set_style_translate_x(labelSpeed, -8, SPEED_OVER_STATE);

        subscribe(MSG_ID::SPORT_STATUS, labelSpeed, [](lv_event_t* e) {
            auto obj = lv_event_get_current_target_obj(e);
            auto msg = lv_event_get_msg(e);
            auto info = (const DataProc::SportStatus_Info_t*)lv_msg_get_payload(msg);
            auto speed = (int)info->speedKph;
            lv_label_set_text_fmt(obj, "%02d", speed);

            if (speed >= 100) {
                lv_obj_add_state(obj, SPEED_OVER_STATE);
            } else {
                lv_obj_clear_state(obj, SPEED_OVER_STATE);
            }
        });
    }

    /* unit */
    {
        lv_obj_t* labelUnit = lv_label_create(cont);
        lv_label_set_text(labelUnit, "km/h");
        lv_obj_set_style_text_font(labelUnit, lv_theme_get_font_small(labelUnit), 0);
        lv_obj_align_to(labelUnit, labelSpeed, LV_ALIGN_OUT_BOTTOM_MID, 0, -8);
    }

    /* trip dist */
    {
        lv_obj_t* label = lv_label_create(cont);
        lv_label_set_text(label, LV_SYMBOL_EXT_ROAD " --");
        lv_obj_align(label, LV_ALIGN_TOP_LEFT, 73, 10);

        subscribe(MSG_ID::SPORT_STATUS, label, [](lv_event_t* e) {
            auto obj = lv_event_get_current_target_obj(e);
            auto msg = lv_event_get_msg(e);
            auto info = (const DataProc::SportStatus_Info_t*)lv_msg_get_payload(msg);
            auto dist = info->singleDistance / 1000.0f;
            if (dist < 100) {
                lv_label_set_text_fmt(obj, LV_SYMBOL_EXT_ROAD " %0.1f km", dist);
            } else {
                lv_label_set_text_fmt(obj, LV_SYMBOL_EXT_ROAD " %d km", (int)dist);
            }
        });
    }

    /* trip time */
    {
        lv_obj_t* label = lv_label_create(cont);
        lv_label_set_text(label, LV_SYMBOL_EXT_CLOCK "--");
        lv_obj_align(label, LV_ALIGN_TOP_LEFT, 74, 30);

        subscribe(MSG_ID::SPORT_STATUS, label, [](lv_event_t* e) {
            auto obj = lv_event_get_target_obj(e);
            auto msg = lv_event_get_msg(e);
            auto info = (const DataProc::SportStatus_Info_t*)lv_msg_get_payload(msg);

            uint64_t ss = info->singleTime / 1000;
            uint64_t mm = ss / 60;
            uint32_t hh = (uint32_t)(mm / 60);

            lv_label_set_text_fmt(obj,
                LV_SYMBOL_EXT_CLOCK " %d:%02d:%02d",
                (int)hh,
                (int)(mm % 60),
                (int)(ss % 60));
        });
    }
}

void LiveMapView::trackCreate(lv_obj_t* par)
{
    _trackView = new TrackView(par);
    _trackView->setStyle(&_style.line);

    lv_obj_t* cont = _trackView->getViewCont();

    /* track recorder state change */
    subscribe(MSG_ID::TRACK_REC, cont, [](lv_event_t* e) {
        auto msg = lv_event_get_msg(e);
        auto self = (LiveMapView*)lv_event_get_user_data(e);

        if (self->msgID(MSG_ID::TRACK_REC) != lv_msg_get_id(msg)) {
            return;
        }

        auto info = (const DataProc::TrackFilter_Info_t*)lv_msg_get_payload(msg);
        self->_trackView->setPointContainer((PointContainer*)info->pointCont, info->level);
        self->_trackView->reload();
    });

    /* map tile change */
    subscribe(MSG_ID::MAP_TILE_RECT_CHANGED, cont, [](lv_event_t* e) {
        auto msg = lv_event_get_msg(e);
        auto self = (LiveMapView*)lv_event_get_user_data(e);

        if (self->msgID(MSG_ID::MAP_TILE_RECT_CHANGED) != lv_msg_get_id(msg)) {
            return;
        }

        auto info = (const TileView::Rect_t*)lv_msg_get_payload(msg);
        self->_trackView->setArea(info->x, info->y, info->x + info->width - 1, info->y + info->height - 1);
        self->_trackView->reload();
    });

    /* map level change */
    subscribe(MSG_ID::MAP_SET_LEVEL, cont, [](lv_event_t* e) {
        auto msg = lv_event_get_msg(e);
        auto self = (LiveMapView*)lv_event_get_user_data(e);

        if (self->msgID(MSG_ID::MAP_SET_LEVEL) != lv_msg_get_id(msg)) {
            return;
        }

        auto level = *(const int*)lv_msg_get_payload(msg);
        self->_trackView->setLevel(level);
        self->_trackView->reload();
    });

    /* geo coord update */
    subscribe(MSG_ID::GEO_COORD_UPDATE, cont, [](lv_event_t* e) {
        auto msg = lv_event_get_msg(e);
        auto self = (LiveMapView*)lv_event_get_user_data(e);

        if (self->msgID(MSG_ID::GEO_COORD_UPDATE) != lv_msg_get_id(msg)) {
            return;
        }

        auto geoCoord = (const MapView::GeoCoord_t*)lv_msg_get_payload(msg);

        /* sync status */
        self->_mapConv.setCoordTransformEnable(self->_mapView->getCoordTrans());
        self->_mapConv.setLevel(self->_trackView->getRefLevel());

        /* convert coord */
        MapConv::Point_t point = self->_mapConv.getCoordinate(geoCoord->longitude, geoCoord->latitude);

        /* push point */
        self->_trackView->pushPoint(point.x, point.y);
        self->_trackView->setActivePoint(point.x, point.y);
    });
}
