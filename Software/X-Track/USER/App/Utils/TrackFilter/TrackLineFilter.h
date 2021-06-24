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
        EVENT_WRITE_POINT,
        EVENT_END_LINE,
        EVENT_RESET
    } EventCode_t;

    typedef struct
    {
        EventCode_t code;
        uint32_t lineIndex;
        const Point_t* point;
    } Event_t;

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

    void SetClipArea(int32_t x0, int32_t x1, int32_t y0, int32_t y1);
    void SetOutputPointCallback(Callback_t callback);

private:
    typedef struct
    {
        int32_t x0;
        int32_t y0;
        int32_t x1;
        int32_t y1;
    } Area_t;

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
