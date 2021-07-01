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
#include "ResourceManager.h"
#include "PM_Log.h"
#include <algorithm>
#include <string.h>

ResourceManager::ResourceManager()
{
    DefaultPtr = nullptr;
}

ResourceManager::~ResourceManager()
{
}

/**
  * @brief  Search resource node based on name
  * @param  name: Resource Name
  * @param  node: Pointer to the resource node
  * @retval Return true if the search is successful
  */
bool ResourceManager::SearchNode(const char* name, ResourceNode_t* node)
{
    for(auto iter : NodePool)
    {
        if (strcmp(name, iter.name) == 0)
        {
            *node = iter;
            return true;
        }
    }
    return false;
}

/**
  * @brief  Add resources to the resource pool
  * @param  name: Resource Name
  * @param  ptr: Pointer to the resource
  * @retval Return true if the addition is successful
  */
bool ResourceManager::AddResource(const char* name, void* ptr)
{
    ResourceNode_t node;
    if (SearchNode(name, &node))
    {
        PM_LOG_WARN("Resource: %s was register", name);
        return false;
    }

    node.name = name;
    node.ptr = ptr;
    NodePool.push_back(node);

    PM_LOG_INFO("Resource: %s[0x%p] add success", node.name, node.ptr);

    return true;
}

/**
  * @brief  Remove resources from the resource pool
  * @param  name: Resource Name
  * @retval Return true if the removal is successful
  */
bool ResourceManager::RemoveResource(const char* name)
{
    ResourceNode_t node;
    if(!SearchNode(name, &node))
    {
        PM_LOG_ERROR("Resource: %s was not found", name);
        return false;
    }

    auto iter = std::find(NodePool.begin(), NodePool.end(), node);

    if (iter == NodePool.end())
    {
        PM_LOG_ERROR("Resource: %s was not found", name);
        return false;
    }

    NodePool.erase(iter);

    PM_LOG_INFO("Resource: %s remove success", name);

    return true;
}

/**
  * @brief  Get resource address
  * @param  name: Resource Name
  * @retval If the acquisition is successful, return the address of the resource, otherwise return the default resource
  */
void* ResourceManager::GetResource(const char* name)
{
    ResourceNode_t node;

    if(!SearchNode(name, &node))
    {
        PM_LOG_WARN("Resource: %s was not found, return default[0x%p]", name, DefaultPtr);
        return DefaultPtr;
    }

    PM_LOG_INFO("Resource: %s[0x%p] was found", name, node.ptr);

    return node.ptr;
}

/**
  * @brief  Set default resources
  * @param  ptr: Pointer to the default resource
  * @retval None
  */
void ResourceManager::SetDefault(void* ptr)
{
    DefaultPtr = ptr;
    PM_LOG_INFO("Resource: set [0x%p] to default", DefaultPtr);
}
