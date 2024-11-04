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
#ifndef __MAP_VIEW_H
#define __MAP_VIEW_H

#include "Utils/MapConv/MapConv.h"
#include "Utils/TileView/TileView.h"
#include "lvgl/lvgl.h"

class MapView {
public:
    typedef struct
    {
        float longitude;
        float latitude;
        float altitude;
        float course;
    } GeoCoord_t;

    typedef struct
    {
        const char* path;
        const char* ext;
        uint8_t levelMin;
        uint8_t levelMax;
        bool coordTrans;
    } MapConfig_t;

    typedef struct
    {
        int32_t x;
        int32_t y;
    } MapPoint_t;

    enum class EVENT_ID {
        MOVE_START, /* param: None */
        MOVE_RESET, /* param: None */
        LEVEL_CHANGED, /* param: int */
        TILE_RECT_CHANGED, /* param: TileView::Rect_t */
        _LAST,
    };

    typedef void EventListener(EVENT_ID id, const void* param, void* userData);

public:
    MapView(uint32_t width, uint32_t height, lv_obj_t* parent, EventListener* listener, void* userData);
    ~MapView();

    void setConfig(const MapConfig_t* config);
    void setFocus(int32_t x, int32_t y, bool animEn = false);
    void setFocus(const GeoCoord_t* geoCoord, bool animEn = false);
    bool setLevel(int level);
    void setScrollEanble(bool enable);
    void setTileSrcEanble(bool enable);
    MapPoint_t getFocus();
    lv_obj_t* getViewCont();
    MapPoint_t getOffset(int32_t x, int32_t y);
    void addArrowImage(const void* src);
    void setGeoCoord(const GeoCoord_t* geoCoord);
    void reset(uint32_t timeout);
    uint32_t getTileSize() { return _tileView.getTileSize(); }
    bool getCoordTrans() { return _mapConv.getCoordTransformEnable(); }

private:
    enum class MSG_ID {
        GEO_COORD_UPDATE, /* param: GeoCoord_t */
        TILE_RECT_CHANGED, /* param: TileView::Rect_t */
        MOVE_START, /* param: None */
        MOVE_RESET, /* param: None */
        _LAST,
    };

private:
    TileView _tileView;
    MapConv _mapConv;

    /* current state */
    TileView::Rect_t _curTileRect;
    GeoCoord_t _curGeoCoord;
    int _curLevel;

    /* config */
    MapConfig_t _config;

    /* view interaction */
    lv_obj_t* _viewCont;
    lv_timer_t* _timerMapReset;
    uint32_t _lastShortClickTime;
    bool _animBusy;
    MapPoint_t _animStart;
    MapPoint_t _animEnd;
    bool _tileSrcEanble;

    /* event */
    EventListener* _listener;
    void* _userData;

private:
    lv_uintptr_t msgID(MSG_ID id);
    void publish(MSG_ID id, const void* payload = nullptr);
    void subscribe(MSG_ID id, lv_obj_t* obj, lv_event_cb_t event_cb);
    void sendEvent(EVENT_ID id, const void* param = nullptr);
    void onMoving();
    void setFocusRow(int32_t x, int32_t y);
    void cancelAnim();
    bool isTileRectChanged();
    void tileTectInvalidate();
    bool arrowCheck(lv_obj_t* img);
};

#endif // __MAP_VIEW_H
