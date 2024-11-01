
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
#ifndef __STATUS_BAR_MODEL_H
#define __STATUS_BAR_MODEL_H

#include "Service/DataProc/DataProc.h"

namespace Page {

class StatusBarModel : private DataNode {

public:
    enum class EVENT_ID {
        CLOCK, /* param: uint16_t (hhmm) */
        GNSS, /* param: int16_t (satellites) */
        POWER, /* param: DataProc::Power_Info_t */
        ENV, /* param: Env_Info_t */
        SHOW, /* param: bool */
        _LAST,
    };

    class EventListener {
    public:
        virtual void onModelEvent(EVENT_ID id, const void* param = nullptr) = 0;
    };

public:
    StatusBarModel(EventListener* listener);
    ~StatusBarModel();
    DataProc::Env_Helper* env()
    {
        return &_env;
    }

private:
    const DataNode* _nodeClock;
    const DataNode* _nodeGNSS;
    const DataNode* _nodePower;
    const DataNode* _nodePageNavi;
    DataProc::Env_Helper _env;
    EventListener* _listener;

private:
    virtual int onEvent(DataNode::EventParam_t* param);
};

}

#endif
