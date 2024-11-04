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
#include "SystemInfos.h"

using namespace Page;

APP_DESCRIPTOR_DEF(SystemInfos);

SystemInfos::SystemInfos()
    : _model(nullptr)
    , _view(nullptr)
{
}

SystemInfos::~SystemInfos()
{
}

void SystemInfos::onInstalled()
{
    setBackGestureDirection(LV_DIR_RIGHT);
}

void SystemInfos::onViewLoad()
{
    _model = new SystemInfosModel(this);
    _view = new SystemInfosView(this, getRoot());
}

void SystemInfos::onViewDidLoad()
{
}

void SystemInfos::onViewWillAppear()
{
}

void SystemInfos::onViewDidAppear()
{
    _model->env()->set("statusbar-opa", "light");
}

void SystemInfos::onViewWillDisappear()
{
}

void SystemInfos::onViewDidDisappear()
{
}

void SystemInfos::onViewUnload()
{
    delete _model;
    delete _view;
}

void SystemInfos::onViewDidUnload()
{
}

void SystemInfos::onModelEvent(SystemInfosModel::EVENT_ID id, const void* param)
{
    switch (id) {
    case SystemInfosModel::EVENT_ID::SPORT_STATUS: {
        _view->publish(SystemInfosView::MSG_ID::SPORT_STATUS, param);
    } break;
    case SystemInfosModel::EVENT_ID::GNSS: {
        _view->publish(SystemInfosView::MSG_ID::GNSS, param);
    } break;
    case SystemInfosModel::EVENT_ID::CLOCK: {
        _view->publish(SystemInfosView::MSG_ID::CLOCK, param);
    } break;
    case SystemInfosModel::EVENT_ID::POWER: {
        _view->publish(SystemInfosView::MSG_ID::POWER, param);
    } break;
    default:
        break;
    }
}

void SystemInfos::onViewEvent(SystemInfosView::EVENT_ID id, const void* param)
{
    switch (id) {
    case SystemInfosView::EVENT_ID::GET_BINDING: {
        auto binding = (SystemInfosView::Binding_Info_t*)param;
        binding->binding = _model->getBinding((SystemInfosModel::BINDING_TYPE)binding->type);
    } break;

    default:
        break;
    }
}
