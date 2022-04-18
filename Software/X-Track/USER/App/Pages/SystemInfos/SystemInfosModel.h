#ifndef __SYSTEM_INFOS_MODEL_H
#define __SYSTEM_INFOS_MODEL_H

#include "Common/DataProc/DataProc.h"

namespace Page
{

class SystemInfosModel
{
public:
    void Init();
    void Deinit();

    void GetSportInfo(
        float* trip,
        char* time, uint32_t len,
        float* maxSpd
    );

    void GetGPSInfo(
        float* lat,
        float* lng,
        float* alt,
        char* utc, uint32_t len,
        float* course,
        float* speed,
        float* temp,
        float* pressure
    );

    void GetMAGInfo(
        float* dir,
        int* x,
        int* y,
        int* z
    );

    void GetIMUInfo(
        int* step,
        char* info, uint32_t len
    );

    void GetRTCInfo(
        char* dateTime, uint32_t len
    );

    void GetBatteryInfo(
        int* usage,
        float* voltage,
        char* state, uint32_t len,
        int16_t* current,
        uint16_t* remaining_capacity,
        uint16_t* fullcharge_capacity,
        int16_t* average_power,
        uint16_t* design_capacity,
        uint16_t* time_to
    );

    void GetStorageInfo(
        bool* detect,
        const char** type,
        char* size, uint32_t len
    );

    void SetStatusBarStyle(DataProc::StatusBar_Style_t style);

private:
    Account* account;

private:

};

}

#endif
