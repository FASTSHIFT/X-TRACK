#ifndef __GPX_PARSER_H
#define __GPX_PARSER_H

#ifdef ARDUINO
#include "Stream.h"
#else
#include "../Stream/Stream.h"
#endif /* ARDUINO */

class GPX_Parser : public Stream {
public:
    typedef int (*Callback_t)(GPX_Parser* parser);

    typedef struct {
        uint16_t year;
        uint8_t month;
        uint8_t day;
        uint8_t hour;
        uint8_t minute;
        uint8_t second;
    } Time_t;

    typedef struct
    {
        float longitude;
        float latitude;
        float altitude;
        Time_t time;
    } Point_t;

    typedef enum {
        FLAG_NONE = 0,

        /* Error */
        FLAG_END_OF_FILE = 1 << 0,
        FLAG_OVER_FLOW = 1 << 1,
        FLAG_UNMATCHED = 1 << 2,
        FLAG_RESERVE = 1 << 3,

        /* Data ready */
        FLAG_LONGITUDE = 1 << 4,
        FLAG_LATITUDE = 1 << 5,
        FLAG_ALTITUDE = 1 << 6,
        FLAG_TIME = 1 << 7
    } Flag_t;

public:
    GPX_Parser(Callback_t avaliableCallback, Callback_t readCallback, void* userData);
    int getNext(Point_t* point);
    void* getUserData() { return _userData; }

private:
    virtual int available();
    virtual int read();
    virtual int peek()
    {
        return 0;
    }
    virtual void flush() { }
    virtual size_t write(uint8_t ch)
    {
        return 0;
    }
    using Print::write;

    bool readStringUntil(char terminator, String* str);

private:
    Callback_t _avaliableCallback;
    Callback_t _readCallback;
    void* _userData;
};

#endif
