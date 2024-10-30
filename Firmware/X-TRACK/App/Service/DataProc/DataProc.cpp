/*
 * MIT License
 * Copyright (c) 2021 - 2023 _VIFEXTech
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
#include "DataProc.h"
#include "Service/HAL/HAL.h"

static DataBroker* DataProcBroker = nullptr;

void DataProc_Init(DataBroker* broker)
{
    DataProcBroker = broker;

    broker->initTimerManager(HAL::GetTick);

/* Create DataNode */
#define DP_DEF(NAME) \
    DataNode* node##NAME = new DataNode(#NAME, broker);
#include "DataProc_NodeList.inc"
#undef DP_DEF

/* Init DataNode */
#define DP_DEF(NAME)                           \
    do {                                       \
        void DP_##NAME##_Init(DataNode* node); \
        DP_##NAME##_Init(node##NAME);          \
    } while (0)
#include "DataProc_NodeList.inc"
#undef DP_DEF
}

DataBroker* DataProc::broker()
{
    return DataProcBroker;
}
