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
