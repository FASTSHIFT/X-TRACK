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
#ifndef __HAL_GNSS_DEF_H
#define __HAL_GNSS_DEF_H

#include <stdbool.h>
#include <stdint.h>

namespace HAL {

/* clang-format off */

/* DEVICEO_OBJECT_IOCMD_DEF(dir, size, type, number) */

/*********************
 *      GNSS
 *********************/

typedef struct
{
    uint8_t id;
    int16_t elevation;
    int16_t azimuth;
    int16_t snr;
} GNSS_GSV_Item_t;

typedef struct
{
    bool isVaild;
    int16_t satellites;
    Clock_Info_t clock;

    float longitude;
    float latitude;
    float altitude;
    float course;
    float speed;

    float pdop;
    float hdop;
    float vdop;

    struct 
    {
        const GNSS_GSV_Item_t* items;
        uint8_t num;
        int16_t satsInView;
    } gsv;
} GNSS_Info_t;

#define GNSS_IOCMD_UPDATE       DEVICE_OBJECT_IOCMD_DEF(DeviceObject::DIR_IN, 0, 0, 0)

/* clang-format on */

} // namespace HAL

#endif // __HAL_GNSS_DEF_H
