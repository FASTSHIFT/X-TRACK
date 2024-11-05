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
#include "DashboardModel.h"

using namespace Page;

DashboardModel::DashboardModel(EventListener* listener)
    : DataNode(__func__, DataProc::broker())
    , _listener(listener)
    , _env(this)
{
    _nodeSportStatus = subscribe("SportStatus");
    _nodeRecorder = subscribe("Recorder");
    _nodeGNSS = subscribe("GNSS");
    setEventFilter(DataNode::EVENT_PUBLISH);
}

DashboardModel::~DashboardModel()
{
}

void DashboardModel::initBingdings()
{
    _bindingRec.setCallback(
        /* setter */
        [](DashboardModel* self, bool v) {
            DataProc::Recorder_Info_t info;
            info.active = v;
            self->notify(self->_nodeRecorder, &info, sizeof(info));
        },
        /* getter */
        [](DashboardModel* self) -> bool {
            DataProc::Recorder_Info_t info;
            self->pull(self->_nodeRecorder, &info, sizeof(info));
            return info.active;
        },
        this);
}

void* DashboardModel::getBinding(BINDING_TYPE type)
{
    switch (type) {
#define BINDING_DEF(name, type) \
    case BINDING_TYPE::name:    \
        return &_binding##name;
#include "BindingDef.inc"
#undef BINDING_DEF
    default:
        return nullptr;
    }
}

int DashboardModel::onEvent(DataNode::EventParam_t* param)
{
    if (param->tran == _nodeSportStatus) {
        _listener->onModelEvent(EVENT_ID::SPORT_STATUS, param->data_p);
    } else if (param->tran == _nodeRecorder) {
        _listener->onModelEvent(EVENT_ID::RECORDER_STATUS, param->data_p);
    } else if (param->tran == _nodeGNSS) {
        _listener->onModelEvent(EVENT_ID::GNSS, param->data_p);
    }

    return DataNode::RES_OK;
}
