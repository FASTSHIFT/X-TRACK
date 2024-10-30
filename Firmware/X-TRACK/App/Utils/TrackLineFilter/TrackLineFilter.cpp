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
#include "TrackLineFilter.h"
#include <math.h>
#include <string.h>

#define SQ(x) ((x) * (x))

#define TLF_USE_LOG 0

#if TLF_USE_LOG
#include <stdio.h>
#define LOG_PRINT(format, ...) printf(format, ##__VA_ARGS__), printf("\n")
#else
#define LOG_PRINT(format, ...)
#endif

TrackLineFilter::TrackLineFilter(Callback_t callback, void* userData)
    : _outputCallback(callback)
    , _userData(userData)
    , _clipArea { 0 }
    , _prePoint { 0 }
    , _pointCnt(0)
    , _pointOutputCnt(0)
    , _isFirstPoint(false)
    , _isInArea(true)
{
}

TrackLineFilter::~TrackLineFilter()
{
}

void TrackLineFilter::reset()
{
    _pointCnt = 0;
    _pointOutputCnt = 0;
    _isFirstPoint = false;
    _isInArea = false;
}

void TrackLineFilter::pushPoint(const Point_t* point)
{
    checkPoint(point);
    next(point);
}

void TrackLineFilter::pushEnd()
{
    const Point_t* p = _pointCnt > 0 ? &_prePoint : nullptr;
    outputPoint(p);
}

void TrackLineFilter::setArea(const Area_t* area)
{
    _clipArea = *area;
}

bool TrackLineFilter::getIsPointInArea(const Point_t* point)
{
    bool inArea = (point->x >= _clipArea.x0
        && point->x <= _clipArea.x1
        && point->y >= _clipArea.y0
        && point->y <= _clipArea.y1);

    LOG_PRINT(
        "# Point(%d,%d) Area(%d,%d # %d,%d) inArea:%d",
        point->x, point->y,
        _clipArea.x0, _clipArea.y0,
        _clipArea.x1, _clipArea.y1,
        inArea);

    return inArea;
}

void TrackLineFilter::outputPoint(const Point_t* point)
{
    if (!_isInArea) {
        sendEvent(EVENT_ID::START_LINE, nullptr);
        _isInArea = true;
    }
    sendEvent(EVENT_ID::APPEND_POINT, point);
    _pointOutputCnt++;
}

void TrackLineFilter::next(const Point_t* point)
{
    _prePoint = *point;
    _pointCnt++;
}

bool TrackLineFilter::checkPoint(const Point_t* point)
{
    /* if line start, record first point */
    if (!_isFirstPoint) {
        _isFirstPoint = true;
        return false;
    }

    /* if two point all in area */
    if (getIsPointInArea(&_prePoint) && getIsPointInArea(point)) {
        outputPoint(point);
        return true;
    }

    /*

    p1 *------------------* p2
       |                  |
       |                  |
       |                  |
       |                  |
       |                  |
       |                  |
       |                  |
    p4 *------------------* p3

    */

    Point_t p1;
    p1.x = _clipArea.x0;
    p1.y = _clipArea.y0;

    Point_t p2;
    p2.x = _clipArea.x1;
    p2.y = _clipArea.y0;

    Point_t p3;
    p3.x = _clipArea.x1;
    p3.y = _clipArea.y1;

    Point_t p4;
    p4.x = _clipArea.x0;
    p4.y = _clipArea.y1;

    Point_t q1 = _prePoint;
    Point_t q2 = *point;

    /* check if line intersect with area */
    if (isIntersect(&p1, &p2, &q1, &q2)
        || isIntersect(&p2, &p3, &q1, &q2)
        || isIntersect(&p3, &p4, &q1, &q2)
        || isIntersect(&p4, &p1, &q1, &q2)) {
        outputPoint(&_prePoint);
        outputPoint(point);
        return true;
    }

    return false;
}

void TrackLineFilter::sendEvent(EVENT_ID id, const Point_t* point)
{
    if (!_outputCallback) {
        return;
    }

    Event_t event;
    event.eventID = id;
    event.point = point;
    _outputCallback(this, &event);
}

int64_t TrackLineFilter::crossProduct(const Point_t* a, const Point_t* b)
{
    return (int64_t)a->x * (int64_t)b->y - (int64_t)b->x * (int64_t)a->y;
}

bool TrackLineFilter::isIntersect(const Point_t* p1, const Point_t* p2, const Point_t* q1, const Point_t* q2)
{
    Point_t u = { p2->x - p1->x, p2->y - p1->y };
    Point_t v = { q2->x - q1->x, q2->y - q1->y };
    Point_t w = { p1->x - q1->x, p1->y - q1->y };
    int64_t t = crossProduct(&u, &v);
    int64_t s1 = crossProduct(&w, &u);
    int64_t s2 = crossProduct(&w, &v);

    if (t > 0) {
        return s1 >= 0 && s1 <= t && s2 >= 0 && s2 <= t;
    }

    if (t < 0) {
        return s1 <= 0 && s1 >= t && s2 <= 0 && s2 >= t;
    }

    return s1 == 0 || s2 == 0;
}
