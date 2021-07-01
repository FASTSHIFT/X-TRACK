#include "StartUpModel.h"

using namespace Page;

void StartUpModel::Init()
{
    account = new Account("StartUpModel", DataProc::Center(), 0, this);
    account->Subscribe("MusicPlayer");
}

void StartUpModel::Deinit()
{
    if (account)
    {
        delete account;
        account = nullptr;
    }
}

void StartUpModel::PlayMusic(const char* music)
{
    DataProc::MusicPlayer_Info_t info;
    info.music = music;
    account->Notify("MusicPlayer", &info, sizeof(info));
}
