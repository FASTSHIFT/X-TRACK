/*
 * MIT License
 * Copyright (c) 2023 - 2024 _VIFEXTech
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
#ifndef __DASHBOARD_MODEL_H
#define __DASHBOARD_MODEL_H

#include "Service/DataProc/DataProc.h"
#include "Utils/Binding/Binding.h"

namespace Page {

class DashboardModel : private DataNode {

public:
    enum class EVENT_ID {
        SPORT_STATUS, /* param: SportStatus_Info_t */
        RECORDER_STATUS, /* param: Recorder_Info_t */
        GNSS, /* param: GNSS_Info_t */
        _EVENT_LAST,
    };

    enum class BINDING_TYPE {
#define BINDING_DEF(name, type) name,
#include "BindingDef.inc"
#undef BINDING_DEF
    };

    typedef struct {
        BINDING_TYPE type;
        void* binding;
    } Binding_Info_t;

    class EventListener {
    public:
        virtual void onModelEvent(EVENT_ID id, const void* param = nullptr) = 0;
    };

public:
    DashboardModel(EventListener* listener);
    ~DashboardModel();
    void initBingdings();
    void* getBinding(BINDING_TYPE type);
    DataProc::Env_Helper* env()
    {
        return &_env;
    }

private:
    EventListener* _listener;
    const DataNode* _nodeSportStatus;
    const DataNode* _nodeRecorder;
    const DataNode* _nodeGNSS;
    DataProc::Env_Helper _env;

#define BINDING_DEF(name, type) Binding<type, DashboardModel> _binding##name;
#include "BindingDef.inc"
#undef BINDING_DEF

private:
    virtual int onEvent(DataNode::EventParam_t* param);
};

} /* namespace Page */

#endif
