/*
 * MIT License
 * Copyright (c) 2024 _VIFEXTech
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
#ifndef __HAL_CLOCK_DEF_H
#define __HAL_CLOCK_DEF_H

#include <stdbool.h>
#include <stdint.h>

namespace HAL {

/* clang-format off */

/* DEVICEO_OBJECT_IOCMD_DEF(dir, size, type, number) */

/*********************
 *      Clock
 *********************/

typedef struct
{
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t week;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint16_t millisecond;
} Clock_Info_t;

#define CLOCK_IOCMD_CALIBRATE   DEVICE_OBJECT_IOCMD_DEF(DeviceObject::DIR_IN, sizeof(HAL::Clock_Info_t), 0, 0)

/* clang-format on */

} // namespace HAL

#endif // __HAL_CLOCK_DEF_H
