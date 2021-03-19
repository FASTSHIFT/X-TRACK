/*
 * This file is part of the CmBacktrace Library.
 *
 * Copyright (c) 2020, Armink, <armink.ztl@gmail.com>
 *                     Chenxuan, <chenxuan.zhao@icloud.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * NOTE: DO NOT include this file on the header file.
 * Encoding: UTF-8
 * Created on: 2020-09-06
 */

[PRINT_MAIN_STACK_CFG_ERROR]  = "错误：无法获取主栈信息，请检查主栈的相关配置",
[PRINT_FIRMWARE_INFO]         = "固件名称：%s，硬件版本号：%s，软件版本号：%s",
[PRINT_ASSERT_ON_THREAD]      = "在线程(%s)中发生断言",
[PRINT_ASSERT_ON_HANDLER]     = "在中断或裸机环境下发生断言",
[PRINT_THREAD_STACK_INFO]     = "=========== 线程堆栈信息 ===========",
[PRINT_MAIN_STACK_INFO]       = "============ 主堆栈信息 ============",
[PRINT_THREAD_STACK_OVERFLOW] = "错误：线程栈(%08x)发生溢出",
[PRINT_MAIN_STACK_OVERFLOW]   = "错误：主栈(%08x)发生溢出",
[PRINT_CALL_STACK_INFO]       = "查看更多函数调用栈信息，请运行：addr2line -e %s%s -a -f %.*s",
[PRINT_CALL_STACK_ERR]        = "获取函数调用栈失败",
[PRINT_FAULT_ON_THREAD]       =  "在线程(%s)中发生错误异常",
[PRINT_FAULT_ON_HANDLER]      = "在中断或裸机环境下发生错误异常",
[PRINT_REGS_TITLE]            = "========================= 寄存器信息 =========================",
[PRINT_HFSR_VECTBL]           = "发生硬错误，原因：取中断向量时出错",
[PRINT_MFSR_IACCVIOL]         = "发生存储器管理错误，原因：企图从不允许访问的区域取指令",
[PRINT_MFSR_DACCVIOL]         = "发生存储器管理错误，原因：企图从不允许访问的区域读、写数据",
[PRINT_MFSR_MUNSTKERR]        = "发生存储器管理错误，原因：出栈时企图访问不被允许的区域",
[PRINT_MFSR_MSTKERR]          = "发生存储器管理错误，原因：入栈时企图访问不被允许的区域",
[PRINT_MFSR_MLSPERR]          = "发生存储器管理错误，原因：惰性保存浮点状态时发生错误",
[PRINT_BFSR_IBUSERR]          = "发生总线错误，原因：指令总线错误",
[PRINT_BFSR_PRECISERR]        = "发生总线错误，原因：精确的数据总线错误",
[PRINT_BFSR_IMPREISERR]       = "发生总线错误，原因：不精确的数据总线错误",
[PRINT_BFSR_UNSTKERR]         = "发生总线错误，原因：出栈时发生错误",
[PRINT_BFSR_STKERR]           = "发生总线错误，原因：入栈时发生错误",
[PRINT_BFSR_LSPERR]           = "发生总线错误，原因：惰性保存浮点状态时发生错误",
[PRINT_UFSR_UNDEFINSTR]       = "发生用法错误，原因：企图执行未定义指令",
[PRINT_UFSR_INVSTATE]         = "发生用法错误，原因：试图切换到 ARM 状态",
[PRINT_UFSR_INVPC]            = "发生用法错误，原因：无效的异常返回码",
[PRINT_UFSR_NOCP]             = "发生用法错误，原因：企图执行协处理器指令",
#if (CMB_CPU_PLATFORM_TYPE == CMB_CPU_ARM_CORTEX_M33)
    [PRINT_UFSR_STKOF]        = "发生用法错误，原因：硬件检测到栈溢出",
#endif
[PRINT_UFSR_UNALIGNED]        = "发生用法错误，原因：企图执行非对齐访问",
[PRINT_UFSR_DIVBYZERO0]       = "发生用法错误，原因：企图执行除 0 操作",
[PRINT_DFSR_HALTED]           = "发生调试错误，原因：NVIC 停机请求",
[PRINT_DFSR_BKPT]             = "发生调试错误，原因：执行 BKPT 指令",
[PRINT_DFSR_DWTTRAP]          = "发生调试错误，原因：数据监测点匹配",
[PRINT_DFSR_VCATCH]           = "发生调试错误，原因：发生向量捕获",
[PRINT_DFSR_EXTERNAL]         = "发生调试错误，原因：外部调试请求",
[PRINT_MMAR]                  = "发生存储器管理错误的地址：%08x",
[PRINT_BFAR]                  = "发生总线错误的地址：%08x",
