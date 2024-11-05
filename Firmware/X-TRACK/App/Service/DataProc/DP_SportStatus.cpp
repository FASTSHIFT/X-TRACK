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
#include "Config/Config.h"
#include "DataProc.h"
#include "Service/HAL/HAL.h"
#include "Service/HAL/HAL_Log.h"
#include "Utils/Filters/Filters.h"
#include "Utils/Geo/Geo.h"

/* clang-format off */

#define CALORIC_CORFFICIENT 0.5f

#define CADENCE_TIMEOUT_MS (3000) // 20rpm

#define MPS_TO_KPH(x) ((x) * 3.6f)
#define MPS_TO_MPH(x) ((x) * 2.2369362920544f)

/* clang-format on */

using namespace DataProc;

class DP_SportStatus {
public:
    DP_SportStatus(DataNode* node);

private:
    DataNode* _node;
    const DataNode* _nodeGNSS;
    SportStatus_Info_t _sportStatus;
    bool _isDistanceFirst;
    double _preLongitude;
    double _preLatitude;

private:
    int onEvent(DataNode::EventParam_t* param);
    void onTimer();
    int onNotify(const SportStatus_Info_t* info);
    double getDistanceOffset(const HAL::GNSS_Info_t* gnssInfo);
};

DP_SportStatus::DP_SportStatus(DataNode* node)
    : _node(node)
    , _nodeGNSS(nullptr)
    , _isDistanceFirst(true)
    , _preLongitude(0.0f)
    , _preLatitude(0.0f)
{
    _nodeGNSS = node->subscribe("GNSS");

#define SPORT_STORAGE(MEMBER) storage.add(#MEMBER, &_sportStatus.MEMBER)

    Storage_Helper storage(node);

    storage.structStart("sportStatus");
    SPORT_STORAGE(totalDistance);
    storage.addArray("totalTime", &_sportStatus.totalTime, sizeof(uint32_t), 2, STORAGE_TYPE::INT);
    SPORT_STORAGE(speedMaxKph);
    SPORT_STORAGE(weight);
    SPORT_STORAGE(longitude);
    SPORT_STORAGE(latitude);
    storage.structEnd();

    _sportStatus.weight = CONFIG_WEIGHT_DEFAULT;
    _sportStatus.longitude = CONFIG_GNSS_LONGITUDE_DEFAULT;
    _sportStatus.latitude = CONFIG_GNSS_LATITUDE_DEFAULT;
    _sportStatus.lastTick = HAL::GetTick();

    node->setEventCallback(
        [](DataNode* n, DataNode::EventParam_t* param) {
            auto ctx = (DP_SportStatus*)n->getUserData();
            return ctx->onEvent(param);
        },
        DataNode::EVENT_PULL | DataNode::EVENT_TIMER | DataNode::EVENT_NOTIFY);

    node->startTimer(500);
}

int DP_SportStatus::onEvent(DataNode::EventParam_t* param)
{
    switch (param->event) {
    case DataNode::EVENT_NOTIFY:
        if (param->size != sizeof(SportStatus_Info_t)) {
            return DataNode::RES_SIZE_MISMATCH;
        }
        return onNotify((SportStatus_Info_t*)param->data_p);

    case DataNode::EVENT_PULL:
        if (param->size != sizeof(SportStatus_Info_t)) {
            return DataNode::RES_SIZE_MISMATCH;
        }
        memcpy(param->data_p, &_sportStatus, sizeof(SportStatus_Info_t));
        break;

    case DataNode::EVENT_TIMER:
        onTimer();
        break;

    default:
        break;
    }

    return DataNode::RES_OK;
}

void DP_SportStatus::onTimer()
{
    /* GPS */
    HAL::GNSS_Info_t gnssInfo;
    if (_node->pull(_nodeGNSS, &gnssInfo, sizeof(gnssInfo)) != DataNode::RES_OK) {
        return;
    }

    uint32_t timeElaps = HAL::GetTickElaps(_sportStatus.lastTick);

    float speedKph = 0.0f;
    bool isSignaLost = (gnssInfo.isVaild && (gnssInfo.satellites == 0));

    /* Only valid latitude and longitude are recorded */
    if (gnssInfo.isVaild) {
        _sportStatus.longitude = gnssInfo.longitude;
        _sportStatus.latitude = gnssInfo.latitude;
        _sportStatus.altitude = gnssInfo.altitude;
    }

    if (gnssInfo.satellites >= 3) {
        float spd = gnssInfo.speed;
        speedKph = spd > 1 ? spd : 0;
    }

    if (speedKph > 0.0f || isSignaLost) {
        _sportStatus.singleTime += timeElaps;
        _sportStatus.totalTime += timeElaps;

        if (speedKph > 0.0f) {
            float dist = getDistanceOffset(&gnssInfo);

            _sportStatus.singleDistance += dist;
            _sportStatus.totalDistance += dist;

            float meterPerSec = _sportStatus.singleDistance * 1000 / _sportStatus.singleTime;
            _sportStatus.speedAvgKph = MPS_TO_KPH(meterPerSec);

            if (speedKph > _sportStatus.speedMaxKph) {
                _sportStatus.speedMaxKph = speedKph;
            }

            float calorie = speedKph * _sportStatus.weight * CALORIC_CORFFICIENT * timeElaps / 1000 / 3600;
            _sportStatus.singleCalorie += calorie;
        }
    }

    _sportStatus.speedKph = speedKph;

    _sportStatus.lastTick = HAL::GetTick();

    _node->publish(&_sportStatus, sizeof(SportStatus_Info_t));
}

int DP_SportStatus::onNotify(const SportStatus_Info_t* info)
{
    switch (info->cmd) {
    case SPORT_STATUS_CMD::RESET_SINGLE:
        _sportStatus.singleDistance = 0;
        _sportStatus.singleCalorie = 0;
        _sportStatus.singleTime = 0;
        break;
    case SPORT_STATUS_CMD::RESET_TOTAL:
        _sportStatus.totalDistance = 0;
        _sportStatus.totalTime = 0;
        break;
    case SPORT_STATUS_CMD::RESET_SPEED_MAX:
        _sportStatus.speedMaxKph = 0;
        break;
    case SPORT_STATUS_CMD::SET_WEIGHT:
        _sportStatus.weight = info->weight;
        break;
    default:
        return DataNode::RES_PARAM_ERROR;
    }

    return DataNode::RES_OK;
}

double DP_SportStatus::getDistanceOffset(const HAL::GNSS_Info_t* gnssInfo)
{
    double offset = 0.0f;

    if (!_isDistanceFirst) {
        offset = Geo::distanceBetween(
            gnssInfo->latitude,
            gnssInfo->longitude,
            _preLatitude,
            _preLongitude);
    } else {
        _isDistanceFirst = false;
    }

    _preLongitude = gnssInfo->longitude;
    _preLatitude = gnssInfo->latitude;

    return offset;
}

DATA_PROC_DESCRIPTOR_DEF(SportStatus)
