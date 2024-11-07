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
#include "Shutdown.h"
#include "../Startup/Startup.h"

using namespace Page;

APP_DESCRIPTOR_DEF(Shutdown);

Shutdown::Shutdown()
    : _model(nullptr)
    , _view(nullptr)
{
}

Shutdown::~Shutdown()
{
}

void Shutdown::onInstalled()
{
    setLoadAnimType(PAGE_ANIM::OVER_TOP);
}

void Shutdown::onViewLoad()
{
    _model = new ShutdownModel(this);
    _view = new ShutdownView(this, getRoot());
}

void Shutdown::onViewDidLoad()
{
}

void Shutdown::onViewWillAppear()
{
    _model->setInterceptPageNavi(true);
    _model->feedback()->trigger(DataProc::FEEDBACK_GEN_EFFECT::NOTIFICATION_WARNING);
}

void Shutdown::onViewDidAppear()
{
    _model->env()->set("statusbar", "disable");
    _model->env()->set("navibar", "enable");
    _model->env()->set("pagenavi", "enable");
}

void Shutdown::onViewWillDisappear()
{
}

void Shutdown::onViewDidDisappear()
{
    _model->setInterceptPageNavi(false);
}

void Shutdown::onViewUnload()
{
    delete _model;
    delete _view;
}

void Shutdown::onViewDidUnload()
{
}

void Shutdown::onModelEvent(ShutdownModel::EVENT_ID id, const void* param)
{
    switch (id) {
    case ShutdownModel::EVENT_ID::PAGE_NAVI: {
        _model->feedback()->trigger(DataProc::FEEDBACK_GEN_EFFECT::IMPACT_LIGHT);
        getManager()->pop();
    } break;

    default:
        break;
    }
}

void Shutdown::onViewEvent(ShutdownView::EVENT_ID id, const void* param)
{
    switch (id) {
    case ShutdownView::EVENT_ID::READY:
        _model->feedback()->trigger(DataProc::FEEDBACK_GEN_EFFECT::SELECTION);
        break;
    case ShutdownView::EVENT_ID::SHUTDOWN: {
        if(_model->checkShutdownDisable()) {
            LV_LOG_WARN("Shutdown disabled by user");
            return;
        }

        _model->setInterceptPageNavi(false);

        /* Let Startup page show shoutdown animation */
        Startup::Param_t startupParam;
        startupParam.shutdown = true;
        PARAM pageParam = PAGE_PARAM_MAKE(startupParam);
        getManager()->push("Startup", &pageParam);

        _model->feedback()->trigger(DataProc::FEEDBACK_GEN_EFFECT::NOTIFICATION_ERROR);
        _model->env()->set("navibar", "disable");
        _model->shutdown();
    } break;
    case ShutdownView::EVENT_ID::EXIT:
        getManager()->pop();
        break;

    default:
        break;
    }
}
