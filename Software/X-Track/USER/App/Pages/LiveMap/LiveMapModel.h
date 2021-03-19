#ifndef __LIVEMAP_MODEL_H
#define __LIVEMAP_MODEL_H

#include "lvgl/lvgl.h"
#include "../../Utils/MapConv/MapConv.h"
#include "../../Common/HAL/HAL.h"

class LiveMapModel
{
public:
    MapConv mapConv;
 
    bool GetGPSInfo(HAL::GPS_Info_t* info)
    {
        HAL::GPS_Update();
        return HAL::GPS_GetInfo(info);
    }
private:
    
};

#endif
