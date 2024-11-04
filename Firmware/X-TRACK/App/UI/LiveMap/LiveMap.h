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
#ifndef __LIVE_MAP_PRESENTER_H
#define __LIVE_MAP_PRESENTER_H

#include "LiveMapModel.h"
#include "LiveMapView.h"

namespace Page {

class LiveMap : public PageBase, public LiveMapModel::EventListener, public LiveMapView::EventListener {
public:
    LiveMap();
    virtual ~LiveMap();

    virtual void onInstalled();
    virtual void onViewLoad();
    virtual void onViewDidLoad();
    virtual void onViewWillAppear();
    virtual void onViewDidAppear();
    virtual void onViewWillDisappear();
    virtual void onViewDidDisappear();
    virtual void onViewUnload();
    virtual void onViewDidUnload();

private:
    LiveMapModel* _model;
    LiveMapView* _view;

private:
    virtual void onModelEvent(LiveMapModel::EVENT_ID id, const void* param) override;
    virtual void onViewEvent(LiveMapView::EVENT_ID id, const void* param) override;
};

}

#endif
