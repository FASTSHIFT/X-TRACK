#include "TonePlayer.h"

#ifndef NULL
#  define NULL 0
#endif

TonePlayer::TonePlayer()
{
    CallbackFunction = NULL;
    CurrentMusic = NULL;
    Speed = SPEED_NORMAL;
}

void TonePlayer::SetMusic(const MusicNode_t* music, uint16_t length)
{
    if(music == CurrentMusic)
        return;
    
    CurrentMusic = music;
    Length = length;
    CurrentPos = length;
}

void TonePlayer::SetSpeed(uint16_t speed)
{
    Speed = speed;
}

void TonePlayer::Play()
{
    CurrentPos = 0;
}

void TonePlayer::Stop()
{
    CurrentPos = Length;
}

bool TonePlayer::Update(uint32_t tick)
{
    if(CallbackFunction == NULL)
        return false;
    
    if(CurrentPos < Length)
    {
        if(tick > NextTime)
        {
            CallbackFunction(CurrentMusic[CurrentPos].Freq, CurrentMusic[CurrentPos].Volume);

            NextTime = tick + CurrentMusic[CurrentPos].Time * Speed / SPEED_NORMAL;
            CurrentPos++;
        }
        return true;
    }
    else if(CurrentPos == Length && tick > NextTime)
    {
        CallbackFunction(0, 0);
        CurrentPos++;
    }
    return false;
}
