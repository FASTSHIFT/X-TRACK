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

const char* DataProc::ConvTime(uint64_t ms, char* buf, uint16_t len)
{
    uint32_t ss = ms / 1000;
    uint32_t mm = ss / 60;
    uint32_t hh = mm / 60;
    lv_snprintf(buf, len, "%d:%02d:%02d", hh, mm % 60, ss % 60);
    return buf;
}

static int Clock_Callback(
    Account* pub,
    Account* sub,
    int msgType,
    void* data_p,
    uint32_t size
)
{
    if (size != sizeof(HAL::Clock_Info_t))
    {
        return -1;
    }

    HAL::Clock_GetInfo((HAL::Clock_Info_t*)data_p);

    return 0;
}

Account* DataProc::Clock_Init(DataCenter* center)
{
    Account* account = new Account("Clock", DataProc::Center());
    account->SetPullCallback(Clock_Callback);
    return account;
}
