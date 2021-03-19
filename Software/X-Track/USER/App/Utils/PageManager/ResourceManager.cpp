/*
 * MIT License
 * Copyright (c) 2021 _VIFEXTech
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the follo18wing conditions:
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
#include "ResourceManager.h"
#include "PageManagerLog.h"

ResourceManager::ResourceManager()
{
    _lv_ll_init(&ResourceList, sizeof(ResourceNode_t));
    DefaultPtr = nullptr;
}

ResourceManager::~ResourceManager()
{
    _lv_ll_clear(&ResourceList);
}

ResourceManager::ResourceNode_t* ResourceManager::SearchNode(const char* name)
{
    void* i;
    _LV_LL_READ(ResourceList, i)
    {
        ResourceNode_t* node = (ResourceNode_t*)i;
        if (strcmp(name, node->name) == 0)
        {
            return node;
        }
    }
    return nullptr;
}

bool ResourceManager::AddResource(const char* name, void* ptr)
{
    if (SearchNode(name) != nullptr)
    {
        PM_LOG_WARN("Resource: %s was register", name);
        return false;
    }

    ResourceNode_t* node = (ResourceNode_t*)_lv_ll_ins_tail(&ResourceList);
    node->name = name;
    node->ptr = ptr;

    PM_LOG_INFO("Resource: %s[0x%p] add success", node->name, node->ptr);

    return true;
}

bool ResourceManager::RemoveResource(const char* name)
{
    ResourceNode_t* node = SearchNode(name);

    if (node == nullptr)
    {
        PM_LOG_ERROR("Resource: %s was not found", name);
        return false;
    }
    
    _lv_ll_remove(&ResourceList, node);

    PM_LOG_INFO("Resource: %s remove success", name);

    return true;
}

void* ResourceManager::GetResource(const char* name)
{
    ResourceNode_t* node = SearchNode(name);

    if (node == nullptr)
    {
        PM_LOG_WARN("Resource: %s was not found, return default[0x%p]", name, DefaultPtr);
        return DefaultPtr;
    }

    PM_LOG_INFO("Resource: %s[0x%p] was found", name, node->ptr);

    return node->ptr;
}

void ResourceManager::SetDefault(void* ptr)
{
    DefaultPtr = ptr;
    PM_LOG_INFO("Resource: set [0x%p] to default", DefaultPtr);
}
