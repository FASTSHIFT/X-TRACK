/*
 * MIT License
 * Copyright (c) 2021 _VIFEXTech
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
#ifndef __STORAGE_SERVICE_H
#define __STORAGE_SERVICE_H

#include <stdint.h>
#include <vector>

class StorageService
{
public:
    typedef enum
    {
        TYPE_UNKNOW,
        TYPE_INT,
        TYPE_FLOAT,
        TYPE_DOUBLE,
        TYPE_STRING
    } DataType_t;

public:
    StorageService(const char* filePath, uint32_t bufferSize = 1024);
    ~StorageService();

    bool Add(const char* key, void* value, uint16_t size, DataType_t type);
    bool Remove(const char* key);
    bool SaveFile(const char* backupPath = nullptr);
    bool LoadFile();

private:
    typedef struct
    {
        const char* key;
        void* value;
        uint16_t size;
        DataType_t type;
    } Node_t;

private:
    const char* FilePath;
    std::vector<Node_t*> NodePool;
    uint32_t BufferSize;

private:
    Node_t* SearchNode(const char* key);    
};

#endif
