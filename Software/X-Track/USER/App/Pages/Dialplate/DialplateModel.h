#ifndef __DIALPLATE_MODEL_H
#define __DIALPLATE_MODEL_H

#include "Common/DataProc/DataProc.h"

class DialplateModel
{
public:
    HAL::SportStatus_Info_t sportStatusInfo;

public:
    void Init();
    void Deinit();
    inline float GetSpeed()
    {
        return sportStatusInfo.speedKph;
    }
    
    inline float GetAvgSpeed()
    {
        return sportStatusInfo.speedAvgKph;
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
