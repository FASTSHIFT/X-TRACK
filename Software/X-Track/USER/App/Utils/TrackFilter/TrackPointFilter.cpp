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
#include <string.h>
#include <cmath> 

#define SQ(x)    ((x)*(x))
#define FLOAT_0  0.00001

#define TPF_USE_LOG             0
#define TPF_USE_LINE_CHECKED    0

#if TPF_USE_LOG
#  include <stdio.h>
#  define LOG_PRINT(format, ...) printf(format, ##__VA_ARGS__),printf("\n")
#else
#  define LOG_PRINT(format, ...)
#endif

TrackPointFilter::TrackPointFilter()
{
    memset(&priv, 0, sizeof(priv));
}

TrackPointFilter::~TrackPointFilter()
{

}

void TrackPointFilter::Reset()
{
    priv.pointCnt = 0;
    priv.pointOutputCnt = 0;
}

bool TrackPointFilter::PushPoint(const Point_t* point)
{
    bool retval = false;
    DumpPoint("\n+P", point);
    if (priv.pointCnt == 0)
    {
        retval = true;
        OutputPoint(point);
    }
    else if (priv.pointCnt == 1)
    {
        if (!GetLine(&priv.refLine, &priv.prePoint, point))
        {
            return false;
        }
        DumpLine("First", &priv.refLine);
    }
    else
    {
        DumpLine("--", &priv.refLine);

        double offset = GetOffset(&priv.refLine, point);
        LOG_PRINT("OFS = %lf", offset);

        if (offset > priv.offsetThreshold)
        {
            LOG_PRINT("<---> offset detect!");

            retval = true;

            if (priv.secondFilterMode)
            {
                OutputPoint(&priv.tailPoint);
            }

            OutputPoint(&priv.prePoint);
            if (!GetLine(&priv.refLine, &priv.prePoint, point))
            {
                return false;
            }
        }
        else
        {
            Line_t line;
            if (!GetLine(&line, &priv.tailPoint, &priv.prePoint))
            {
                return false;
            }

            DumpLine("L", &line);
#if TPF_USE_LINE_CHECKED
            bool inLine1 = GetIsPointInLine(&line, &priv.tailPoint);
            if (!inLine1)
            {
                DumpPoint("tailPoint", &priv.tailPoint);
                LOG_PRINT("not in L");
                while (1);
            }

            bool inLine2 = GetIsPointInLine(&line, &priv.prePoint);
            if (!inLine2)
            {
                DumpPoint("prePoint", &priv.prePoint);
                LOG_PRINT("not in L");
                while (1);
            }
#endif

            Line_t verLine;
            GetVerticalLine(&verLine, &line, &priv.prePoint);

            DumpLine("|", &verLine);
            DumpPoint("in", &priv.prePoint);

#if TPF_USE_LINE_CHECKED
            bool inLine3 = GetIsPointInLine(&verLine, &priv.prePoint);
            if (!inLine3)
            {
                DumpPoint("prePoint", &priv.prePoint);
                LOG_PRINT("not in verLine");
                while (1);
            }
#endif

            if (GetIsOnSameSide(&verLine, &priv.tailPoint, point))
            {
                LOG_PRINT("~~~ direction change detect!");

                DumpPoint("p0", &priv.tailPoint);
                DumpPoint("p1", &priv.prePoint);
                DumpPoint("p2", point);

                retval = true;

                if (priv.secondFilterMode)
                {
                    OutputPoint(&priv.tailPoint);
                }

                OutputPoint(&priv.prePoint);
                if (!GetLine(&priv.refLine, &priv.prePoint, point))
                {
                    return false;
                }
            }
        }
    }

    priv.tailPoint = priv.prePoint;
    priv.prePoint = *point;
    priv.pointCnt++;

    return retval;
}

void TrackPointFilter::PushEnd()
{
    OutputPoint(&priv.prePoint);
    Reset();
}

void TrackPointFilter::SetOffsetThreshold(double offset)
{
    priv.offsetThreshold = offset;
}

void TrackPointFilter::SetOutputPointCallback(Callback_t callback)
{
    priv.outputCallback = callback;
}

void TrackPointFilter::SetSecondFilterModeEnable(bool en)
{
    priv.secondFilterMode = en;
}

void TrackPointFilter::OutputPoint(const Point_t* point)
{
    if (priv.outputCallback)
    {
        DumpPoint(">>> output", point);
        LOG_PRINT("");
        priv.outputCallback(this, point);
    }
    priv.pointOutputCnt++;
}

bool TrackPointFilter::GetLine(Line_t* line, const Point_t* point0, const Point_t* point1)
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

    if (x_diff_abs < FLOAT_0 && y_diff_abs > FLOAT_0)
    {
        a = 1;
        b = 0;
        c = -x0;
    }
    else if (x_diff_abs > FLOAT_0 && y_diff_abs < FLOAT_0)
    {
        a = 0;
        b = 1;
        c = -y0;
    }
    else if (x_diff_abs > FLOAT_0 && y_diff_abs > FLOAT_0)
    {
        a = (y1 - y0) / (x0 - x1);
        b = (a * (x0 - x1)) / (y1 - y0);
        c = 0 - a * x0 - b * y0;
    }
    else
    {
        retval = false;
    }

    line->a = a;
    line->b = b;
    line->c = c;

    return retval;
}

void TrackPointFilter::DumpLine(const char* name, const Line_t* line)
{
    LOG_PRINT(
        "%s : %lfx + %lfy + %lf = 0 { y = %lfx + %lf }",
        name, line->a, line->b, line->c,
        -line->a / line->b, -line->c / line->b
    );
}

void TrackPointFilter::DumpPoint(const char* name, const Point_t* point)
{
    LOG_PRINT("%s : (%lf, %lf)", name, point->x, point->y);
}

void TrackPointFilter::GetVerticalLine(Line_t* verLine, const Line_t* oriLine, const Point_t* point)
{
    verLine->a = -oriLine->b;
    verLine->b = oriLine->a;
    verLine->c = 0 - verLine->a * point->x - verLine->b * point->y;
}

double TrackPointFilter::GetOffset(const Line_t* line, const Point_t* point)
{
    double temp = line->a * point->x + line->b * point->y + line->c;
    double offset = std::abs(temp) * QuickSort(SQ(line->a) + SQ(line->b));
    return offset;
}

bool TrackPointFilter::GetIsOnSameSide(const Line_t* line, const Point_t* point0, const Point_t* point1)
{
    bool retval = true;
    double side = (line->a * point0->x + line->b * point0->y + line->c)
                  * (line->a * point1->x + line->b * point1->y + line->c);

    if (side < FLOAT_0)
    {
        retval = false;
    }

    return retval;
}

bool TrackPointFilter::GetIsPointInLine(const Line_t* line, const Point_t* point)
{
    double result = line->a * point->x + line->b * point->y + line->c;
    return std::abs(result) < FLOAT_0;
}

double TrackPointFilter::QuickSort(double num)
{
    long i;
    float x2, y;
    const float threehalfs = 1.5f;
    x2 = (float)num * 0.5f;
    y = (float)num;
    i = *(long*)&y;
    i = 0x5f3759df - (i >> 1);
    y = *(float*)&i;
    y = y * (threehalfs - (x2 * y * y));
    y = y * (threehalfs - (x2 * y * y));
    return y;
}
