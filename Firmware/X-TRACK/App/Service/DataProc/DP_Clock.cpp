#include "DataProc.h"
#include "../HAL/HAL.h"
#include "lvgl/lvgl.h"

uint32_t DataProc::GetTick()
{
    return lv_tick_get();
}

uint32_t DataProc::GetTickElaps(uint32_t prevTick)
{
    return lv_tick_elaps(prevTick);
}

const char* DataProc::MakeTimeString(uint64_t ms, char* buf, uint16_t len)
{
    uint64_t ss = ms / 1000;
    uint64_t mm = ss / 60;
    uint32_t hh = (uint32_t)(mm / 60);

    lv_snprintf(
        buf, len,
        "%d:%02d:%02d",
        hh,
        (uint32_t)(mm % 60),
        (uint32_t)(ss % 60)
    );

    return buf;
}

static bool Clock_Calibrate(Account* account, HAL::GPS_Info_t* gpsInfo)
{
    bool retval = false;
    if(gpsInfo->isVaild)
    {
        HAL::Clock_Info_t clock;
        if (account->Pull("TzConv", &clock, sizeof(clock)) == Account::RES_OK)
        {
            HAL::Clock_SetInfo(&clock);
            retval = true;
        }
    }
    return retval;
}

static int onEvent(Account* account, Account::EventParam_t* param)
{
    if (param->event == Account::EVENT_PUB_PUBLISH)
    {
        if (param->size == sizeof(HAL::GPS_Info_t))
        {
            if (Clock_Calibrate(account, (HAL::GPS_Info_t*)param->data_p))
            {
                account->Unsubscribe("GPS");
            }
        }
        
        return Account::RES_OK;
    }

    if (param->event != Account::EVENT_SUB_PULL)
    {
        return Account::RES_UNSUPPORTED_REQUEST;
    }

    if (param->size != sizeof(HAL::Clock_Info_t))
    {
        return Account::RES_SIZE_MISMATCH;
    }

    HAL::Clock_Info_t* info = (HAL::Clock_Info_t*)param->data_p;
    HAL::Clock_GetInfo(info);

    return Account::RES_OK;
}

DATA_PROC_INIT_DEF(Clock)
{
    account->Subscribe("TzConv");
    account->Subscribe("GPS");
    account->SetEventCallback(onEvent);
}
