#include "rng.h"

#if defined(RCC_AHB2Periph_RNG)
static void RNG_Init(void)
{
    RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);
    RNG_Cmd(ENABLE);
}
#endif

uint32_t RNG_GetRandNum(void)
{
#if defined(RCC_AHB2Periph_RNG)
    static uint8_t isInit = 0;
    uint32_t rand;
    
    if(!isInit)
    {
        RNG_Init();
        isInit = 1;
    }
    
    while(!RNG_GetFlagStatus(RNG_FLAG_DRDY));
    rand = RNG_GetRandomNumber();
    return rand;
#else
#warning "RNG is not supported for this MCU!"
    return 0;
#endif
}
