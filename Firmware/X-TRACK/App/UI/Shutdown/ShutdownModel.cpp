
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
#include "ShutdownModel.h"

using namespace Page;

ShutdownModel::ShutdownModel(EventListener* listener)
    : DataNode(__func__, DataProc::broker())
    , _env(this)
    , _feedback(this)
    , _listener(listener)
    , _interceptPageNavi(false)
{
    _nodePower = subscribe("Power");
    _nodePageNavi = subscribe("PageNavi");

    setEventFilter(DataNode::EVENT_PUBLISH);
}

ShutdownModel::~ShutdownModel()
{
}

int ShutdownModel::shutdown()
{
    DataProc::Power_Info_t info;
    info.delayShutdownTime = 3000;
    info.cmd = DataProc::POWER_CMD::SHUTDOWN;
    return notify(_nodePower, &info, sizeof(info));
}

void ShutdownModel::setInterceptPageNavi(bool en)
{
    _interceptPageNavi = en;
}

bool ShutdownModel::checkShutdownDisable()
{
    auto value = _env.get("shutdown");
    if (!value) {
        return false;
    }

    return (strcmp(value, "disable") == 0);
}

int ShutdownModel::onEvent(DataNode::EventParam_t* param)
{
    if (_interceptPageNavi && param->tran == _nodePageNavi) {
        auto info = (const DataProc::PageNavi_Info_t*)param->data_p;
        if (info->cmd == DataProc::PAGE_NAVI_CMD::PUSH) {
            _listener->onModelEvent(EVENT_ID::PAGE_NAVI);
        }
    }

    return DataNode::RES_OK;
}
