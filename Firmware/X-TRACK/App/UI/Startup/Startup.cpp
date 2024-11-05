/*
 * MIT License
 * Copyright (c) 2023 _VIFEXTech
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
#include "Startup.h"
#include "UI/Dashboard/Dashboard.h"

using namespace Page;

APP_DESCRIPTOR_DEF(Startup);

Startup::Startup()
    : _model(nullptr)
    , _view(nullptr)
{
}

Startup::~Startup()
{
}

void Startup::onInstalled()
{
    setCacheEnable(false);
    setLoadAnimType(PAGE_ANIM::FADE_ON);
}

void Startup::onViewLoad()
{
    _model = new StartupModel(this);
    _view = new StartupView(this, getRoot());
}

void Startup::onViewDidLoad()
{
}

void Startup::onViewWillAppear()
{
    Param_t param = { 0 };
    PAGE_GET_PARAM(param);

    _view->publish(StartupView::MSG_ID::SHOW, (void*)(lv_uintptr_t)!param.shutdown);

    /* load ephemeris when animation is done */
    if (!param.shutdown) {
        lv_timer_t* timer = lv_timer_create(
            [](lv_timer_t* tmr) {
                auto self = (Startup*)lv_timer_get_user_data(tmr);
                self->readyToExit();
            },
            1500, this);
        lv_timer_set_repeat_count(timer, 1);
    }
}

void Startup::onViewDidAppear()
{
    _model->env()->set("navibar", "disable");
    _model->env()->set("statusbar", "disable");
    _model->env()->set("pagenavi", "disable");
}

void Startup::onViewWillDisappear()
{
}

void Startup::onViewDidDisappear()
{
}

void Startup::onViewUnload()
{
    delete _model;
    delete _view;
}

void Startup::onViewDidUnload()
{
}

void Startup::onModelEvent(StartupModel::EVENT_ID id, const void* param)
{
}

void Startup::onViewEvent(StartupView::EVENT_ID id, const void* param)
{
}

void Startup::readyToExit()
{
    /* wait fade out animation done */
    lv_timer_t* timer = lv_timer_create(
        [](lv_timer_t* tmr) {
            auto self = (Startup*)lv_timer_get_user_data(tmr);
            self->onExitTimer();
        },
        500, this);
    lv_timer_set_repeat_count(timer, 1);
}

void Startup::onExitTimer()
{
    Dashboard::Param_t dashBoardParam;
    dashBoardParam.animEnable = true;
    PARAM param = PAGE_PARAM_MAKE(dashBoardParam);
    getManager()->replace("Dashboard", &param);
}
