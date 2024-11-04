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
#include "Frameworks/PageManager/PageManager.h"

#define IS_STR_EQ(STR1, STR2) (strcmp(STR1, STR2) == 0)

using namespace DataProc;

class DP_PageNavi {
public:
    DP_PageNavi(DataNode* node);

private:
    DataNode* _node;
    const DataNode* _nodeGlobal;
    Env_Helper _env;
    PageManager* _manager;
    bool _enable;
    lv_style_t _pageStyle;

private:
    int onEvent(DataNode::EventParam_t* param);
    int onNotify(const PageNavi_Info_t* info);
    int onGlobalEvent(const Global_Info_t* info);
    int naviToPage(PAGE_NAVI_CMD cmd, const char* name = nullptr);
    static PageManager::RES_TYPE onPageEvent(PageManager::EVENT* event);
};

DP_PageNavi::DP_PageNavi(DataNode* node)
    : _node(node)
    , _env(node)
{
    _nodeGlobal = node->subscribe("Global");

    node->setEventCallback(
        [](DataNode* n, DataNode::EventParam_t* param) {
            auto ctx = (DP_PageNavi*)n->getUserData();
            return ctx->onEvent(param);
        },
        DataNode::EVENT_PUBLISH | DataNode::EVENT_NOTIFY);
}

int DP_PageNavi::onEvent(DataNode::EventParam_t* param)
{
    if (param->event == DataNode::EVENT_NOTIFY) {
        if (param->size != sizeof(PageNavi_Info_t)) {
            return DataNode::RES_SIZE_MISMATCH;
        }

        return onNotify((const PageNavi_Info_t*)param->data_p);
    }

    if (param->tran == _env) {
        auto info = (const Env_Info_t*)param->data_p;
        if (IS_STR_EQ(info->key, "pagenavi")) {
            _enable = IS_STR_EQ(info->value, "enable");
        }
    }

    if (param->tran == _nodeGlobal) {
        return onGlobalEvent((const Global_Info_t*)param->data_p);
    }

    if (!_enable) {
        return DataNode::RES_UNSUPPORTED_REQUEST;
    }

    return DataNode::RES_UNSUPPORTED_REQUEST;
}

int DP_PageNavi::onNotify(const PageNavi_Info_t* info)
{
    auto retval = PageManager::RES_TYPE::ERR_UNKNOWN;

    switch (info->cmd) {
    case PAGE_NAVI_CMD::PUSH: {
        retval = _manager->push(info->name);
    } break;

    case PAGE_NAVI_CMD::POP: {
        retval = _manager->pop();
    } break;

    default:
        break;
    }

    return retval == PageManager::RES_TYPE::OK ? DataNode::RES_OK : DataNode::RES_UNSUPPORTED_REQUEST;
}

int DP_PageNavi::onGlobalEvent(const Global_Info_t* info)
{
    switch (info->event) {
    case GLOBAL_EVENT::PAGE_MANAGER_INIT_FINISHED: {
        _manager = (PageManager*)info->param;

        /* Screen style */
        lv_obj_t* scr = lv_scr_act();
        lv_obj_set_style_pad_all(scr, 0, 0);
        lv_obj_clear_flag(scr, LV_OBJ_FLAG_SCROLLABLE);
        _manager->setRootParent(scr);

        /* Page style */
        lv_style_init(&_pageStyle);
        lv_style_set_size(&_pageStyle, lv_pct(100), lv_pct(100));
        lv_style_set_pad_all(&_pageStyle, 0);
        lv_style_set_radius(&_pageStyle, 0);
        lv_style_set_border_width(&_pageStyle, 0);
        lv_style_set_text_font(&_pageStyle, lv_theme_get_font_normal(nullptr));
        _manager->setRootDefaultStyle(&_pageStyle);

        /* Page event hooker */
        _manager->setEventCallback(onPageEvent, this);
    } break;

    case GLOBAL_EVENT::STATUS_BAR_INIT_FINISHED: {
        /* Update StatusBar padding */
        lv_style_set_pad_top(&_pageStyle, _env.getInt("statusbar-padding-top"));
    } break;

    case GLOBAL_EVENT::APP_STARTED: {
        /* Open page */
        _manager->push("Dashboard");
    } break;

    case GLOBAL_EVENT::APP_STOPPED: {
        /* Close page */
        _manager->setRootDefaultStyle(nullptr);
        lv_style_reset(&_pageStyle);
    } break;

    default:
        break;
    }

    return DataNode::RES_OK;
}

int DP_PageNavi::naviToPage(PAGE_NAVI_CMD cmd, const char* name)
{
    PageNavi_Info_t info;
    info.cmd = cmd;
    info.name = name;
    return onNotify(&info);
}

PageManager::RES_TYPE DP_PageNavi::onPageEvent(PageManager::EVENT* event)
{
    auto self = (DP_PageNavi*)event->userData;

    switch (event->type) {
    case PageManager::EVENT_TYPE::PAGE_STATE_CHANGED: {
        auto state = *(const PageBase::STATE*)event->param;

        /* Set default env when page did appear */
        if (state == PageBase::STATE::DID_APPEAR) {
            self->_env.set("statusbar-opa", "cover");
            self->_env.set("navibar", "enable");
            self->_env.set("statusbar", "enable");
            self->_env.set("pagenavi", "enable");
        }
    } break;

    case PageManager::EVENT_TYPE::PAGE_PUSH:
    case PageManager::EVENT_TYPE::PAGE_POP: {
        PageNavi_Info_t info;
        info.cmd = event->type == PageManager::EVENT_TYPE::PAGE_PUSH ? PAGE_NAVI_CMD::PUSH : PAGE_NAVI_CMD::POP;
        info.name = (const char*)event->param;
        auto res = self->_node->publish(&info, sizeof(info));

        /* if the page navigation is intercept, stop process */
        if (res == DataNode::RES_STOP_PROCESS) {
            return PageManager::RES_TYPE::STOP_PROCESS;
        }

        return PageManager::RES_TYPE::OK;
    } break;

    default:
        break;
    }

    return PageManager::RES_TYPE::OK;
}

DATA_PROC_DESCRIPTOR_DEF(PageNavi);
