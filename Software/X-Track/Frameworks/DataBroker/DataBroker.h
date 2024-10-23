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
#ifndef __DATA_BROKER_H
#define __DATA_BROKER_H

#include "DataNode.h"

class DataTimerManager;

class DataBroker {
    friend class DataNode;

public:
    /**
     * @brief  Data broker constructor
     * @param  name: The name of the data broker
     */
    DataBroker(const char* name);

    /**
     * @brief  Data broker destructor
     */
    ~DataBroker();

    /**
     * @brief  Get the number of nodes in the node pool
     * @retval Number of nodes
     */
    size_t getNodeNumber();

    /**
     * @brief  Get the main node
     * @retval Pointer to the main node
     */
    DataNode* mainNode()
    {
        return _mainNode;
    }

    /**
     * @brief  Set the main node
     * @param  node: Pointer to the main node
     */
    void initTimerManager(uint32_t (*tickFunction)(void));

    /**
     * @brief  Handle timer
     * @retval Time till next timer event in millisecond
     */
    uint32_t handleTimer();

protected:
    /**
     * @brief  Add an node to the node pool
     * @param  node: Pointer to node
     * @retval If the addition is successful, return true
     */
    bool add(DataNode* node);

    /**
     * @brief  Remove the node from the node pool
     * @param  node: Pointer to node
     * @retval Return true if the removal is successful
     */
    bool remove(DataNode* node);

    /**
     * @brief  Remove node in vector
     * @param  vec: Pointer to vector
     * @param  id:  DataNode ID
     * @retval Return true if the removal is successful
     */
    bool remove(DataNode::DataNodeList_t* vec, DataNode* node);

    /**
     * @brief  Search node
     * @param  id: DataNode ID
     * @retval If the search is successful, return the pointer of the node
     */
    DataNode* search(const char* id);

    /**
     * @brief  Search node in vector
     * @param  vec: Pointer to vector
     * @param  id:  DataNode ID
     * @retval If the search is successful, return the pointer of the node
     */
    DataNode* search(DataNode::DataNodeList_t* vec, const char* id);

    /**
     * @brief  Get the timer manager
     * @retval Pointer to the timer manager
     */
    DataTimerManager* timerManager()
    {
        return _timerManager;
    }

private:
    /* DataNode pool */
    DataNode::DataNodeList_t* _nodePool;

    /* Main node, will automatically follow all nodes */
    DataNode* _mainNode;

    /* Timer manager */
    DataTimerManager* _timerManager;

    /* The name of the data center will be used as the ID of the main node */
    const char* _name;
};

#endif
