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
#ifndef __LIVEMAP_MODEL_H
#define __LIVEMAP_MODEL_H

#include "Service/DataProc/DataProc.h"

namespace Page {

class LiveMapModel : private DataNode {
public:
    enum class EVENT_ID {
        SPORT_STATUS, /* param: SportStatus_Info_t */
        TRACK_REC, /* param: TrackFilter_Info_t */
        GNSS, /* param: GNSS_Info_t */
        MAP_LEVEL_CHANGE, /* param: int */
        _LAST,
    };

    class EventListener {
    public:
        virtual void onModelEvent(EVENT_ID id, const void* param = nullptr) = 0;
    };

public:
    LiveMapModel(EventListener* listener);
    ~LiveMapModel();

    DataProc::Env_Helper* env()
    {
        return &_env;
    }

    bool getGNSSInfo(HAL::GNSS_Info_t* info);
    void setGNSSEnable(bool enable) { _gnssEnable = enable; }
    bool getMapInfo(DataProc::Map_Info_t* info);
    int getMapLevel() { return _mapLevel; };
    bool setMapLevel(int level, bool sendEvent = false);
    bool getTrackFilterInfo(DataProc::TrackFilter_Info_t* info);
    void loadSportStatusInfo();

private:
    EventListener* _listener;
    DataProc::Env_Helper _env;
    const DataNode* _nodeSportStatus;
    const DataNode* _nodeGNSS;
    const DataNode* _nodeTrackFilter;
    int _mapLevel;
    DataProc::Map_Info_t _mapInfo;
    bool _gnssEnable;

private:
    virtual int onEvent(DataNode::EventParam_t* param);
};

}

#endif /* __LIVEMAP_MODEL_H */
