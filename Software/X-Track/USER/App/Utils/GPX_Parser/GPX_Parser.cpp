#include "GPX_Parser.h"
#include <string.h>

#define STR_LEN_MAX 64

GPX_Parser::GPX_Parser()
{
    memset(&priv, 0, sizeof(priv));
}

GPX_Parser::~GPX_Parser()
{

}

void GPX_Parser::SetCallback(Callback_t avaliableCallback, Callback_t readCallback)
{
    priv.avaliableCallback = avaliableCallback;
    priv.readCallback = readCallback;
}

bool GPX_Parser::readStringUntil(char terminator, String* str)
{
    bool retval = false;
    char strBuf[STR_LEN_MAX];

    size_t len = readBytesUntil(terminator, strBuf, sizeof(strBuf));

    if (len < sizeof(strBuf))
    {
        strBuf[len] = '\0';
        *str = strBuf;
        retval = true;
    }

    return retval;
}

bool GPX_Parser::ReadNext(Point_t* point)
{
    if (!find((char*)"<trkpt"))
    {
        return false;
    }

    String str;


    while (true)
    {
        if (!readStringUntil('>', &str))
        {
            return false;
        }

        int index = str.indexOf("lat=");
        if (index >= 0)
        {
            String lat = str.substring(str.indexOf('"') + 1);
            point->latitude = lat.toFloat();

            String lon = str.substring(str.indexOf("lon="));
            lon = lon.substring(lon.indexOf('"') + 1);
            point->longitude = lon.toFloat();
            continue;
        }

        index = str.indexOf("<ele");
        if (index >= 0)
        {
            if (!readStringUntil('>', &str))
            {
                return false;
            }
            String ele = str;
            point->altitude = ele.toFloat();
            continue;
        }

        index = str.indexOf("<time");
        if (index >= 0)
        {
            if (!readStringUntil('>', &str))
            {
                return false;
            }
            String time = str;
            int year, month, day, hour, minute, second;
            sscanf(
                time.c_str(),
                "%d-%d-%dT%d:%d:%dZ",
                &year,
                &month,
                &day,
                &hour,
                &minute,
                &second
            );
            point->time.year = year;
            point->time.month = month;
            point->time.day = day;
            point->time.hour = hour;
            point->time.minute = minute;
            point->time.second = second;
            continue;
        }

        if (str.length() == 0)
        {
            return false;
        }

        if (str.indexOf("</trkpt") >= 0)
        {
            break;
        }
    }

    return true;
}

int GPX_Parser::available()
{
    if (priv.avaliableCallback)
    {
        return priv.avaliableCallback(this);
    }

    return 0;
}

int GPX_Parser::read()
{
    if (priv.readCallback)
    {
        return priv.readCallback(this);
    }

    return 0;
}
