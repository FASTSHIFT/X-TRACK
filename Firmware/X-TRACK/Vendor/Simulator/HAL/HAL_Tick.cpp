/*
 * MIT License
 * Copyright (c) 2023 - 2024 _VIFEXTech
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
#include "HAL.h"
#include <time.h>

#ifdef WIN32
#include <stdint.h>
#include <windows.h>

#define CLOCK_MONOTONIC 0
#define EPOCHFILETIME (116444736000000000ULL)

static int clock_gettime(int, struct timespec* tv)
{
    if (!tv) {
        return -1;
    }

    FILETIME ft;
    uint64_t tmpres = 0;
    GetSystemTimeAsFileTime(&ft);
    tmpres |= ft.dwHighDateTime;
    tmpres <<= 32;
    tmpres |= ft.dwLowDateTime;

    tmpres /= 10; // Convert 100 nanoseconds intervals to microseconds
    tmpres -= EPOCHFILETIME; // Subtract time since epoch

    tv->tv_sec = static_cast<long>(tmpres / 1000000UL);
    tv->tv_nsec = static_cast<long>(tmpres % 1000000UL) * 1000;

    return 0;
}
#endif

uint32_t HAL::GetTick()
{
    struct timespec ts;
    uint32_t ms;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    ms = ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
    return ms;
}

uint32_t HAL::GetTickElaps(uint32_t prevTick)
{
    uint32_t actTick = GetTick();

    /*If there is no overflow in sys_time simple subtract*/
    if (actTick >= prevTick) {
        prevTick = actTick - prevTick;
    } else {
        prevTick = UINT32_MAX - prevTick + 1;
        prevTick += actTick;
    }

    return prevTick;
}
