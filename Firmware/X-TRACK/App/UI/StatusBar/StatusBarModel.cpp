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
#include "StatusBarModel.h"

using namespace Page;

StatusBarModel::StatusBarModel(EventListener* listener)
    : DataNode(__func__, DataProc::broker())
    , _env(this)
    , _listener(listener)
{
    _nodeClock = subscribe("Clock");
    _nodeGNSS = subscribe("GNSS");
    _nodePower = subscribe("Power");
    _nodePageNavi = subscribe("PageNavi");

    setEventFilter(DataNode::EVENT_PUBLISH);
}

StatusBarModel::~StatusBarModel()
{
}

int StatusBarModel::onEvent(DataNode::EventParam_t* param)
{
    if (param->tran == _nodeClock) {
        auto info = (const HAL::Clock_Info_t*)param->data_p;
        uint16_t hhmm = (uint8_t)info->hour << 8 | (uint8_t)info->minute;
        _listener->onModelEvent(EVENT_ID::CLOCK, &hhmm);
        return DataNode::RES_OK;
    }

    if (param->tran == _nodeGNSS) {
        auto info = (const HAL::GNSS_Info_t*)param->data_p;
        _listener->onModelEvent(EVENT_ID::GNSS, &info->satellites);
        return DataNode::RES_OK;
    }

    if (param->tran == _nodePower) {
        _listener->onModelEvent(EVENT_ID::POWER, param->data_p);
        return DataNode::RES_OK;
    }

    if (param->tran == _env) {
        _listener->onModelEvent(EVENT_ID::ENV, param->data_p);
        return DataNode::RES_OK;
    }

    return DataNode::RES_UNSUPPORTED_REQUEST;
}
