/*
 * MIT License
 * Copyright (c) 2021 - 2023 _VIFEXTech
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
#include "DataProc.h"
#include "Utils/MapConv/MapConv.h"
#include "Utils/PointContainer/PointContainer.h"
#include "Utils/TrackPointFilter/TrackPointFilter.h"
#include "lvgl/lvgl.h"
#include <vector>

#define FILTER_LEVEL 16

using namespace DataProc;

class DP_TrackFilter {
public:
    DP_TrackFilter(DataNode* node);

private:
    DataNode* _node;
    const DataNode* _nodeGNSS;
    const DataNode* _nodeMapInfo;
    PointContainer* _pointContainer;
    bool _isActive;
    MapConv _mapConv;
    TrackPointFilter _pointFilter;

private:
    int onEvent(DataNode::EventParam_t* param);
    bool onNotify(const TrackFilter_Info_t* info);
    void onGNSSPublish(const HAL::GNSS_Info_t* gnssInfo);
};

DP_TrackFilter::DP_TrackFilter(DataNode* node)
    : _node(node)
    , _nodeGNSS(nullptr)
    , _pointContainer(nullptr)
    , _isActive(false)
    , _mapConv(true, FILTER_LEVEL)
{
    _nodeGNSS = node->subscribe("GNSS");
    if (!_nodeGNSS) {
        return;
    }

    _nodeMapInfo = node->subscribe("MapInfo");

    node->setEventCallback(
        [](DataNode* n, DataNode::EventParam_t* param) {
            auto ctx = (DP_TrackFilter*)n->getUserData();
            return ctx->onEvent(param);
        },
        DataNode::EVENT_PUBLISH | DataNode::EVENT_PULL | DataNode::EVENT_NOTIFY);
}

int DP_TrackFilter::onEvent(DataNode::EventParam_t* param)
{
    if (param->tran == _nodeGNSS) {
        onGNSSPublish((HAL::GNSS_Info_t*)param->data_p);
        return DataNode::RES_OK;
    }

    if (param->size != sizeof(TrackFilter_Info_t)) {
        return DataNode::RES_SIZE_MISMATCH;
    }

    switch (param->event) {
    case DataNode::EVENT_PULL: {
        TrackFilter_Info_t* info = (TrackFilter_Info_t*)param->data_p;
        info->active = _isActive;
        info->level = FILTER_LEVEL;
        info->pointCont = _pointContainer;
    } break;
    case DataNode::EVENT_NOTIFY:
        if (onNotify((TrackFilter_Info_t*)param->data_p)) {

            /* publish status */
            TrackFilter_Info_t info;
            info.active = _isActive;
            info.level = FILTER_LEVEL;
            info.pointCont = _pointContainer;
            _node->publish(&info, sizeof(info));
        }
        break;

    default:
        break;
    }

    return DataNode::RES_OK;
}

bool DP_TrackFilter::onNotify(const TrackFilter_Info_t* info)
{
    if (info->active) {
        /* check already started */
        if (_pointContainer) {
            LV_LOG_WARN("Track filter already started");
            return false;
        }

        /* start */
        _pointContainer = new PointContainer;
        _pointFilter.reset();
        _isActive = true;

        /* get map info */
        Map_Info_t mapInfo;
        if (_node->pull(_nodeMapInfo, &mapInfo, sizeof(mapInfo)) == DataNode::RES_OK) {
            _mapConv.setCoordTransformEnable(mapInfo.coordTrans);
        }

        LV_LOG_USER("Track filter start");
        return true;
    }

    if (!_pointContainer) {
        LV_LOG_WARN("NOT started");
        return false;
    }

    _isActive = false;
    delete _pointContainer;
    _pointContainer = nullptr;

    uint32_t sum = 0, output = 0;
    _pointFilter.getCounts(&sum, &output);
    LV_LOG_USER(
        "Track filter stop, "
        "filted(%" LV_PRIu32 "%%): sum = %" LV_PRIu32 ", output = %" LV_PRIu32,
        sum ? (100 - output * 100 / sum) : 0,
        sum,
        output);

    return true;
}

void DP_TrackFilter::onGNSSPublish(const HAL::GNSS_Info_t* gnssInfo)
{
    if (!_isActive) {
        return;
    }

    MapConv::Point_t point = _mapConv.getCoordinate(gnssInfo->longitude, gnssInfo->latitude);

    if (_pointFilter.pushPoint(point.x, point.y)) {
        _pointContainer->pushPoint(point.x, point.y);
    }
}

DATA_PROC_DESCRIPTOR_DEF(TrackFilter);
