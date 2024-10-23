#include "HAL.h"

#if CONFIG_SHOW_STACK_INFO

#include "StackInfo/StackInfo.h"

static void Memory_ShowStackInfo()
{
    Serial.printf(
        "Stack: %0.2f%% used (total: 0x%x, free: 0x%x)\r\n",
        StackInfo_GetMaxUtilization() * 100,
        StackInfo_GetTotalSize(),
        StackInfo_GetMinFreeSize()
    );
}
#endif

#if CONFIG_SHOW_HEAP_INFO

#include <stdarg.h>
#include <stdlib.h>

static int Memory_HeapPrint(void* param, char const* format, ...)
{
    char printf_buff[64];

    va_list args;
    va_start(args, format);

    int ret_status = vsnprintf(printf_buff, sizeof(printf_buff), format, args);

    va_end(args);

    Serial.print("Heap: ");
    Serial.println(printf_buff);

    return ret_status;
}

static void Memory_ShowHeapInfo()
{
    int size = 0;
    __heapstats((__heapprt)Memory_HeapPrint, &size);
}
#endif

void HAL::Memory_DumpInfo()
{
#if CONFIG_SHOW_STACK_INFO
    Memory_ShowStackInfo();
#endif

#if CONFIG_SHOW_HEAP_INFO
    Memory_ShowHeapInfo();
#endif
}
