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
#include "Dashboard.h"
#include <cmath>

using namespace Page;

APP_DESCRIPTOR_DEF(Dashboard);

Dashboard::Dashboard()
    : _model(nullptr)
    , _view(nullptr)
{
}

Dashboard::~Dashboard()
{
}

void Dashboard::onInstalled()
{
    setLoadAnimType(PAGE_ANIM::NONE);
}

void Dashboard::onViewLoad()
{
    _model = new DashboardModel(this);
    _view = new DashboardView(this, getRoot());
}

void Dashboard::onViewDidLoad()
{
}

void Dashboard::onViewWillAppear()
{
}

void Dashboard::onViewDidAppear()
{
    Param_t param;
    param.animEnable = false,

    PAGE_GET_PARAM(param);
    if (param.animEnable) {
        _view->publish(DashboardView::MSG_ID::ANIM_START);
    }
}

void Dashboard::onViewWillDisappear()
{
}

void Dashboard::onViewDidDisappear()
{
}

void Dashboard::onViewUnload()
{
    delete _model;
    delete _view;
}

void Dashboard::onViewDidUnload()
{
}

void Dashboard::onModelEvent(DashboardModel::EVENT_ID id, const void* param)
{
    switch (id) {
    case DashboardModel::EVENT_ID::SPORT_STATUS: {
        _view->publish(DashboardView::MSG_ID::SPORT_STATUS, param);
    } break;

    case DashboardModel::EVENT_ID::RECORDER_STATUS: {
        _view->publish(DashboardView::MSG_ID::RECORDER_STATUS, param);
    } break;

    case DashboardModel::EVENT_ID::GNSS: {
        _view->publish(DashboardView::MSG_ID::GNSS_STATUS, param);
    } break;

    default:
        break;
    }
}

void Dashboard::onViewEvent(DashboardView::EVENT_ID id, const void* param)
{
    switch (id) {
    case DashboardView::EVENT_ID::GET_BINDING: {
        auto binding = (DashboardView::Binding_Info_t*)param;
        binding->binding = _model->getBinding((DashboardModel::BINDING_TYPE)binding->type);
    } break;

    case DashboardView::EVENT_ID::NAVI_TO_PAGE: {
        auto pageID = (const char*)param;
        getManager()->push(pageID);
    }; break;

    default:
        break;
    }
}
