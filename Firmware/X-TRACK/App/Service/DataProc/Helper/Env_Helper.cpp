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
#include "Env_Helper.h"
#include "Frameworks/DataBroker/DataBroker.h"
#include <cstdio>
#include <cstdlib>

using namespace DataProc;

Env_Helper::Env_Helper(DataNode* node)
    : _node(node)
{
    _nodeEnv = node->subscribe("Env");
}

int Env_Helper::set(const char* key, const char* value)
{
    DataProc::Env_Info_t envInfo;
    envInfo.key = key;
    envInfo.value = value;
    return _node->notify(_nodeEnv, &envInfo, sizeof(envInfo));
}

int Env_Helper::setInt(const char* key, int value)
{
    char buf[16];
    snprintf(buf, sizeof(buf), "%d", value);
    return set(key, buf);
}

const char* Env_Helper::get(const char* key, const char* def)
{
    DataProc::Env_Info_t envInfo;
    envInfo.key = key;
    _node->pull(_nodeEnv, &envInfo, sizeof(envInfo));
    return envInfo.value ? envInfo.value : def;
}

int Env_Helper::getInt(const char* key)
{
    return atoi(get(key, "0"));
}
