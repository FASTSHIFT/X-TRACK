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
#include "Config/Config.h"
#include "DataProc.h"
#include "Service/i18n/lv_i18n.h"
#include "lvgl/lvgl.h"

#define IS_STR_EQ(STR1, STR2) (strcmp(STR1, STR2) == 0)
#define LANGUAGE_STR "language"

using namespace DataProc;

class DP_i18n {
public:
    DP_i18n(DataNode* node);

private:
    DataNode* _node;
    const DataNode* _nodeEnv;
    const DataNode* _nodeStorage;
    char _language[8];

private:
    int onEvent(DataNode::EventParam_t* param);
};

DP_i18n::DP_i18n(DataNode* node)
    : _node(node)
    , _nodeEnv(nullptr)
    , _language { CONFIG_LANGUAGE_DEFAULT }
{
    lv_i18n_init(lv_i18n_language_pack);

    _nodeEnv = node->subscribe("Env");
    _nodeStorage = node->subscribe("Storage");
    node->setEventCallback(
        [](DataNode* n, DataNode::EventParam_t* param) {
            auto ctx = (DP_i18n*)n->getUserData();
            return ctx->onEvent(param);
        },
        DataNode::EVENT_PUBLISH);

    Storage_Helper storage(node);
    storage.add(LANGUAGE_STR, _language, sizeof(_language), STORAGE_TYPE::STRING);

    if (lv_i18n_set_locale(_language) != 0) {
        LV_LOG_ERROR("set locale '%s' failed", _language);
    }
}

int DP_i18n::onEvent(DataNode::EventParam_t* param)
{
    if (param->tran == _nodeEnv) {
        auto info = (Env_Info_t*)param->data_p;
        if (IS_STR_EQ(info->key, LANGUAGE_STR)) {
            if (lv_i18n_set_locale(info->value) != 0) {
                LV_LOG_ERROR("set locale '%s' failed", info->value);
                return DataNode::RES_PARAM_ERROR;
            }

            /* fix strncpy-param-overlap */
            if (info->value != _language) {
                strncpy(_language, info->value, sizeof(_language) - 1);
            }
            LV_LOG_USER("set locale to %s", info->value);
        }
    } else if (param->tran == _nodeStorage) {
        auto info = (Storage_Info_t*)param->data_p;
        /* load language */
        if (info->cmd == STORAGE_CMD::LOAD) {
            Env_Info_t envInfo;
            envInfo.key = LANGUAGE_STR;
            envInfo.value = _language;
            _node->notify(_nodeEnv, &envInfo, sizeof(Env_Info_t));
        }
    }
    return DataNode::RES_OK;
}

DATA_PROC_DESCRIPTOR_DEF(i18n)
