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
#include "TrackLineFilter.h"
#include <string.h>
#include <math.h>

#define ABS(x)   (((x)>0)?(x):-(x))
#define SQ(x)    ((x)*(x))

#define TLF_USE_LOG 1

#if TLF_USE_LOG
#  include <stdio.h>
#  define LOG_PRINT(format, ...) printf(format, ##__VA_ARGS__),printf("\n")
#endif

TrackLineFilter::TrackLineFilter()
{
    memset(&priv, 0, sizeof(priv));
}

TrackLineFilter::~TrackLineFilter()
{

}

void TrackLineFilter::Reset()
{
    priv.lineCount = 0;
    priv.pointCnt = 0;
    priv.pointOutputCnt = 0;
    priv.inArea = false;
    SendEvent(EVENT_RESET, nullptr);
}

void TrackLineFilter::PushPoint(const Point_t* point)
{
    if (GetIsPointInArea(&priv.clipArea, point))
    {
        if (!priv.inArea)
        {
            const Point_t* p = priv.pointCnt > 0 ? &priv.prePoint : point;
            SendEvent(EVENT_START_LINE, p);
            priv.inArea = true;
        }

        OutputPoint(point);
    }
    else
    {
        if (priv.inArea)
        {
            SendEvent(EVENT_END_LINE, point);
            priv.lineCount++;
            priv.inArea = false;
        }
    }
    priv.prePoint = *point;
    priv.pointCnt++;
}

void TrackLineFilter::SetClipArea(int32_t x0, int32_t x1, int32_t y0, int32_t y1)
{
    priv.clipArea.x0 = x0;
    priv.clipArea.x1 = x1;
    priv.clipArea.y0 = y0;
    priv.clipArea.y1 = y1;
}

void TrackLineFilter::SetOutputPointCallback(Callback_t callback)
{
    priv.outputCallback = callback;
}

bool TrackLineFilter::GetIsPointInArea(const Area_t* area, const Point_t* point)
{
    return (point->x >= area->x0 && point->x <= area->x1
            && point->y >= area->y0 && point->y <= area->y1);
}

void TrackLineFilter::OutputPoint(const Point_t* point)
{
    SendEvent(EVENT_WRITE_POINT, point);
    priv.pointOutputCnt++;
}

void TrackLineFilter::SendEvent(EventCode_t code, const Point_t* point)
{
    if (!priv.outputCallback)
    {
        return;
    }

    Event_t event;
    event.code = code;
    event.lineIndex = priv.lineCount;
    event.point = point;
    priv.outputCallback(this, &event);
}
