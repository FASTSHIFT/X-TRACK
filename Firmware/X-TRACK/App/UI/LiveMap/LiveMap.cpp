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
#include "LiveMap.h"

using namespace Page;

APP_DESCRIPTOR_DEF(LiveMap);

LiveMap::LiveMap()
    : _model(nullptr)
    , _view(nullptr)
{
}

LiveMap::~LiveMap()
{
}

void LiveMap::onInstalled()
{
    /* workaround page is hidden after the model is not disconnected */
    setAutoCacheEnable(false);

    setBackGestureDirection(LV_DIR_RIGHT);
}

void LiveMap::onViewLoad()
{
    _model = new LiveMapModel(this);
    _view = new LiveMapView(this, getRoot());

    DataProc::Map_Info_t mapInfo;
    if (_model->getMapInfo(&mapInfo)) {
        MapView::MapConfig_t config;
        config.path = mapInfo.path;
        config.ext = mapInfo.ext;
        config.levelMin = mapInfo.levelMin;
        config.levelMax = mapInfo.levelMax;
        config.coordTrans = mapInfo.coordTrans;
        _view->publish(LiveMapView::MSG_ID::MAP_CONFIG_UPDATE, &config);
    }
}

void LiveMap::onViewDidLoad()
{
}

void LiveMap::onViewWillAppear()
{
}

void LiveMap::onViewDidAppear()
{
    _model->env()->set("statusbar-opa", "light");

    _model->loadSportStatusInfo();

    int mapLevel = _model->getMapLevel();
    onModelEvent(LiveMapModel::EVENT_ID::MAP_LEVEL_CHANGE, &mapLevel);

    DataProc::TrackFilter_Info_t trackFilterInfo;
    if (_model->getTrackFilterInfo(&trackFilterInfo)) {
        onModelEvent(LiveMapModel::EVENT_ID::TRACK_REC, &trackFilterInfo);
    }

    HAL::GNSS_Info_t gnssInfo;
    if (_model->getGNSSInfo(&gnssInfo)) {
        onModelEvent(LiveMapModel::EVENT_ID::GNSS, &gnssInfo);

        /* enable gnss event */
        _model->setGNSSEnable(true);
    }
}

void LiveMap::onViewWillDisappear()
{
    /* disable gnss event */
    _model->setGNSSEnable(false);

    int mapLevel = _model->getMapLevel();
    char buf[8];
    lv_snprintf(buf, sizeof(buf), "%d", mapLevel);
    _model->env()->set("maplevel", buf);
}

void LiveMap::onViewDidDisappear()
{
}

void LiveMap::onViewUnload()
{
    delete _model;
    delete _view;
}

void LiveMap::onViewDidUnload()
{
}

void LiveMap::onModelEvent(LiveMapModel::EVENT_ID id, const void* param)
{
    switch (id) {
    case LiveMapModel::EVENT_ID::SPORT_STATUS: {
        _view->publish(LiveMapView::MSG_ID::SPORT_STATUS, param);
    } break;

    case LiveMapModel::EVENT_ID::GNSS: {
        auto gnssInfo = (const HAL::GNSS_Info_t*)param;
        MapView::GeoCoord_t geoCoord;
        geoCoord.longitude = gnssInfo->longitude;
        geoCoord.latitude = gnssInfo->latitude;
        geoCoord.altitude = gnssInfo->altitude;
        geoCoord.course = gnssInfo->course;
        _view->publish(LiveMapView::MSG_ID::GEO_COORD_UPDATE, &geoCoord);
    } break;

    case LiveMapModel::EVENT_ID::MAP_LEVEL_CHANGE:
        _view->publish(LiveMapView::MSG_ID::MAP_SET_LEVEL, param);
        break;

    case LiveMapModel::EVENT_ID::TRACK_REC:
        _view->publish(LiveMapView::MSG_ID::TRACK_REC, param);
        break;

    default:
        break;
    }
}

void LiveMap::onViewEvent(LiveMapView::EVENT_ID id, const void* param)
{
    switch (id) {
    case LiveMapView::EVENT_ID::MAP_LEVEL_CHANGED: {
        int mapLevel = *(const int*)param;
        _model->setMapLevel(mapLevel);
    } break;

    default:
        break;
    }
}
