#ifndef __DIALPLATE_MODEL_H
#define __DIALPLATE_MODEL_H

#include "Common/DataProc/DataProc.h"

class DialplateModel
{
public:
    typedef enum {
        REC_STOP = DataProc::RECORDER_STOP,
        REC_START = DataProc::RECORDER_START,
        REC_PAUSE = DataProc::RECORDER_PAUSE,       
    }RecCtrl_t;

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

    void RecorderCtrl(RecCtrl_t ctrl);

private:
    Account* account;

private:
    static int onEvent(Account::EventParam_t*param);
};

#endif
