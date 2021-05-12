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
        account->Pull("GPS", info, sizeof(HAL::GPS_Info_t));
    }

private:
    Account* account;

private:
    static int onEvent(Account::EventParam_t* param);
};

#endif
