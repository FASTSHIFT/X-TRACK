/*
 * MIT License
 * Copyright (c) 2017 - 2022 _VIFEXTech
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "wdg.h"

uint32_t WDG_Init(uint32_t timeout)
{
    uint32_t reload_value;
    uint32_t real_timeout = 0;

    uint16_t division;

    static const uint8_t div_map[] =
    {
        IWDG_Psc_4,
        IWDG_Psc_8,
        IWDG_Psc_16,
        IWDG_Psc_32,
        IWDG_Psc_64,
        IWDG_Psc_128,
        IWDG_Psc_256
    };

    /* set reload value
     * lick_freq = 40000
     * timeout = reload_value * divider / lick_freq * 1000 (ms)
     * timeout / divider * lick_freq / 1000 = reload_value
     */
    const uint32_t lick_freq = 40000;

    for(int i = 0; i < sizeof(div_map) / sizeof(uint8_t); i++)
    {
        int div = 4 << i;
        reload_value = (uint64_t)timeout * lick_freq / div / 1000;

        if(reload_value <= 0xFFF)
        {
            real_timeout = (uint64_t)reload_value * div * 1000 / lick_freq;
            division = div_map[i];
            break;
        }
    }

    if(reload_value > 0xFFF)
    {
        return 0;
    }

    /* Check if the system has resumed from IWDG reset */
    if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
    {
        /* Clear reset flags */
        RCC_ClearFlag();
    }

    /* Enable write access to IWDG_PSC and IWDG_RLD registers */
    IWDG_KeyRegWrite(IWDG_KeyRegWrite_Enable);

    /* IWDG counter clock: LSI/division */
    IWDG_SetPrescaler(division);

    /* Set counter reload value */
    IWDG_SetReload(reload_value - 1);

    /* Reload IWDG counter */
    IWDG_ReloadCounter();

    /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
    IWDG_Enable();

    return real_timeout;
}

void WDG_ReloadCounter(void)
{
    IWDG_ReloadCounter();
}
