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
#include "Config/Config.h"
#include "DataProc.h"
#include "Frameworks/PageManager/PageManager.h"
#include "UI/Resource/ResourcePool.h"
#include "Utils/lv_toast/lv_toast.h"

using namespace DataProc;

class DP_Toast {
public:
    DP_Toast(DataNode* node);

private:
    const DataNode* _nodeGlobal;
    lv_obj_t* _toast;
    lv_style_t _style;
    ResourcePool::Font _font;

private:
    int onEvent(DataNode::EventParam_t* param);
};

DP_Toast::DP_Toast(DataNode* node)
    : _toast(nullptr)
    , _font(16, "bold")
{
    _nodeGlobal = node->subscribe("Global");

    lv_style_init(&_style);
    lv_style_set_radius(&_style, LV_RADIUS_CIRCLE);
    lv_style_set_margin_top(&_style, 10);
    lv_style_set_bg_color(&_style, lv_color_black());
    lv_style_set_bg_opa(&_style, LV_OPA_60);
    lv_style_set_border_width(&_style, 0);
    lv_style_set_text_color(&_style, lv_color_white());
    lv_style_set_text_font(&_style, _font);

    _toast = lv_toast_create(lv_layer_top());
    lv_toast_set_cont_style(_toast, &_style);

    node->setEventCallback(
        [](DataNode* n, DataNode::EventParam_t* param) {
            auto ctx = (DP_Toast*)n->getUserData();
            return ctx->onEvent(param);
        },
        DataNode::EVENT_NOTIFY | DataNode::EVENT_PUBLISH);
}

int DP_Toast::onEvent(DataNode::EventParam_t* param)
{
    switch (param->event) {
    case DataNode::EVENT_NOTIFY: {
        if (param->size != sizeof(Toast_Info_t)) {
            return DataNode::RES_SIZE_MISMATCH;
        }

        auto info = (const Toast_Info_t*)param->data_p;
        lv_toast_show_text(_toast, info->txt, info->duration);
    } break;

    case DataNode::EVENT_PUBLISH: {
        if (param->tran == _nodeGlobal) {
            auto info = (const Global_Info_t*)param->data_p;
            if (info->event == GLOBAL_EVENT::PAGE_MANAGER_INIT_FINISHED) {
                auto manager = (PageManager*)info->param;

                /* Change the parent of the toast to the top layer of the page manager */
                lv_obj_set_parent(_toast, manager->getLayerTop());
            }
        }
    } break;

    default:
        return DataNode::RES_UNSUPPORTED_REQUEST;
    }

    return DataNode::RES_OK;
}

DATA_PROC_DESCRIPTOR_DEF(Toast)
