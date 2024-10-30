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
#ifndef __TRACK_VIEW_H
#define __TRACK_VIEW_H

#include "Utils/TrackLineFilter/TrackLineFilter.h"
#include "Utils/TrackPointFilter/TrackPointFilter.h"
#include "lvgl/lvgl.h"

class PointContainer;
class lv_poly_line;

class TrackView {
public:
    TrackView(lv_obj_t* parent);
    ~TrackView();

    void setStyle(lv_style_t* style);
    void setArea(int32_t x0, int32_t y0, int32_t x1, int32_t y1);
    void setActivePoint(int32_t x, int32_t y);
    void pushPoint(int32_t x, int32_t y);
    void setLevel(int level);
    void setPointContainer(PointContainer* pointCont, int refLevel);
    bool reload();
    lv_obj_t* getViewCont() { return _cont; }
    int getLevel() { return _curLevel; }
    int getRefLevel() { return _refLevel; }

private:
    typedef struct
    {
        int32_t x;
        int32_t y;
    } Point_t;

private:
    lv_obj_t* _cont;
    lv_poly_line* _trackLine;
    lv_obj_t* _activeLine;
    lv_point_precise_t _activePoint[2];
    PointContainer* _pointContainer;
    int _refLevel;
    int _curLevel;

    TrackPointFilter _pointFilter;
    TrackLineFilter _lineFilter;

private:
    Point_t getOffset(int32_t x, int32_t y);
    Point_t convertPoint(int32_t srcX, int32_t srcY, int srcLevel, int destLevel);
    static void onPointFilterEvent(TrackPointFilter* filter, const TrackPointFilter::Point_t* point);
    static void onLineFilterEvent(TrackLineFilter* filter, const TrackLineFilter::Event_t* event);
};

#endif // __TRACK_VIEW_H
