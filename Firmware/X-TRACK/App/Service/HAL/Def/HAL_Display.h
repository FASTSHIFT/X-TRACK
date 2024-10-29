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
#ifndef __HAL_DISPLAY_DEF_H
#define __HAL_DISPLAY_DEF_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

namespace HAL {

/* clang-format off */

/* DEVICEO_OBJECT_IOCMD_DEF(dir, size, type, number) */

/*********************
 *      Display
 *********************/

typedef struct
{
    int16_t width;
    int16_t height;
    void* buffer0;
    void* buffer1;
    size_t size;
    uint8_t bpp;
} Display_Info_t;

typedef struct
{
    int16_t x0;
    int16_t y0;
    int16_t x1;
    int16_t y1;
    void* buffer;
    size_t size;
} Display_PutArea_Info_t;

typedef void (*Display_CallbackFunc_t)(void*);

typedef struct
{
    Display_CallbackFunc_t callback;
    void* userData;
} Display_SetCallback_Info_t;

typedef struct
{
    const char* title;
    const char* info;
    const char* callstack;
} Display_ShowCrash_Info_t;

#define DISPLAY_IOCMD_GET_INFO          DEVICE_OBJECT_IOCMD_DEF(DeviceObject::DIR_OUT, sizeof(HAL::Display_Info_t), 0, 0)
#define DISPLAY_IOCMD_PUT_AREA          DEVICE_OBJECT_IOCMD_DEF(DeviceObject::DIR_IN, sizeof(HAL::Display_PutArea_Info_t), 1, 0)
#define DISPLAY_IOCMD_SET_CALLBACK      DEVICE_OBJECT_IOCMD_DEF(DeviceObject::DIR_IN, sizeof(HAL::Display_SetCallback_Info_t), 2, 0)
#define DISPLAY_IOCMD_SET_ROTATION      DEVICE_OBJECT_IOCMD_DEF(DeviceObject::DIR_IN, sizeof(int), 3, 0)
#define DISPLAY_IOCMD_SHOW_CRASH_INFO   DEVICE_OBJECT_IOCMD_DEF(DeviceObject::DIR_IN, sizeof(HAL::Display_ShowCrash_Info_t), 4, 0)
#define DISPLAY_IOCMD_FILL_COLOR        DEVICE_OBJECT_IOCMD_DEF(DeviceObject::DIR_IN, sizeof(uint16_t), 5, 0)

/* clang-format on */

} // namespace HAL

#endif // __HAL_DISPLAY_DEF_H
