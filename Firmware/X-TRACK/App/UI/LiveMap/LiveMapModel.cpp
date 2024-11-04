
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
#include "LiveMapModel.h"
#include <stdlib.h>

using namespace Page;

LiveMapModel::LiveMapModel(EventListener* listener)
    : DataNode(__func__, DataProc::broker())
    , _listener(listener)
    , _env(this)
    , _mapLevel(0)
    , _gnssEnable(false)
{
    _nodeGNSS = subscribe("GNSS");
    _nodeSportStatus = subscribe("SportStatus");
    _nodeTrackFilter = subscribe("TrackFilter");

    subscribe("MapInfo");
    setEventFilter(DataNode::EVENT_PUBLISH);

    auto maplevelStr = _env.get("maplevel");
    if (maplevelStr) {
        _mapLevel = atoi(maplevelStr);
    } else {
        DataProc::Map_Info_t mapInfo;
        if (getMapInfo(&mapInfo)) {
            _mapLevel = mapInfo.levelMax;
        }
    }
}

LiveMapModel::~LiveMapModel()
{
}

bool LiveMapModel::getGNSSInfo(HAL::GNSS_Info_t* info)
{
    if (pull(_nodeGNSS, info, sizeof(HAL::GNSS_Info_t)) == DataNode::RES_OK) {
        if (info->isVaild) {
            return true;
        }
    }

    memset(info, 0, sizeof(HAL::GNSS_Info_t));

    /* Use default location */
    DataProc::SportStatus_Info_t ssInfo;
    if (pull(_nodeSportStatus, &ssInfo, sizeof(ssInfo)) == DataNode::RES_OK) {
        info->longitude = ssInfo.longitude;
        info->latitude = ssInfo.latitude;
        return true;
    }

    return false;
}

bool LiveMapModel::getMapInfo(DataProc::Map_Info_t* info)
{
    if (_mapInfo.path) {
        *info = _mapInfo;
        return true;
    }

    if (pull("MapInfo", &_mapInfo, sizeof(DataProc::Map_Info_t)) == DataNode::RES_OK) {
        *info = _mapInfo;
        return true;
    }

    return false;
}

bool LiveMapModel::setMapLevel(int level, bool sendEvent)
{
    DataProc::Map_Info_t mapInfo;
    if (!getMapInfo(&mapInfo)) {
        return false;
    }

    if (level < mapInfo.levelMin || level > mapInfo.levelMax) {
        return false;
    }

    _mapLevel = level;
    if (sendEvent) {
        _listener->onModelEvent(EVENT_ID::MAP_LEVEL_CHANGE, &_mapLevel);
    }

    return true;
}

bool LiveMapModel::getTrackFilterInfo(DataProc::TrackFilter_Info_t* info)
{
    return pull(_nodeTrackFilter, info, sizeof(DataProc::TrackFilter_Info_t)) == DataNode::RES_OK;
}

void LiveMapModel::loadSportStatusInfo()
{
    DataProc::SportStatus_Info_t info;
    if (pull(_nodeSportStatus, &info, sizeof(info)) != DataNode::RES_OK) {
        return;
    }

    _listener->onModelEvent(EVENT_ID::SPORT_STATUS, &info);
}

int LiveMapModel::onEvent(DataNode::EventParam_t* param)
{
    if (param->tran == _nodeSportStatus) {
        _listener->onModelEvent(EVENT_ID::SPORT_STATUS, param->data_p);
    } else if (param->tran == _nodeGNSS && _gnssEnable) {
        _listener->onModelEvent(EVENT_ID::GNSS, param->data_p);
    } else if (param->tran == _nodeTrackFilter) {
        _listener->onModelEvent(EVENT_ID::TRACK_REC, param->data_p);
    }

    return DataNode::RES_OK;
}
