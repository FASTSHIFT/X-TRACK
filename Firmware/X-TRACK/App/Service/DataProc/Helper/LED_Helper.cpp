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
#include "LED_Helper.h"
#include "Frameworks/DataBroker/DataBroker.h"

using namespace DataProc;

LED_Helper::LED_Helper(DataNode* node, LED_ID id)
    : _node(node)
    , _nodeLED(node->subscribe("LED"))
    , _id(id)
{
}

int LED_Helper::start(const LED_Squence_t* squence)
{
    LED_Info_t info;
    info.id = _id;
    info.squence = squence;
    return _node->notify(_nodeLED, &info, sizeof(info));
}

int LED_Helper::on()
{
    static const LED_Squence_t seq[] = {
        { LED_STATUS::ON, 0 },
        { LED_STATUS::STOP, 0 },
    };

    return start(seq);
}

int LED_Helper::off()
{
    static const LED_Squence_t seq[] = {
        { LED_STATUS::OFF, 0 },
        { LED_STATUS::STOP, 0 },
    };

    return start(seq);
}
