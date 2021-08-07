#include "StartupModel.h"

using namespace Page;

void StartupModel::Init()
{
    account = new Account("StartupModel", DataProc::Center(), 0, this);
    account->Subscribe("MusicPlayer");
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
    info.music = music;
    account->Notify("MusicPlayer", &info, sizeof(info));
}
