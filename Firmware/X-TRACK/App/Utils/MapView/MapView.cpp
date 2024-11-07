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

#include "MapView.h"
#include "Utils/lv_msg/lv_msg.h"
#include <string.h>

#define MAP_MOVED_STATE LV_STATE_USER_1
#define MAP_RESET_TIMEOUT (60 * 1000)

MapView::MapView(
    uint32_t width,
    uint32_t height,
    lv_obj_t* parent,
    EventListener* listener,
    void* userData)
    : _tileView(width, height)
    , _mapConv(true, 16)
    , _curGeoCoord { 0 }
    , _curLevel(0)
    , _config { 0 }
    , _viewCont(nullptr)
    , _timerMapReset(nullptr)
    , _lastShortClickTime(0)
    , _animBusy(false)
    , _animStart { 0 }
    , _animEnd { 0 }
    , _tileSrcEanble(true)
    , _listener(listener)
    , _userData(userData)
{
    _tileView.setFocusPos(0, 0);

    lv_obj_t* viewCont = lv_obj_create(parent);
    _viewCont = viewCont;
    lv_obj_remove_style_all(viewCont);
    lv_obj_set_size(viewCont, width, height);

    /* scoll auto load map view */
    lv_obj_add_event(
        viewCont, [](lv_event_t* e) {
            if (!lv_indev_get_act()) {
                return;
            }

            auto self = (MapView*)lv_event_get_user_data(e);
            self->onMoving();

            /* update focus point */
            auto focus = self->getFocus();
            self->setFocusRow(focus.x, focus.y);
        },
        LV_EVENT_SCROLL, this);

    /* double click to zoom up */
    lv_obj_add_event(
        viewCont, [](lv_event_t* e) {
            auto self = (MapView*)lv_event_get_user_data(e);

            /* not scrollable */
            if (!lv_obj_has_flag(self->_viewCont, LV_OBJ_FLAG_SCROLLABLE)) {
                return;
            }

            /* double click */
            if (lv_tick_elaps(self->_lastShortClickTime) < 300) {
                if (self->setLevel(self->_curLevel + 1)) {
                    self->sendEvent(EVENT_ID::LEVEL_CHANGED, &self->_curLevel);
                }

                self->onMoving();

                lv_point_t p;
                lv_indev_get_point(lv_indev_get_act(), &p);

                lv_obj_t* cont = lv_obj_get_child(self->_viewCont, 0);
                lv_area_t area;
                lv_obj_get_coords(cont, &area);

                int32_t x = p.x - area.x1;
                int32_t y = p.y - area.y1;

                auto oriPos = self->_tileView.getTilePos(0);
                self->setFocus(oriPos.x + x, oriPos.y + y, LV_ANIM_ON);
            }

            self->_lastShortClickTime = lv_tick_get();
        },
        LV_EVENT_SHORT_CLICKED, this);

    /* coord change move map */
    subscribe(MSG_ID::GEO_COORD_UPDATE, viewCont, [](lv_event_t* e) {
        auto self = (MapView*)lv_event_get_user_data(e);
        auto msg = lv_event_get_msg(e);

        if (self->msgID(MSG_ID::GEO_COORD_UPDATE) != lv_msg_get_id(msg)) {
            return;
        }
        auto obj = lv_event_get_target_obj(e);

        /* not move when edit */
        if (lv_obj_has_state(obj, MAP_MOVED_STATE)) {
            return;
        }

        auto geoCoord = (const GeoCoord_t*)lv_msg_get_payload(msg);
        self->setFocus(geoCoord);
    });

    subscribe(MSG_ID::MOVE_RESET, viewCont, [](lv_event_t* e) {
        auto self = (MapView*)lv_event_get_user_data(e);
        auto msg = lv_event_get_msg(e);

        if (self->msgID(MSG_ID::MOVE_RESET) != lv_msg_get_id(msg)) {
            return;
        }

        auto obj = lv_event_get_current_target_obj(e);
        lv_obj_clear_state(obj, MAP_MOVED_STATE);
        self->setFocus(&self->_curGeoCoord, true);
    });

    /* map cont */
    {
        lv_obj_t* cont = lv_obj_create(viewCont);
        lv_obj_remove_style_all(cont);
        lv_obj_clear_flag(cont, LV_OBJ_FLAG_CLICKABLE);

        TileView::Rect_t rect = _tileView.getTileRect();
        lv_obj_set_size(cont, rect.width, rect.height);

        /* map img */
        uint32_t tileNum = _tileView.getTileNum();
        for (uint32_t i = 0; i < tileNum; i++) {
            lv_obj_t* img = lv_img_create(cont);
            lv_obj_remove_style_all(img);
        }
    }

    /* map reset timer */
    _timerMapReset = lv_timer_create([](lv_timer_t* timer) {
        auto self = (MapView*)lv_timer_get_user_data(timer);
        self->publish(MSG_ID::MOVE_RESET);
        self->sendEvent(EVENT_ID::MOVE_RESET);
        lv_timer_pause(timer);
    },
        0, this);
    lv_timer_pause(_timerMapReset);
}

