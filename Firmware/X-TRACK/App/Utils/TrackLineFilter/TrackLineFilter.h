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
#ifndef __TRACK_LINE_FILTER_H
#define __TRACK_LINE_FILTER_H

#include <stdint.h>

class TrackLineFilter {
public:
    typedef struct
    {
        int32_t x;
        int32_t y;
    } Point_t;

    enum class EVENT_ID {
        START_LINE,
        APPEND_POINT,
    };

    typedef struct
    {
        EVENT_ID eventID;
        uint32_t lineIndex;
        const Point_t* point;
    } Event_t;

    typedef struct
    {
        int32_t x0;
        int32_t y0;
        int32_t x1;
        int32_t y1;
    } Area_t;

    typedef void (*Callback_t)(TrackLineFilter* filter, const Event_t* event);

public:
public:
    TrackLineFilter(Callback_t callback, void* userData);
    ~TrackLineFilter();

    void reset();
    void pushPoint(int32_t x, int32_t y)
    {
        Point_t point = { x, y };
        pushPoint(&point);
    }
    void pushPoint(const Point_t* point);
    void pushEnd();

    void setArea(const Area_t* area);
    const Area_t* getArea() { return &_clipArea; }
    bool getIsPointInArea(const Point_t* point);
    void* getUserData() { return _userData; }

private:
    Callback_t _outputCallback;
    void* _userData;
    Area_t _clipArea;
    Point_t _prePoint;
    uint32_t _pointCnt;
    uint32_t _pointOutputCnt;
    bool _isFirstPoint;
    bool _isInArea;

private:
    void next(const Point_t* point);
    bool checkPoint(const Point_t* point);
    void outputPoint(const Point_t* point);
    void sendEvent(EVENT_ID id, const Point_t* point);
    int64_t crossProduct(const Point_t* a, const Point_t* b);
    bool isIntersect(const Point_t* p1, const Point_t* p2, const Point_t* q1, const Point_t* q2);
};

#endif
