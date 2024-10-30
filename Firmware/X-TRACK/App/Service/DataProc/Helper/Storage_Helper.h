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
#ifndef __DATA_PROC_STORAGE_HELPER_H
#define __DATA_PROC_STORAGE_HELPER_H

#include "../Def/DP_Storage.h"

class DataNode;

namespace DataProc {

class Storage_Helper {
public:
    Storage_Helper(DataNode* node);
    int add(const char* key, void* value, uint32_t size, DataProc::STORAGE_TYPE type);
    int add(const char* key, float* value);
    int add(const char* key, int* value);
    int add(const char* key, bool* value);
    int addArray(const char* key, void* value, uint32_t size, uint32_t len, DataProc::STORAGE_TYPE type);
    int structStart(const char* key);
    int structEnd();
    operator const DataNode*() const
    {
        return _nodeStorage;
    }

private:
    DataNode* _node;
    const DataNode* _nodeStorage;
};

} // namespace DataProc

#endif // __DATA_PROC_STORAGE_HELPER_H