MapView::~MapView()
{
    lv_timer_del(_timerMapReset);
    lv_obj_del(_viewCont);
    _viewCont = nullptr;
}

void MapView::setConfig(const MapConfig_t* config)
{
    _config = *config;
    _mapConv.setCoordTransformEnable(_config.coordTrans);
    tileTectInvalidate();
}

void MapView::setFocus(int32_t x, int32_t y, bool animEn)
{
    if (animEn) {
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, this);
        lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
        lv_anim_set_time(&a, 300);
        lv_anim_set_values(&a, 0, 1000);
        lv_anim_set_ready_cb(&a, [](lv_anim_t* anim) {
            auto self = (MapView*)anim->var;
            self->cancelAnim();
        });

        lv_anim_set_exec_cb(&a, [](void* var, int32_t v) {
            auto self = (MapView*)var;
            int32_t fx = lv_map(v, 0, 1000, self->_animStart.x, self->_animEnd.x);
            int32_t fy = lv_map(v, 0, 1000, self->_animStart.y, self->_animEnd.y);
            self->setFocusRow(fx, fy);
        });

        _animStart = getFocus();
        _animEnd.x = x;
        _animEnd.y = y;
        _animBusy = true;
        lv_anim_start(&a);
        return;
    }

    if (_animBusy) {
        return;
    }

    setFocusRow(x, y);
}

void MapView::setFocus(const GeoCoord_t* geoCoord, bool animEn)
{
    MapConv::Point_t point = _mapConv.getCoordinate(geoCoord->longitude, geoCoord->latitude);
    setFocus(point.x, point.y, animEn);
}

bool MapView::setLevel(int level)
{
    /* check level */
    if (level < _config.levelMin || level > _config.levelMax) {
        return false;
    }

    TileView::Point_t cur = _tileView.getFocusPos();

    _mapConv.setLevel(level);
    MapConv::Point_t newPoint = _mapConv.convertPoint(cur.x, cur.y, _curLevel);
    _curLevel = level;
    tileTectInvalidate();
    cancelAnim();
    setFocus(newPoint.x, newPoint.y);

    return true;
}

void MapView::setScrollEanble(bool enable)
{
    enable ? lv_obj_add_flag(_viewCont, LV_OBJ_FLAG_SCROLLABLE) : lv_obj_clear_flag(_viewCont, LV_OBJ_FLAG_SCROLLABLE);
}

void MapView::setTileSrcEanble(bool enable)
{
    _tileSrcEanble = enable;
}

MapView::MapPoint_t MapView::getFocus()
{
    auto scroll_x = lv_obj_get_scroll_x(_viewCont);
    auto scroll_y = lv_obj_get_scroll_y(_viewCont);
    auto tilePoint = _tileView.getTilePos(0);
    auto focusX = tilePoint.x + scroll_x + _tileView.getViewWidth() / 2;
    auto focusY = tilePoint.y + scroll_y + _tileView.getViewHeight() / 2;
    return { focusX, focusY };
}

lv_obj_t* MapView::getViewCont()
{
    return _viewCont;
}

MapView::MapPoint_t MapView::getOffset(int32_t x, int32_t y)
{
    TileView::Point_t tilePoint = { x, y };
    auto offset = _tileView.getOffset(&tilePoint);
    return { offset.x, offset.y };
}

void MapView::addArrowImage(const void* src)
{
    lv_obj_t* img = lv_img_create(_viewCont);
    lv_img_set_src(img, src);
    lv_obj_add_flag(img, LV_OBJ_FLAG_HIDDEN);
    lv_obj_update_layout(img);
    lv_obj_set_style_translate_x(img, -lv_obj_get_width(img) / 2, 0);
    lv_obj_set_style_translate_y(img, -lv_obj_get_height(img) / 2, 0);

    subscribe(MSG_ID::GEO_COORD_UPDATE, img, [](lv_event_t* e) {
        auto obj = lv_event_get_target_obj(e);
        auto msg = lv_event_get_msg(e);
        auto self = (MapView*)lv_event_get_user_data(e);

        if (self->msgID(MSG_ID::GEO_COORD_UPDATE) != lv_msg_get_id(msg)) {
            return;
        }

        /* copy geo coord */
        auto geoCoord = (const GeoCoord_t*)lv_msg_get_payload(msg);

        if (!self->arrowCheck(obj)) {
            return;
        }

        lv_img_set_angle(obj, geoCoord->course * 10);
    });

    subscribe(MSG_ID::TILE_RECT_CHANGED, img, [](lv_event_t* e) {
        auto obj = lv_event_get_target_obj(e);
        auto msg = lv_event_get_msg(e);
        auto self = (MapView*)lv_event_get_user_data(e);

        if (self->msgID(MSG_ID::TILE_RECT_CHANGED) != lv_msg_get_id(msg)) {
            return;
        }

        self->arrowCheck(obj);
    });
}

