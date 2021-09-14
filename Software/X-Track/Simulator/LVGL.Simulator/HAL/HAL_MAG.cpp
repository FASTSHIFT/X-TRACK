#include "HAL.h"
#include <stdlib.h>
#include "Common/DataProc/DataProc.h"

static HAL::CommitFunc_t CommitFunc;
static void* UserData;

void HAL::MAG_SetCommitCallback(CommitFunc_t func, void* userData)
{
    CommitFunc = func;
    UserData = userData;
}

void HAL::MAG_Update()
{
    MAG_Info_t mag;
    mag.x = rand() % 1000 - 500;
    mag.y = rand() % 1000 - 500;
    mag.z = rand() % 1000 - 500;

    if (CommitFunc)
    {
        CommitFunc(&mag, UserData);
    }
}
