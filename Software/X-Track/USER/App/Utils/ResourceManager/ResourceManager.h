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
#ifndef __RESOURCE_MANAGER_H
#define __RESOURCE_MANAGER_H

#include <vector>

class ResourceManager
{

public:
    ResourceManager();
    ~ResourceManager();

    bool AddResource(const char* name, void* ptr);
    bool RemoveResource(const char* name);
    void* GetResource(const char* name);
    void SetDefault(void* ptr);

private:
    typedef struct ResourceNode
    {
        const char* name;
        void* ptr;

        bool operator==(const struct ResourceNode n) const
        {
            return (this->name == n.name && this->ptr == n.ptr);
        }
    } ResourceNode_t;

private:
    std::vector<ResourceNode_t> NodePool;
    void* DefaultPtr;
    bool SearchNode(const char* name, ResourceNode_t* node);
};

#endif
