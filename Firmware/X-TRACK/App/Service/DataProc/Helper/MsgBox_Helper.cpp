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
#include "MsgBox_Helper.h"
#include "Frameworks/DataBroker/DataBroker.h"

using namespace DataProc;

MsgBox_Helper::MsgBox_Helper(DataNode* node)
    : _node(node)
{
    _nodeMsgBox = node->subscribe("MsgBox");
}

int MsgBox_Helper::show(const char* title, const char* txt, const char** btns)
{
    DataProc::MsgBox_Info_t info;
    info.title = title;
    info.txt = txt;
    info.btns = btns;
    return show(&info);
}

int MsgBox_Helper::show(const DataProc::MsgBox_Info_t* info)
{
    return _node->notify(_nodeMsgBox, info, sizeof(DataProc::MsgBox_Info_t));
}
