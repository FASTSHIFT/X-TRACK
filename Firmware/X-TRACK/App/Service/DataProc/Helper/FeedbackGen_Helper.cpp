/*
 * MIT License
 * Copyright (c) 2024 _VIFEXTech
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
#include "FeedbackGen_Helper.h"
#include "Frameworks/DataBroker/DataBroker.h"

using namespace DataProc;

FeedbackGen_Helper::FeedbackGen_Helper(DataNode* node)
    : _node(node)
{
    _nodeFeedBackGen = node->subscribe("FeedbackGen");
}

int FeedbackGen_Helper::trigger(DataProc::FEEDBACK_GEN_EFFECT effect)
{
    DataProc::FeedbackGen_Info_t info;
    info.cmd = DataProc::FEEDBACK_GEN_CMD::PREPARE;
    info.effect = effect;

    int retval = _node->notify(_nodeFeedBackGen, &info, sizeof(info));
    if (retval != DataNode::RES_OK) {
        return retval;
    }

    info.cmd = DataProc::FEEDBACK_GEN_CMD::TRIGGER;
    return _node->notify(_nodeFeedBackGen, &info, sizeof(info));
}
