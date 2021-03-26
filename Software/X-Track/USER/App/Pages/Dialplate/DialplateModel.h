#ifndef __DIALPLATE_MODEL_H
#define __DIALPLATE_MODEL_H

#include "lvgl/lvgl.h"
#include "../../Common/HAL/HAL.h"

class DialplateModel
{
public:
    float GetSpeed()
    {
        HAL::GPS_Info_t gpsInfo;
        HAL::GPS_GetInfo(&gpsInfo);
        return gpsInfo.speed;
    }
private:

};

#endif
