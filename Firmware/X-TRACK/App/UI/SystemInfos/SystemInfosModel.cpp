
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
#include "SystemInfosModel.h"

using namespace Page;

SystemInfosModel::SystemInfosModel(EventListener* listener)
    : DataNode(__func__, DataProc::broker())
    , _listener(listener)
    , _env(this)
{
    _nodeSportStatus = subscribe("SportStatus");
    _nodeGNSS = subscribe("GNSS");
    _nodeClock = subscribe("Clock");
    _nodePower = subscribe("Power");
    _nodeStorage = subscribe("Storage");
    _nodeVersion = subscribe("Version");

    setEventFilter(DataNode::EVENT_PUBLISH);

    initBingdings();
}

SystemInfosModel::~SystemInfosModel()
{
}

void SystemInfosModel::initBingdings()
{
    _bindingVerison.setCallback(
        /* setter */
        nullptr,
        /* getter */
        [](SystemInfosModel* self) -> DataProc::Version_Info_t {
            DataProc::Version_Info_t info;
            self->pull(self->_nodeVersion, &info, sizeof(info));
            return info;
        },
        this);

    _bindingStorage.setCallback(
        /* setter */
        nullptr,
        /* getter */
        [](SystemInfosModel* self) -> DataProc::Storage_Device_Info_t {
            DataProc::Storage_Device_Info_t info;
            self->pull(self->_nodeStorage, &info, sizeof(info));
            return info;
        },
        this);
}

void* SystemInfosModel::getBinding(BINDING_TYPE type)
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

int SystemInfosModel::onEvent(DataNode::EventParam_t* param)
{
    if (param->tran == _nodeSportStatus) {
        _listener->onModelEvent(EVENT_ID::SPORT_STATUS, param->data_p);
    } else if (param->tran == _nodeGNSS) {
        _listener->onModelEvent(EVENT_ID::GNSS, param->data_p);
    } else if (param->tran == _nodeClock) {
        _listener->onModelEvent(EVENT_ID::CLOCK, param->data_p);
    } else if (param->tran == _nodePower) {
        _listener->onModelEvent(EVENT_ID::POWER, param->data_p);
    }

    return DataNode::RES_OK;
}
