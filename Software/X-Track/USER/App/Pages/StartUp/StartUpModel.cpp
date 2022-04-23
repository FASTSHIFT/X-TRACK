#include "StartUpModel.h"

using namespace Page;

void StartupModel::Init()
{
    account = new Account("StartupModel", DataProc::Center(), 0, this);
    account->Subscribe("MusicPlayer");
    account->Subscribe("StatusBar");
}

void StartupModel::Deinit()
{
    if (account)
    {
        delete account;
        account = nullptr;
    }
}

void StartupModel::PlayMusic(const char* music)
{
    DataProc::MusicPlayer_Info_t info;
    DATA_PROC_INIT_STRUCT(info);
    info.music = music;
    account->Notify("MusicPlayer", &info, sizeof(info));
}

void StartupModel::SetStatusBarAppear(bool en)
{
    DataProc::StatusBar_Info_t info;
    DATA_PROC_INIT_STRUCT(info);
    info.cmd = DataProc::STATUS_BAR_CMD_APPEAR;
    info.param.appear = en;
    account->Notify("StatusBar", &info, sizeof(info));
}
