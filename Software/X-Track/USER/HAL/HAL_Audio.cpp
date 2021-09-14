#include "HAL.h"
#include "App/Utils/TonePlayer/TonePlayer.h"

static TonePlayer player;

#include "App/Common/Music/MusicCode.h"

void HAL::Audio_Init()
{
    player.SetCallback([](uint32_t freq, uint16_t volume)
    {
        HAL::Buzz_Tone(freq);
    });
}

void HAL::Audio_Update()
{
    player.Update(millis());
}

bool HAL::Audio_PlayMusic(const char* name)
{
    bool retval = false;
    for (int i = 0; i < sizeof(MusicList) / sizeof(MusicList[0]); i++)
    {
        if (strcmp(name, MusicList[i].name) == 0)
        {
            player.Play(MusicList[i].mc, MusicList[i].length);
            retval = true;
            break;
        }
    }
    return retval;
}
