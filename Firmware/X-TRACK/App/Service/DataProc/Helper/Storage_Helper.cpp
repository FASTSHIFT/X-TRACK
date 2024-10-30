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
#include "Storage_Helper.h"
#include "Frameworks/DataBroker/DataBroker.h"

using namespace DataProc;

Storage_Helper::Storage_Helper(DataNode* node)
    : _node(node)
{
    _nodeStorage = node->subscribe("Storage");
}

int Storage_Helper::add(const char* key, void* value, uint32_t size, DataProc::STORAGE_TYPE type)
{
    DataProc::Storage_Info_t storageInfo;
    storageInfo.cmd = DataProc::STORAGE_CMD::ADD;
    storageInfo.key = key;
    storageInfo.value = value;
    storageInfo.size = size;
    storageInfo.type = type;
    return _node->notify(_nodeStorage, &storageInfo, sizeof(storageInfo));
}

int Storage_Helper::add(const char* key, float* value)
{
    return add(key, value, sizeof(float), DataProc::STORAGE_TYPE::FLOAT);
}

int Storage_Helper::add(const char* key, int* value)
{
    return add(key, value, sizeof(int), DataProc::STORAGE_TYPE::INT);
}

int Storage_Helper::add(const char* key, bool* value)
{
    return add(key, value, sizeof(bool), DataProc::STORAGE_TYPE::BOOL);
}

int Storage_Helper::addArray(const char* key, void* value, uint32_t size, uint32_t len, DataProc::STORAGE_TYPE type)
{
    DataProc::Storage_Info_t storageInfo;
    storageInfo.cmd = DataProc::STORAGE_CMD::ADD;
    storageInfo.key = key;
    storageInfo.size = len;
    storageInfo.type = DataProc::STORAGE_TYPE::ARRAY;

    auto ret = _node->notify(_nodeStorage, &storageInfo, sizeof(storageInfo));
    if (ret != DataNode::RES_OK) {
        return ret;
    }

    storageInfo.cmd = DataProc::STORAGE_CMD::ADD;
    storageInfo.key = nullptr;
    storageInfo.value = value;
    storageInfo.size = size;
    storageInfo.type = type;
    return _node->notify(_nodeStorage, &storageInfo, sizeof(storageInfo));
}

int Storage_Helper::structStart(const char* key)
{
    DataProc::Storage_Info_t storageInfo;
    storageInfo.cmd = DataProc::STORAGE_CMD::ADD;
    storageInfo.key = key;
    storageInfo.value = nullptr;
    storageInfo.size = 0;
    storageInfo.type = DataProc::STORAGE_TYPE::STRUCT_START;
    return _node->notify(_nodeStorage, &storageInfo, sizeof(storageInfo));
}

int Storage_Helper::structEnd()
{
    DataProc::Storage_Info_t storageInfo;
    storageInfo.cmd = DataProc::STORAGE_CMD::ADD;
    storageInfo.key = nullptr;
    storageInfo.value = nullptr;
    storageInfo.size = 0;
    storageInfo.type = DataProc::STORAGE_TYPE::STRUCT_END;
    return _node->notify(_nodeStorage, &storageInfo, sizeof(storageInfo));
}
