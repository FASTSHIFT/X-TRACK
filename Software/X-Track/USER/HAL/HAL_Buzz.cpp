#include "HAL.h"

static bool IsEnable = true;

void HAL::Buzz_init()
{
    pinMode(BUZZ_PIN, OUTPUT);
}

void HAL::Buzz_SetEnable(bool en)
{
    IsEnable = en;
}

void HAL::Buzz_Tone(uint32_t freq, int32_t duration)
{
    if(!IsEnable)
        return;
    
    if(duration >= 0)
    {
        tone(BUZZ_PIN, freq, duration);
    }
    else
    {
        tone(BUZZ_PIN, freq);
    }
}
