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
#include "StatusBar.h"
#include "Frameworks/PageManager/PageManager.h"

#define IS_STR_EQ(STR1, STR2) (strcmp(STR1, STR2) == 0)

using namespace Page;

StatusBar::StatusBar(void* arg)
{
    auto parent = lv_layer_top();

    if (arg) {
        auto manager = (PageManager*)arg;
        auto rootParent = manager->getRootParent();

        /* If the root parent is not the active screen, create a new object for the status bar */
        if (rootParent != lv_scr_act()) {
            parent = lv_obj_create(lv_layer_top());
            lv_obj_remove_style_all(parent);
            lv_obj_remove_flag(parent, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_remove_flag(parent, LV_OBJ_FLAG_CLICKABLE);

            /* Set the parent's style to the root parent's style */
            lv_obj_set_style_align(parent, lv_obj_get_style_align(rootParent, 0), 0);
            lv_obj_set_pos(parent, lv_obj_get_style_x(rootParent, 0), lv_obj_get_style_y(rootParent, 0));
            lv_obj_set_size(parent, lv_obj_get_style_width(rootParent, 0), lv_obj_get_style_height(rootParent, 0));
        }
    }

    _model = new StatusBarModel(this);
    _view = new StatusBarView(this, parent);
}

StatusBar::~StatusBar()
{
    delete _view;
    delete _model;
}

void StatusBar::onModelEvent(StatusBarModel::EVENT_ID id, const void* param)
{
    switch (id) {
    case StatusBarModel::EVENT_ID::CLOCK: {
        _view->publish(StatusBarView::MSG_ID::CLOCK, param);
    } break;

    case StatusBarModel::EVENT_ID::GNSS: {
        _view->publish(StatusBarView::MSG_ID::SAT_NUM, param);
    } break;

    case StatusBarModel::EVENT_ID::POWER: {
        _view->publish(StatusBarView::MSG_ID::POWER, param);
    } break;

    case StatusBarModel::EVENT_ID::ENV: {
        auto envInfo = (const DataProc::Env_Info_t*)param;
        if (IS_STR_EQ(envInfo->key, "statusbar-opa")) {
            StatusBarView::STYLE_ID style = IS_STR_EQ(envInfo->value, "light")
                ? StatusBarView::STYLE_ID::LIGHT
                : StatusBarView::STYLE_ID::DARK;
            _view->publish(StatusBarView::MSG_ID::STYLE_CHANGE, &style);
        } else if (IS_STR_EQ(envInfo->key, "rec")) {
            _view->publish(StatusBarView::MSG_ID::REC_CHANGE, envInfo->value);
        } else if (IS_STR_EQ(envInfo->key, "statusbar")) {
            bool show = IS_STR_EQ(envInfo->value, "enable");
            _view->publish(StatusBarView::MSG_ID::SHOW, &show);
        }
    } break;

    default:
        break;
    }
}

void StatusBar::onViewEvent(StatusBarView::EVENT_ID id, const void* param)
{
    switch (id) {
    case StatusBarView::EVENT_ID::HEIGHT_UPDATE: {
        auto height = *(int*)param;
        _model->env()->setInt("statusbar-padding-top", height);
    } break;

    default:
        break;
    }
}
