/*
 * MIT License
 * Copyright (c) 2021 - 2024 _VIFEXTech
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
#ifndef __DATA_PROC_SPORT_STATUS_DEF_H
#define __DATA_PROC_SPORT_STATUS_DEF_H

#include <cstdint>

namespace DataProc {

enum class SPORT_STATUS_CMD {
    NONE,
    RESET_SINGLE,
    RESET_TOTAL,
    RESET_SPEED_MAX,
    SET_WEIGHT,
};

typedef struct SportStatus_Info {
    SportStatus_Info()
        : cmd(SPORT_STATUS_CMD::NONE)
        , lastTick(0)
        , weight(0)
        , speedKph(0)
        , speedMaxKph(0)
        , speedAvgKph(0)
        , totalTime(0)
        , totalDistance(0)
        , singleTime(0)
        , singleDistance(0)
        , singleCalorie(0)
        , longitude(0)
        , latitude(0)
        , altitude(0)
    {
    }

    SPORT_STATUS_CMD cmd;

    uint32_t lastTick;

    float weight;

    float speedKph;
    float speedMaxKph;
    float speedAvgKph;

    uint64_t totalTime;
    float totalDistance;
    uint64_t singleTime;
    float singleDistance;
    float singleCalorie;

    float longitude;
    float latitude;
    float altitude;
} SportStatus_Info_t;

} // namespace DataProc

#endif // __DATA_PROC_SPORT_STATUS_DEF_H
