#include "HAL.h"
#include "StackInfo/StackInfo.h"
#include <stdarg.h>
#include <stdlib.h>

#if CONFIG_SHOW_STACK_INFO
static void Memory_ShowStackInfo()
{
    Serial.printf(
        "Stack: Use = %0.2f%% Free = %d\r\n",
        StackInfo_GetMaxUtilization() * 100,
        StackInfo_GetMinFreeSize()
    );
}
#endif

#if CONFIG_SHOW_HEAP_INFO
static int HeapPrint(void* param, char const* format, ...)
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
    __heapstats((__heapprt)HeapPrint, &size);
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
