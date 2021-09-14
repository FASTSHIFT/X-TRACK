#ifndef __GPX_PARSER_H
#define __GPX_PARSER_H

#ifdef  ARDUINO
#include "Stream.h"
#else
#include "../Stream/Stream.h"
#endif //  ARDUINO

class GPX_Parser : public Stream
{
public:
    typedef int (*Callback_t)(GPX_Parser* parser);

    typedef struct {
        uint16_t year;
        uint8_t month;
        uint8_t day;
        uint8_t hour;
        uint8_t minute;
        uint8_t second;
    }Time_t;

    typedef struct
    {
        float longitude;
        float latitude;
        float altitude;
        Time_t time;
    } Point_t;

    typedef enum {
        PARSER_FLAG_NONE = 0,

        /* Error */
        PARSER_FLAG_EOF       = 1 << 0,
        PARSER_FLAG_OVF       = 1 << 1,
        PARSER_FLAG_UNMATCHED = 1 << 2,
        PARSER_FLAG_REV       = 1 << 3,

        /* Data ready */
        PARSER_FLAG_LNG = 1 << 4,
        PARSER_FLAG_LAT = 1 << 5,
        PARSER_FLAG_ALT = 1 << 6,
        PARSER_FLAG_TIME = 1 << 7
    } ParserFlag_t;

public:
    GPX_Parser();
    ~GPX_Parser();

    void SetCallback(Callback_t avaliableCallback, Callback_t readCallback);
    int ReadNext(Point_t* point);

public:
    void* userData;

private:
    virtual int available();
    virtual int read();
    virtual int peek()
    {
        return 0;
    }
    virtual void flush() {}
    virtual size_t write(uint8_t ch)
    {
        return 0;
    }
    using Print::write;

    bool readStringUntil(char terminator, String* str);

private:
    struct
    {
        Callback_t avaliableCallback;
        Callback_t readCallback;
    } priv;
};

#endif
