#ifndef __START_UP_MODEL_H
#define __START_UP_MODEL_H

#include "Common/DataProc/DataProc.h"

class StartUpModel
{
public:
    void Init();
    void Deinit();
    void PlayMusic(const char* music);

private:
    Account* account;
};

#endif