void MapView::setGeoCoord(const GeoCoord_t* geoCoord)
{
    _curGeoCoord = *geoCoord;
    publish(MSG_ID::GEO_COORD_UPDATE, &_curGeoCoord);
}

void MapView::reset(uint32_t timeout)
{
    lv_timer_set_period(_timerMapReset, timeout);
    lv_timer_reset(_timerMapReset);
    lv_timer_resume(_timerMapReset);
}

lv_uintptr_t MapView::msgID(MSG_ID id)
{
    return (lv_uintptr_t)this + (lv_uintptr_t)id;
}

void MapView::publish(MSG_ID id, const void* payload)
{
    lv_msg_send(msgID(id), payload);
}

void MapView::subscribe(MSG_ID id, lv_obj_t* obj, lv_event_cb_t event_cb)
{
    lv_msg_subscribe_obj(msgID(id), obj, this);
    lv_obj_add_event(obj, event_cb, LV_EVENT_MSG_RECEIVED, this);
}

void MapView::sendEvent(EVENT_ID id, const void* param)
{
    _listener(id, param, _userData);
}

void MapView::onMoving()
{
    cancelAnim();
    reset(MAP_RESET_TIMEOUT);

    if (lv_obj_has_state(_viewCont, MAP_MOVED_STATE)) {
        return;
    }

    lv_obj_add_state(_viewCont, MAP_MOVED_STATE);
    publish(MSG_ID::MOVE_START);
    sendEvent(EVENT_ID::MOVE_START);
}

void MapView::setFocusRow(int32_t x, int32_t y)
{
    _tileView.setFocusPos(x, y);
    TileView::Point offset = _tileView.getTileRectOffset();
    lv_obj_scroll_to(_viewCont, offset.x, offset.y, LV_ANIM_OFF);

    /* map reload check */
    if (!isTileRectChanged()) {
        return;
    }

    /* tile src */
    if (_tileSrcEanble) {
        lv_obj_t* cont = lv_obj_get_child(_viewCont, 0);
        uint32_t tileNum = _tileView.getTileNum();
        lv_disp_t* disp = lv_obj_get_disp(cont);
        lv_disp_enable_invalidation(disp, false);
        for (uint32_t i = 0; i < tileNum; i++) {
            TileView::Point_t pos = _tileView.getTilePos(i);
            lv_obj_t* img = lv_obj_get_child(cont, i);

            char mapPath[32];
            _mapConv.getPath(mapPath, sizeof(mapPath), pos.x, pos.y);

            char realPath[64];
            lv_snprintf(realPath, sizeof(realPath),
                "%s%s.%s", _config.path, mapPath, _config.ext);

            auto imgPos = _tileView.getOffset(&pos);
            lv_obj_set_pos(img, imgPos.x, imgPos.y);
            lv_img_set_src(img, realPath);
        }
        lv_disp_enable_invalidation(disp, true);

        /* merge invalid area */
        lv_obj_invalidate(_viewCont);
    }

    /* update tile view rect */
    _curTileRect = _tileView.getTileRect();
    LV_LOG_INFO("tile rect: %" LV_PRId32 ", %" LV_PRId32 ", %" LV_PRId32 ", %" LV_PRId32,
        _curTileRect.x, _curTileRect.y, _curTileRect.width, _curTileRect.height);

    publish(MSG_ID::TILE_RECT_CHANGED, &_curTileRect);
    sendEvent(EVENT_ID::TILE_RECT_CHANGED, &_curTileRect);
}

void MapView::cancelAnim()
{
    if (!_animBusy) {
        return;
    }

    lv_anim_del(this, NULL);
    _animBusy = false;
}

bool MapView::isTileRectChanged()
{
    TileView::Rect_t rect = _tileView.getTileRect();
    return memcmp(&_curTileRect, &rect, sizeof(TileView::Rect_t)) != 0;
}

void MapView::tileTectInvalidate()
{
    lv_memzero(&_curTileRect, sizeof(TileView::Rect_t));
}

bool MapView::arrowCheck(lv_obj_t* img)
{
    auto mapCoord = _mapConv.getCoordinate(_curGeoCoord.longitude, _curGeoCoord.latitude);
    auto imgPos = _tileView.getOffset((TileView::Point_t*)&mapCoord);

    if (imgPos.x < 0 || imgPos.y < 0
        || imgPos.x > _curTileRect.width || imgPos.y > _curTileRect.height) {
        lv_obj_add_flag(img, LV_OBJ_FLAG_HIDDEN);
        return false;
    }

    lv_obj_set_pos(img, imgPos.x, imgPos.y);
    lv_obj_clear_flag(img, LV_OBJ_FLAG_HIDDEN);
    return true;
}
