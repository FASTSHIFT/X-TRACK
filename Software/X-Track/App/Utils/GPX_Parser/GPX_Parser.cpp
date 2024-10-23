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

int GPX_Parser::ReadNext(Point_t* point)
{
    String str;
    int flag = PARSER_FLAG_NONE;

    if (!find((char*)"<trkpt"))
    {
        flag |= PARSER_FLAG_UNMATCHED;
        flag |= PARSER_FLAG_EOF;
        goto failed;
    }

    while (true)
    {
        bool ret = readStringUntil('>', &str);
        if (!ret)
        {
            flag |= PARSER_FLAG_UNMATCHED;
            goto failed;
        }

        int index = str.indexOf("lat=");
        if (index >= 0)
        {
            String lat = str.substring(str.indexOf('"') + 1);
            point->latitude = lat.toFloat();
            flag |= PARSER_FLAG_LAT;

            String lon = str.substring(str.indexOf("lon="));
            lon = lon.substring(lon.indexOf('"') + 1);
            point->longitude = lon.toFloat();
            flag |= PARSER_FLAG_LNG;
            continue;
        }

        index = str.indexOf("<ele");
        if (index >= 0)
        {
            if (!readStringUntil('>', &str))
            {
                flag |= PARSER_FLAG_UNMATCHED;
                goto failed;
            }
            String ele = str;
            point->altitude = ele.toFloat();
            flag |= PARSER_FLAG_ALT;
            continue;
        }

        index = str.indexOf("<time");
        if (index >= 0)
        {
            if (!readStringUntil('>', &str))
            {
                flag |= PARSER_FLAG_UNMATCHED;
                goto failed;
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
            flag |= PARSER_FLAG_TIME;
            continue;
        }

        if (str.length() == 0)
        {     
            goto failed;
        }

        if (str.indexOf("</trkpt") >= 0)
        {
            break;
        }
    }

failed:
    return flag;
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
