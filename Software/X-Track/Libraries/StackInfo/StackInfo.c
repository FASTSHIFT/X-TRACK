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
#include "StackInfo.h"

#define CSTACK_BLOCK_NAME                    STACK

#define SECTION_START(_name_)                _name_##$$Base
#define SECTION_END(_name_)                  _name_##$$Limit

#define CSTACK_BLOCK_START(_name_)           SECTION_START(_name_)
#define CSTACK_BLOCK_END(_name_)             SECTION_END(_name_)

extern const int CSTACK_BLOCK_START(CSTACK_BLOCK_NAME);
extern const int CSTACK_BLOCK_END(CSTACK_BLOCK_NAME);

uint32_t StackInfo_GetTotalSize(void)
{
    uint32_t stackBaseAddr = (uint32_t)&CSTACK_BLOCK_START(CSTACK_BLOCK_NAME);
    uint32_t stackSize = (uint32_t)&CSTACK_BLOCK_END(CSTACK_BLOCK_NAME) - stackBaseAddr;
    return stackSize;
}

uint32_t StackInfo_GetMaxUsageSize(void)
{
    static uint32_t stackMaxUsage = 0;
    uint32_t stackBaseAddr = (uint32_t)&CSTACK_BLOCK_START(CSTACK_BLOCK_NAME);
    uint32_t stackSize = StackInfo_GetTotalSize();

    volatile uint32_t* stackBase = (uint32_t*)stackBaseAddr;

    uint32_t usageSize = 0;
    uint32_t size = stackSize / sizeof(uint32_t);

    for(uint32_t i = 0; i < size; i++)
    {
        if(stackBase[i] != STACK_INFO_BLANK)
        {
            usageSize = size - i;
            break;
        }
    }

    if(usageSize > stackMaxUsage)
    {
        stackMaxUsage = usageSize;
    }

    return stackMaxUsage * sizeof(uint32_t);
}

uint32_t StackInfo_GetMinFreeSize(void)
{
    return StackInfo_GetTotalSize() - StackInfo_GetMaxUsageSize();
}

float StackInfo_GetMaxUtilization(void)
{
    return (float)StackInfo_GetMaxUsageSize() / StackInfo_GetTotalSize();
}
