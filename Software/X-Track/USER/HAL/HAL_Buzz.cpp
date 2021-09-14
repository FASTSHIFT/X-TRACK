#include "HAL.h"

static bool IsEnable = true;

void HAL::Buzz_init()
{
    pinMode(CONFIG_BUZZ_PIN, OUTPUT);
}

void HAL::Buzz_SetEnable(bool en)
{
    if(!en)
    {
        Buzz_Tone(0);
    }

    IsEnable = en;
}

void HAL::Buzz_Tone(uint32_t freq, int32_t duration)
{
    if(!IsEnable)
    {
        return;
    }

    if(duration >= 0)
    {
        tone(CONFIG_BUZZ_PIN, freq, duration);
    }
    else
    {
        tone(CONFIG_BUZZ_PIN, freq);
    }
}
