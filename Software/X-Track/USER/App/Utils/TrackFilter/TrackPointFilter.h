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
#ifndef __TRACK_POINT_FILTER_H
#define __TRACK_POINT_FILTER_H

#include <stdint.h>

class TrackPointFilter
{
public:
    typedef struct
    {
        double x;
        double y;
    } Point_t;

    typedef void (*Callback_t)(TrackPointFilter* filter, const Point_t* point);

public:
    void* userData;

public:
    TrackPointFilter();
    ~TrackPointFilter();
    void Reset();
    bool PushPoint(double x, double y)
    {
        Point_t point = { x, y };
        return PushPoint(&point);
    }
    bool PushPoint(const Point_t* point);
    void PushEnd();
    void SetOffsetThreshold(double offset);
    void SetOutputPointCallback(Callback_t callback);
    void SetSecondFilterModeEnable(bool en);
    void GetCounts(uint32_t* sum, uint32_t* output)
    {
        *sum = priv.pointCnt;
        *output = priv.pointOutputCnt;
    }

private:
    typedef struct
    {
        double a;
        double b;
        double c;
    } Line_t;

private:
    struct
    {
        double offsetThreshold;
        Callback_t outputCallback;
        Point_t tailPoint;
        Point_t prePoint;
        Line_t refLine;
        uint32_t pointCnt;
        uint32_t pointOutputCnt;
        bool secondFilterMode;
    } priv;

private:
    bool GetLine(Line_t* line, const Point_t* point0, const Point_t* point1);
    void GetVerticalLine(Line_t* verLine, const Line_t* oriLine, const Point_t* point);
    double GetOffset(const Line_t* line, const Point_t* point);
    bool GetIsOnSameSide(const Line_t* line, const Point_t* point0, const Point_t* point1);
    bool GetIsPointInLine(const Line_t* line, const Point_t* point);

    void DumpLine(const char* name, const Line_t* line);
    void DumpPoint(const char* name, const Point_t* point);
    void OutputPoint(const Point_t* point);
    double QuickSort(double num);
};

#endif
