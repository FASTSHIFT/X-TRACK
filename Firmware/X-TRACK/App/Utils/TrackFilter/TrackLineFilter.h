/*
 * MIT License
 * Copyright (c) 2021 _VIFEXTech
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

class TrackLineFilter
{
public:
    typedef struct
    {
        int32_t x;
        int32_t y;
    } Point_t;

    typedef enum
    {
        EVENT_START_LINE,
        EVENT_APPEND_POINT,
        EVENT_END_LINE,
        EVENT_RESET
    } EventCode_t;

    typedef struct
    {
        EventCode_t code;
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

    typedef void (*Callback_t)(TrackLineFilter* filter, Event_t* event);

public:
    void* userData;

public:
    TrackLineFilter();
    ~TrackLineFilter();

    void Reset();
    void PushPoint(int32_t x, int32_t y)
    {
        Point_t point = { x, y };
        PushPoint(&point);
    }
    void PushPoint(const Point_t* point);
    void PushPointForce(int32_t x, int32_t y)
    {
        Point_t point = { x, y };
        PushPointForce(&point);
    }
    void PushPointForce(const Point_t* point);
    void PushEnd();

    void SetClipArea(const Area_t* area);
    void SetOutputPointCallback(Callback_t callback);

private:
    struct
    {
        Callback_t outputCallback;
        Area_t clipArea;
        bool inArea;
        Point_t prePoint;
        uint32_t lineCount;
        uint32_t pointCnt;
        uint32_t pointOutputCnt;
    } priv;

private:
    bool GetIsPointInArea(const Area_t* area, const Point_t* point);
    void OutputPoint(const Point_t* point);
    void SendEvent(EventCode_t code, const Point_t* point);
};

#endif
