#ifndef __START_UP_MODEL_H
#define __START_UP_MODEL_H

#include "Common/DataProc/DataProc.h"
#include "Common/HAL/HAL.h"

namespace Page
{

class StartUpModel
{
public:
    void Init();
    void Deinit();
    void PlayMusic(const char* music);
    void SetEncoderEnable(bool en)
    {
        HAL::Encoder_SetEnable(en);
    }

private:
    Account* account;
};

}

#endif
