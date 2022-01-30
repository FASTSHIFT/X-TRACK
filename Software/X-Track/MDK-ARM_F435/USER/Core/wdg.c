/*
 * MIT License
 * Copyright (c) 2019-2021 _VIFEXTech
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
    wdt_division_type division;

    static const wdt_division_type div_map[] =
    {
        WDT_CLK_DIV_4,
        WDT_CLK_DIV_8,
        WDT_CLK_DIV_16,
        WDT_CLK_DIV_32,
        WDT_CLK_DIV_64,
        WDT_CLK_DIV_128,
        WDT_CLK_DIV_256
    };

    /* set reload value
     * lick_freq = 40000
     * timeout = reload_value * divider / lick_freq * 1000 (ms)
     * timeout / divider * lick_freq / 1000 = reload_value
     */
    const uint32_t lick_freq = 40000;

    for(int i = 0; i < sizeof(div_map) / sizeof(wdt_division_type); i++)
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

    if(crm_flag_get(CRM_WDT_RESET_FLAG) != RESET)
    {
        /* reset from wdt */
        crm_flag_clear(CRM_WDT_RESET_FLAG);
    }

    /* disable register write protection */
    wdt_register_write_enable(TRUE);

    /* set the wdt divider value */
    wdt_divider_set(division);

    /* set reload value */
    wdt_reload_value_set(reload_value - 1);

    /* enable wdt */
    wdt_enable();

    return real_timeout;
}

void WDG_ReloadCounter(void)
{
    wdt_counter_reload();
}
