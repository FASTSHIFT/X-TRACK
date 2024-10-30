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
#ifndef __DATA_PROC_STORAGE_DEF_H
#define __DATA_PROC_STORAGE_DEF_H

#include <cstdint>

namespace DataProc {

enum class STORAGE_CMD {
    LOAD,
    SAVE,
    ADD,
    MOUNT,
    UNMOUNT,
};

enum class STORAGE_TYPE {
    BOOL = 1,
    INT,
    FLOAT,
    DOUBLE,
    STRING,
    ARRAY,
    STRUCT_START,
    STRUCT_END,
};

typedef struct Storage_Info {
    Storage_Info()
        : cmd(STORAGE_CMD::LOAD)
        , key(nullptr)
        , value(nullptr)
        , size(0)
        , type(STORAGE_TYPE::BOOL)
    {
    }
    STORAGE_CMD cmd;
    const char* key;
    void* value;
    uint16_t size;
    STORAGE_TYPE type;
} Storage_Info_t;

typedef struct Storage_Device_Info {
    Storage_Device_Info()
        : isDetect(false)
        , totalSize(0)
        , freeSize(0)
    {
    }
    bool isDetect;
    bool isActive;
    uint64_t totalSize;
    uint64_t freeSize;
} Storage_Device_Info_t;

} // namespace DataProc

#endif // __DATA_PROC_STORAGE_DEF_H
