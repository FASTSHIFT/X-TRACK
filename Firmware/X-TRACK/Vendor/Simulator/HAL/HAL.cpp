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

/* Import Device */
#define HAL_DEF(name) extern DeviceObject DevObj_##name;
#include "HAL_DeviceTree.inc"
#undef HAL_DEF

DeviceManager* HAL::Manager()
{
    /* Device Array */
    static DeviceObject* devObjArr[] = {
#define HAL_DEF(name) &(DevObj_##name),
#include "HAL_DeviceTree.inc"
#undef HAL_DEF
    };

    static DeviceManager manager(devObjArr, CM_ARRAY_SIZE(devObjArr));
    return &manager;
}

void HAL::Init()
{
    HAL_Log_Init();
    HAL_LOG_INFO("begin");

    Manager()->init([](DeviceManager* manager, DeviceObject* dev, int retval) {
        if (retval < 0) {
            HAL_LOG_ERROR("[%s] init fail: %d", dev->getName(), retval);
        } else {
            HAL_LOG_INFO("[%s] init success", dev->getName());
        }
    });

    HAL_LOG_INFO("end");
}
