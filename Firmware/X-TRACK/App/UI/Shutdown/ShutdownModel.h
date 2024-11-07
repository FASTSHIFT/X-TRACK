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
#ifndef __SHUTDOWN_MODEL_H
#define __SHUTDOWN_MODEL_H

#include "Service/DataProc/DataProc.h"

namespace Page {

class ShutdownModel : private DataNode {
public:
    enum class EVENT_ID {
        PAGE_NAVI, /* param: None */
        _LAST,
    };

    class EventListener {
    public:
        virtual void onModelEvent(EVENT_ID id, const void* param = nullptr) = 0;
    };

public:
    ShutdownModel(EventListener* listener);
    ~ShutdownModel();
    int shutdown();
    void setInterceptPageNavi(bool en);
    bool checkShutdownDisable();

    DataProc::Env_Helper* env()
    {
        return &_env;
    }

    DataProc::FeedbackGen_Helper* feedback()
    {
        return &_feedback;
    }

private:
    DataProc::Env_Helper _env;
    DataProc::FeedbackGen_Helper _feedback;
    const DataNode* _nodePower;
    const DataNode* _nodePageNavi;
    EventListener* _listener;
    bool _interceptPageNavi;

private:
    virtual int onEvent(DataNode::EventParam_t* param);
};

}

#endif /* __SHUTDOWN_MODEL_H */
