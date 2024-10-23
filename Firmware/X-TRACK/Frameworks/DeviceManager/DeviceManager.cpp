/*
 * MIT License
 * Copyright (c) 2022 _VIFEXTech
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

#include "DeviceManager.h"
#include "DeviceObject.h"
#include <string.h>

DeviceManager::DeviceManager(DeviceObject** devArray, size_t len)
{
    _devArray = devArray;
    _devArrayLen = len;
}

DeviceManager::~DeviceManager()
{
}

void DeviceManager::init(InitFinishCallback_t callback)
{
    for (size_t i = 0; i < _devArrayLen; i++) {
        DeviceObject* dev = _devArray[i];
        int retval = dev->init();
        if (callback) {
            callback(this, dev, retval);
        }
    }
}

DeviceObject* DeviceManager::getDevice(const char* name)
{
    for (size_t i = 0; i < _devArrayLen; i++) {
        DeviceObject* dev = _devArray[i];
        if (strcmp(name, dev->getName()) == 0) {
            return dev;
        }
    }
    return nullptr;
}
