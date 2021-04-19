#ifndef __LIVEMAP_MODEL_H
#define __LIVEMAP_MODEL_H

#include "lvgl/lvgl.h"
#include "../../Utils/MapConv/MapConv.h"
#include "Common/DataProc/DataProc.h"

class LiveMapModel
{
public:
    MapConv mapConv;
    HAL::SportStatus_Info_t sportStatusInfo;

public:
    void Init();
    void Deinit();
    void GetGPS_Info(HAL::GPS_Info_t* info)
    {
        account->Pull("GPS", 0, info, sizeof(HAL::GPS_Info_t));
    }

private:
    Account* account;

private:
    static int Callback(
        Account* pub,
        Account* sub,
        int msgType,
        void* data_p,
        uint32_t size
    );
};

#endif
