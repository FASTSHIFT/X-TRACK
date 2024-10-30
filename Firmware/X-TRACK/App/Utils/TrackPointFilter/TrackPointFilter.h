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
#ifndef __TRACK_POINT_FILTER_H
#define __TRACK_POINT_FILTER_H

#include <stdint.h>

class TrackPointFilter {
public:
    typedef struct
    {
        double x;
        double y;
    } Point_t;

    typedef void (*Callback_t)(TrackPointFilter* filter, const Point_t* point);

public:
    TrackPointFilter(Callback_t callback = nullptr, void* userData = nullptr, double offsetThr = 2);
    ~TrackPointFilter();

    void reset();
    bool pushPoint(double x, double y)
    {
        Point_t point = { x, y };
        return pushPoint(&point);
    }
    bool pushPoint(const Point_t* point);
    void pushEnd();

    void setSecondFilterModeEnable(bool en);
    void getCounts(uint32_t* sum, uint32_t* output)
    {
        *sum = _pointCnt;
        *output = _pointOutputCnt;
    }
    void* getUserData()
    {
        return _userData;
    }

private:
    typedef struct
    {
        double a;
        double b;
        double c;
    } Line_t;

private:
    Callback_t _outputCallback;
    void* _userData;
    double _offsetThreshold;
    Point_t _tailPoint;
    Point_t _prePoint;
    Line_t _refLine;
    uint32_t _pointCnt;
    uint32_t _pointOutputCnt;
    bool _secondFilterMode;

private:
    bool getLine(Line_t* line, const Point_t* point0, const Point_t* point1);
    void getVerticalLine(Line_t* verLine, const Line_t* oriLine, const Point_t* point);
    double getOffset(const Line_t* line, const Point_t* point);
    bool getIsOnSameSide(const Line_t* line, const Point_t* point0, const Point_t* point1);
    bool getIsPointInLine(const Line_t* line, const Point_t* point);

    void dumpLine(const char* name, const Line_t* line);
    void dumpPoint(const char* name, const Point_t* point);
    void outputPoint(const Point_t* point);
    double invSqrt(double num);
};

#endif
