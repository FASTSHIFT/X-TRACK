#include "GPX_Parser.h"
#include <string.h>

GPX_Parser::GPX_Parser(Callback_t avaliableCallback, Callback_t readCallback, void* userData)
    : _avaliableCallback(avaliableCallback)
    , _readCallback(readCallback)
    , _userData(userData)
{
}

bool GPX_Parser::readStringUntil(char terminator, String* str)
{
    bool retval = false;
    char strBuf[64];

    size_t len = readBytesUntil(terminator, strBuf, sizeof(strBuf));

    if (len < sizeof(strBuf)) {
        strBuf[len] = '\0';
        *str = strBuf;
        retval = true;
    }

    return retval;
}

int GPX_Parser::getNext(Point_t* point)
{
    String str;
    int flag = FLAG_NONE;

    if (!find((char*)"<trkpt")) {
        flag |= FLAG_UNMATCHED;
        flag |= FLAG_END_OF_FILE;
        goto failed;
    }

    while (true) {
        bool ret = readStringUntil('>', &str);
        if (!ret) {
            flag |= FLAG_UNMATCHED;
            goto failed;
        }

        int index = str.indexOf("lat=");
        if (index >= 0) {
            String lat = str.substring(str.indexOf('"') + 1);
            point->latitude = lat.toFloat();
            flag |= FLAG_LATITUDE;

            String lon = str.substring(str.indexOf("lon="));
            lon = lon.substring(lon.indexOf('"') + 1);
            point->longitude = lon.toFloat();
            flag |= FLAG_LONGITUDE;
            continue;
        }

        index = str.indexOf("<ele");
        if (index >= 0) {
            if (!readStringUntil('>', &str)) {
                flag |= FLAG_UNMATCHED;
                goto failed;
            }
            String ele = str;
            point->altitude = ele.toFloat();
            flag |= FLAG_ALTITUDE;
            continue;
        }

        index = str.indexOf("<time");
        if (index >= 0) {
            if (!readStringUntil('>', &str)) {
                flag |= FLAG_UNMATCHED;
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
                &second);
            point->time.year = year;
            point->time.month = month;
            point->time.day = day;
            point->time.hour = hour;
            point->time.minute = minute;
            point->time.second = second;
            flag |= FLAG_TIME;
            continue;
        }

        if (str.length() == 0) {
            goto failed;
        }

        if (str.indexOf("</trkpt") >= 0) {
            break;
        }
    }

failed:
    return flag;
}

int GPX_Parser::available()
{
    return _avaliableCallback(this);
}

int GPX_Parser::read()
{
    return _readCallback(this);
}
