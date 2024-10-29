#include "DataProc.h"
#include "../HAL/HAL.h"

static int onEvent(Account* account, Account::EventParam_t* param)
{
    if (param->event != Account::EVENT_NOTIFY)
    {
        return Account::RES_UNSUPPORTED_REQUEST;
    }

    if (param->size != sizeof(DataProc::MusicPlayer_Info_t))
    {
        return Account::RES_SIZE_MISMATCH;
    }

    DataProc::MusicPlayer_Info_t* info = (DataProc::MusicPlayer_Info_t*)param->data_p;

    HAL::Audio_PlayMusic(info->music);

    return Account::RES_OK;
}

DATA_PROC_INIT_DEF(MusicPlayer)
{
    account->SetEventCallback(onEvent);
}
