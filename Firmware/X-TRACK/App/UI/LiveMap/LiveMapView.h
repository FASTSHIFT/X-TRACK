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
#ifndef __LIVE_MAP_VIEW_H
#define __LIVE_MAP_VIEW_H

#include "../Page.h"
#include "Utils/MapView/MapView.h"

class TrackView;

namespace Page {

class LiveMapView {
public:
    enum class EVENT_ID {
        MAP_LEVEL_CHANGED, /* param: None */
        LOAD_FILE, /* param: None */
        _LAST,
    };

    enum class MSG_ID {
        GEO_COORD_UPDATE, /* param: MapView::GeoCoord_t */
        SPORT_STATUS, /* param: SportStatus_Infot */
        TRACK_REC, /* param: DataProc::TrackFilter_Info_t */
        MAP_CONFIG_UPDATE, /* param: MapView::MapConfig_t */
        MAP_SET_LEVEL, /* param: int */
        MAP_TILE_RECT_CHANGED, /* param: TileView::TileRect_t */
        LOAD_ROAD_MAP, /* param: DataProc::RoadMap_Info_t */
        _LAST,
    };

    class EventListener {
    public:
        virtual void onViewEvent(EVENT_ID id, const void* param = nullptr) = 0;
    };

public:
    LiveMapView(EventListener* listener, lv_obj_t* root);
    ~LiveMapView();
    void publish(MSG_ID id, const void* payload = nullptr);

private:
    EventListener* _listener;
    ResourcePool::Font _fontSpeed;
    struct STYLE {
        STYLE()
        {
            lv_style_init(&cont);
            lv_style_init(&line);
        }
        ~STYLE()
        {
            lv_style_reset(&cont);
            lv_style_reset(&line);
        }
        lv_style_t cont;
        lv_style_t line;
    } _style;

    MapView* _mapView;
    TrackView* _roadMapView;
    TrackView* _trackView;
    MapConv _mapConv;

private:
    lv_uintptr_t msgID(MSG_ID id);
    void subscribe(MSG_ID id, lv_obj_t* obj, lv_event_cb_t event_cb);
    void styleInit();
    void mapViewCreate(lv_obj_t* par);
    static void onMapViewEvent(MapView::EVENT_ID id, const void* param, void* userData);
    void sportInfoCreate(lv_obj_t* par);
    void trackCreate(lv_obj_t* par);
};

}

#endif /* __LIVE_MAP_VIEW_H */
