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
#include "TrackPointFilter.h"
#include <cmath>
#include <float.h>
#include <string.h>

#define SQ(x) ((x) * (x))
#define FLOAT_0 FLT_EPSILON

#define TPF_USE_LOG 0
#define TPF_USE_LINE_CHECKED 0

#if TPF_USE_LOG
#include <stdio.h>
#define LOG_PRINT(format, ...) printf(format, ##__VA_ARGS__), printf("\n")
#else
#define LOG_PRINT(format, ...)
#endif

TrackPointFilter::TrackPointFilter(Callback_t callback, void* userData, double offsetThr)
    : _outputCallback(callback)
    , _userData(userData)
    , _offsetThreshold(offsetThr)
    , _tailPoint { 0 }
    , _prePoint { 0 }
    , _refLine { 0 }
    , _pointCnt(0)
    , _pointOutputCnt(0)
    , _secondFilterMode(false)
{
}

TrackPointFilter::~TrackPointFilter()
{
}

void TrackPointFilter::reset()
{
    _pointCnt = 0;
    _pointOutputCnt = 0;
}

bool TrackPointFilter::pushPoint(const Point_t* point)
{
    bool retval = false;
    dumpPoint("\n+P", point);
    if (_pointCnt == 0) {
        retval = true;
        outputPoint(point);
    } else if (_pointCnt == 1) {
        if (!getLine(&_refLine, &_prePoint, point)) {
            return false;
        }
        dumpLine("First", &_refLine);
    } else {
        dumpLine("--", &_refLine);

        double offset = getOffset(&_refLine, point);
        LOG_PRINT("OFS = %lf", offset);

        if (offset > _offsetThreshold) {
            LOG_PRINT("<---> offset detect!");

            retval = true;

            if (_secondFilterMode) {
                outputPoint(&_tailPoint);
            }

            outputPoint(&_prePoint);
            if (!getLine(&_refLine, &_prePoint, point)) {
                return false;
            }
        } else {
            Line_t line;
            if (!getLine(&line, &_tailPoint, &_prePoint)) {
                return false;
            }

            dumpLine("L", &line);
#if TPF_USE_LINE_CHECKED
            bool inLine1 = getIsPointInLine(&line, &_tailPoint);
            if (!inLine1) {
                dumpPoint("tailPoint", &_tailPoint);
                LOG_PRINT("not in L");
                while (1) { }
            }

            bool inLine2 = getIsPointInLine(&line, &_prePoint);
            if (!inLine2) {
                dumpPoint("prePoint", &_prePoint);
                LOG_PRINT("not in L");
                while (1) { }
            }
#endif

            Line_t verLine;
            getVerticalLine(&verLine, &line, &_prePoint);

            dumpLine("|", &verLine);
            dumpPoint("in", &_prePoint);

#if TPF_USE_LINE_CHECKED
            bool inLine3 = getIsPointInLine(&verLine, &_prePoint);
            if (!inLine3) {
                dumpPoint("prePoint", &_prePoint);
                LOG_PRINT("not in verLine");
                while (1) { }
            }
#endif

            if (getIsOnSameSide(&verLine, &_tailPoint, point)) {
                LOG_PRINT("~~~ direction change detect!");

                dumpPoint("p0", &_tailPoint);
                dumpPoint("p1", &_prePoint);
                dumpPoint("p2", point);

                retval = true;

                if (_secondFilterMode) {
                    outputPoint(&_tailPoint);
                }

                outputPoint(&_prePoint);
                if (!getLine(&_refLine, &_prePoint, point)) {
                    return false;
                }
            }
        }
    }

    _tailPoint = _prePoint;
    _prePoint = *point;
    _pointCnt++;

    return retval;
}

void TrackPointFilter::pushEnd()
{
    outputPoint(&_prePoint);
    reset();
}

void TrackPointFilter::setSecondFilterModeEnable(bool en)
{
    _secondFilterMode = en;
}

void TrackPointFilter::outputPoint(const Point_t* point)
{
    if (_outputCallback) {
        dumpPoint(">>> output", point);
        LOG_PRINT(" ");
        _outputCallback(this, point);
    }
    _pointOutputCnt++;
}

bool TrackPointFilter::getLine(Line_t* line, const Point_t* point0, const Point_t* point1)
{
    bool retval = true;

    double x0 = point0->x;
    double x1 = point1->x;
    double y0 = point0->y;
    double y1 = point1->y;

    double x_diff_abs = std::abs(x0 - x1);
    double y_diff_abs = std::abs(y0 - y1);

    double a = 0;
    double b = 0;
    double c = 0;

    if (x_diff_abs < FLOAT_0 && y_diff_abs > FLOAT_0) {
        a = 1;
        b = 0;
        c = -x0;
    } else if (x_diff_abs > FLOAT_0 && y_diff_abs < FLOAT_0) {
        a = 0;
        b = 1;
        c = -y0;
    } else if (x_diff_abs > FLOAT_0 && y_diff_abs > FLOAT_0) {
        a = (y1 - y0) / (x0 - x1);
        b = (a * (x0 - x1)) / (y1 - y0);
        c = 0 - a * x0 - b * y0;
    } else {
        retval = false;
    }

    line->a = a;
    line->b = b;
    line->c = c;

    return retval;
}

void TrackPointFilter::dumpLine(const char* name, const Line_t* line)
{
    LOG_PRINT(
        "%s : %lfx + %lfy + %lf = 0 { y = %lfx + %lf }",
        name, line->a, line->b, line->c,
        -line->a / line->b, -line->c / line->b);
}

void TrackPointFilter::dumpPoint(const char* name, const Point_t* point)
{
    LOG_PRINT("%s : (%lf, %lf)", name, point->x, point->y);
}

void TrackPointFilter::getVerticalLine(Line_t* verLine, const Line_t* oriLine, const Point_t* point)
{
    verLine->a = -oriLine->b;
    verLine->b = oriLine->a;
    verLine->c = 0 - verLine->a * point->x - verLine->b * point->y;
}

double TrackPointFilter::getOffset(const Line_t* line, const Point_t* point)
{
    double temp = line->a * point->x + line->b * point->y + line->c;
    double offset = std::abs(temp) * invSqrt(SQ(line->a) + SQ(line->b));
    return offset;
}

bool TrackPointFilter::getIsOnSameSide(const Line_t* line, const Point_t* point0, const Point_t* point1)
{
    bool retval = true;
    double side = (line->a * point0->x + line->b * point0->y + line->c)
        * (line->a * point1->x + line->b * point1->y + line->c);

    if (side < FLOAT_0) {
        retval = false;
    }

    return retval;
}

bool TrackPointFilter::getIsPointInLine(const Line_t* line, const Point_t* point)
{
    double result = line->a * point->x + line->b * point->y + line->c;
    return std::abs(result) < FLOAT_0;
}

double TrackPointFilter::invSqrt(double num)
{
    uint32_t i;
    float x2, y;
    const float threehalfs = 1.5f;
    x2 = (float)num * 0.5f;
    y = (float)num;
    i = *(uint32_t*)&y;
    i = 0x5f3759df - (i >> 1);
    y = *(float*)&i;
    y = y * (threehalfs - (x2 * y * y));
    y = y * (threehalfs - (x2 * y * y));
    return y;
}
