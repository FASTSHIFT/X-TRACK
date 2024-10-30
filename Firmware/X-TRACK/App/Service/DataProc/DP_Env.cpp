/*
 * MIT License
 * Copyright (c) 2021 - 2023 _VIFEXTech
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
#include "DataProc.h"
#include "Utils/WString/WString.h"
#include <vector>

using namespace DataProc;

class DP_Env {
public:
    DP_Env(DataNode* node);

private:
    typedef struct {
        String key;
        String value;
    } key_pair_t;

private:
    DataNode* _node;
    std::vector<key_pair_t> _keyPairs;

private:
    int onEvent(DataNode::EventParam_t* param);
    int onSet(const Env_Info_t* info);
    int onGet(Env_Info_t* info);
};

DP_Env::DP_Env(DataNode* node)
    : _node(node)
{
    node->setEventCallback(
        [](DataNode* n, DataNode::EventParam_t* param) {
            auto ctx = (DP_Env*)n->getUserData();
            return ctx->onEvent(param);
        },
        DataNode::EVENT_NOTIFY | DataNode::EVENT_PULL);
}

int DP_Env::onEvent(DataNode::EventParam_t* param)
{
    if (param->size != sizeof(Env_Info_t)) {
        return DataNode::RES_SIZE_MISMATCH;
    }

    auto info = (Env_Info_t*)param->data_p;

    switch (param->event) {
    case DataNode::EVENT_NOTIFY: {
        auto res = onSet(info);
        if (res == DataNode::RES_OK) {
            _node->publish(info, sizeof(Env_Info_t));
        }
        return res;
    }

    case DataNode::EVENT_PULL:
        return onGet(info);

    default:
        break;
    }

    return DataNode::RES_UNSUPPORTED_REQUEST;
}

int DP_Env::onSet(const Env_Info_t* info)
{
    for (auto iter = _keyPairs.begin(); iter != _keyPairs.end(); ++iter) {
        if (iter->key == info->key) {
            iter->value = info->value;
            return DataNode::RES_OK;
        }
    }

    key_pair_t pair = { info->key, info->value };
    _keyPairs.push_back(pair);

    return DataNode::RES_OK;
}

int DP_Env::onGet(Env_Info_t* info)
{
    for (auto iter = _keyPairs.begin(); iter != _keyPairs.end(); ++iter) {
        if (iter->key == info->key) {
            info->value = iter->value.c_str();
            return DataNode::RES_OK;
        }
    }

    info->value = nullptr;
    return DataNode::RES_NO_DATA;
}

DATA_PROC_DESCRIPTOR_DEF(Env)
