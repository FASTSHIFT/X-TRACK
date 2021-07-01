#include "DialplateModel.h"

using namespace Page;

void DialplateModel::Init()
{
    account = new Account("DialplateModel", DataProc::Center(), 0, this);
    account->Subscribe("SportStatus");
    account->Subscribe("Recorder");
    account->Subscribe("StatusBar");
    account->Subscribe("GPS");
    account->Subscribe("MusicPlayer");
    account->SetEventCallback(onEvent);
}

void DialplateModel::Deinit()
{
    if (account)
    {
        delete account;
        account = nullptr;
    }
}

bool DialplateModel::GetGPSReady()
{
    HAL::GPS_Info_t gps;
    account->Pull("GPS", &gps, sizeof(gps));
    return (gps.satellites > 0);
}

int DialplateModel::onEvent(Account* account, Account::EventParam_t* param)
{
    if (param->event != Account::EVENT_PUB_PUBLISH)
    {
        return Account::ERROR_UNSUPPORTED_REQUEST;
    }

    if (strcmp(param->tran->ID, "SportStatus") != 0
            || param->size != sizeof(HAL::SportStatus_Info_t))
    {
        return -1;
    }

    DialplateModel* instance = (DialplateModel*)account->UserData;
    memcpy(&(instance->sportStatusInfo), param->data_p, param->size);

    return 0;
}

void DialplateModel::RecorderCommand(RecCmd_t cmd)
{
    DataProc::Recorder_Info_t recInfo;
    recInfo.cmd = (DataProc::Recorder_Cmd_t)cmd;
    recInfo.time = 1000;
    account->Notify("Recorder", &recInfo, sizeof(recInfo));

    DataProc::StatusBar_Info_t statInfo;

    switch (cmd)
    {
    case REC_START:
    case REC_CONTINUE:
        statInfo.showLabelRec = true;
        statInfo.labelRecStr = "REC";
        break;
    case REC_PAUSE:
        statInfo.showLabelRec = true;
        statInfo.labelRecStr = "PAUSE";
        break;
    case REC_STOP:
        statInfo.showLabelRec = false;
        break;
    default:
        break;
    }

    account->Notify("StatusBar", &statInfo, sizeof(statInfo));
}

void DialplateModel::PlayMusic(const char* music)
{
    DataProc::MusicPlayer_Info_t info;
    info.music = music;
    account->Notify("MusicPlayer", &info, sizeof(info));
}
