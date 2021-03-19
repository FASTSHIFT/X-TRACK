/*
 * This file is part of the CmBacktrace Library.
 *
 * Copyright (c) 2016-2020, Armink, <armink.ztl@gmail.com>
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
 * Function: It is the macro definition head file for this library.
 * Created on: 2016-12-15
 */

#ifndef _CMB_DEF_H_
#define _CMB_DEF_H_

#include "cmb_cfg.h"
#include <stdint.h>
#include <stdlib.h>

/* library software version number */
#define CMB_SW_VERSION                "1.4.0"

#define CMB_CPU_ARM_CORTEX_M0             0
#define CMB_CPU_ARM_CORTEX_M3             1
#define CMB_CPU_ARM_CORTEX_M4             2
#define CMB_CPU_ARM_CORTEX_M7             3
#define CMB_CPU_ARM_CORTEX_M33            4

#define CMB_OS_PLATFORM_RTT               0
#define CMB_OS_PLATFORM_UCOSII            1
#define CMB_OS_PLATFORM_UCOSIII           2
#define CMB_OS_PLATFORM_FREERTOS          3

#define CMB_PRINT_LANGUAGE_ENGLISH        0
#define CMB_PRINT_LANGUAGE_CHINESE        1
#define CMB_PRINT_LANGUAGE_CHINESE_UTF8   2

/* name max length, default size: 32 */
#ifndef CMB_NAME_MAX
#define CMB_NAME_MAX                      32
#endif

/* print information language, default is English */
#ifndef CMB_PRINT_LANGUAGE
#define CMB_PRINT_LANGUAGE                CMB_PRINT_LANGUAGE_ENGLISH
#endif


#if defined(__CC_ARM) || defined(__CLANG_ARM)
    /* C stack block name, default is STACK */
    #ifndef CMB_CSTACK_BLOCK_NAME
    #define CMB_CSTACK_BLOCK_NAME          STACK
    #endif
    /* code section name, default is ER_IROM1 */
    #ifndef CMB_CODE_SECTION_NAME
    #define CMB_CODE_SECTION_NAME          ER_IROM1
    #endif
#elif defined(__ICCARM__)
    /* C stack block name, default is 'CSTACK' */
    #ifndef CMB_CSTACK_BLOCK_NAME
    #define CMB_CSTACK_BLOCK_NAME          "CSTACK"
    #endif
    /* code section name, default is '.text' */
    #ifndef CMB_CODE_SECTION_NAME
    #define CMB_CODE_SECTION_NAME          ".text"
    #endif
#elif defined(__GNUC__)
    /* C stack block start address, defined on linker script file, default is _sstack */
    #ifndef CMB_CSTACK_BLOCK_START
    #define CMB_CSTACK_BLOCK_START         _sstack
    #endif
    /* C stack block end address, defined on linker script file, default is _estack */
    #ifndef CMB_CSTACK_BLOCK_END
    #define CMB_CSTACK_BLOCK_END           _estack
    #endif
    /* code section start address, defined on linker script file, default is _stext */
    #ifndef CMB_CODE_SECTION_START
    #define CMB_CODE_SECTION_START         _stext
    #endif
    /* code section end address, defined on linker script file, default is _etext */
    #ifndef CMB_CODE_SECTION_END
    #define CMB_CODE_SECTION_END           _etext
    #endif
#else
    #error "not supported compiler"
#endif

/* supported function call stack max depth, default is 16 */
#ifndef CMB_CALL_STACK_MAX_DEPTH
#define CMB_CALL_STACK_MAX_DEPTH       16
#endif

/* system handler control and state register */
#ifndef CMB_SYSHND_CTRL
#define CMB_SYSHND_CTRL                (*(volatile unsigned int*)  (0xE000ED24u))
#endif

/* memory management fault status register */
#ifndef CMB_NVIC_MFSR
#define CMB_NVIC_MFSR                  (*(volatile unsigned char*) (0xE000ED28u))
#endif

/* bus fault status register */
#ifndef CMB_NVIC_BFSR
#define CMB_NVIC_BFSR                  (*(volatile unsigned char*) (0xE000ED29u))
#endif

/* usage fault status register */
#ifndef CMB_NVIC_UFSR
#define CMB_NVIC_UFSR                  (*(volatile unsigned short*)(0xE000ED2Au))
#endif

/* hard fault status register */
#ifndef CMB_NVIC_HFSR
#define CMB_NVIC_HFSR                  (*(volatile unsigned int*)  (0xE000ED2Cu))
#endif

/* debug fault status register */
#ifndef CMB_NVIC_DFSR
#define CMB_NVIC_DFSR                  (*(volatile unsigned short*)(0xE000ED30u))
#endif

/* memory management fault address register */
#ifndef CMB_NVIC_MMAR
#define CMB_NVIC_MMAR                  (*(volatile unsigned int*)  (0xE000ED34u))
#endif

/* bus fault manage address register */
#ifndef CMB_NVIC_BFAR
#define CMB_NVIC_BFAR                  (*(volatile unsigned int*)  (0xE000ED38u))
#endif

/* auxiliary fault status register */
#ifndef CMB_NVIC_AFSR
#define CMB_NVIC_AFSR                  (*(volatile unsigned short*)(0xE000ED3Cu))
#endif

/**
 * Cortex-M fault registers
 */
struct cmb_hard_fault_regs{
  struct {
    unsigned int r0;                     // Register R0
    unsigned int r1;                     // Register R1
    unsigned int r2;                     // Register R2
    unsigned int r3;                     // Register R3
    unsigned int r12;                    // Register R12
    unsigned int lr;                     // Link register
    unsigned int pc;                     // Program counter
    union {
      unsigned int value;
      struct {
#if (CMB_CPU_PLATFORM_TYPE == CMB_CPU_ARM_CORTEX_M33)
        unsigned int IPSR : 9;           // Interrupt Program Status register (IPSR)
        unsigned int EPSR : 18;          // Execution Program Status register (EPSR)
        unsigned int APSR : 5;           // Application Program Status register (APSR)
#else
        unsigned int IPSR : 8;           // Interrupt Program Status register (IPSR)
        unsigned int EPSR : 19;          // Execution Program Status register (EPSR)
        unsigned int APSR : 5;           // Application Program Status register (APSR)
#endif
      } bits;
    } psr;                               // Program status register.
  } saved;

  union {
    unsigned int value;
    struct {
      unsigned int MEMFAULTACT    : 1;   // Read as 1 if memory management fault is active
      unsigned int BUSFAULTACT    : 1;   // Read as 1 if bus fault exception is active
#if (CMB_CPU_PLATFORM_TYPE == CMB_CPU_ARM_CORTEX_M33)
      unsigned int HARDFAULTACT   : 1;   // Read as 1 if hardfault is active
#else
      unsigned int UnusedBits1    : 1;
#endif
      unsigned int USGFAULTACT    : 1;   // Read as 1 if usage fault exception is active
#if (CMB_CPU_PLATFORM_TYPE == CMB_CPU_ARM_CORTEX_M33)
      unsigned int SECUREFAULTACT : 1;   // Read as 1 if secure fault exception is active
      unsigned int NMIACT         : 1;   // Read as 1 if NMI exception is active
      unsigned int UnusedBits2    : 1;
#else
      unsigned int UnusedBits2    : 3;
#endif
      unsigned int SVCALLACT      : 1;   // Read as 1 if SVC exception is active
      unsigned int MONITORACT     : 1;   // Read as 1 if debug monitor exception is active
      unsigned int UnusedBits3    : 1;
      unsigned int PENDSVACT      : 1;   // Read as 1 if PendSV exception is active
      unsigned int SYSTICKACT     : 1;   // Read as 1 if SYSTICK exception is active
      unsigned int USGFAULTPENDED : 1;   // Usage fault pended; usage fault started but was replaced by a higher-priority exception
      unsigned int MEMFAULTPENDED : 1;   // Memory management fault pended; memory management fault started but was replaced by a higher-priority exception
      unsigned int BUSFAULTPENDED : 1;   // Bus fault pended; bus fault handler was started but was replaced by a higher-priority exception
      unsigned int SVCALLPENDED   : 1;   // SVC pended; SVC was started but was replaced by a higher-priority exception
      unsigned int MEMFAULTENA    : 1;   // Memory management fault handler enable
      unsigned int BUSFAULTENA    : 1;   // Bus fault handler enable
      unsigned int USGFAULTENA    : 1;   // Usage fault handler enable
#if (CMB_CPU_PLATFORM_TYPE == CMB_CPU_ARM_CORTEX_M33)
      unsigned int SECUREFAULTENA : 1;   // Secure fault handler enable
      unsigned int SECUREFAULTPENDED : 1;   // Secure fault pended; Secure fault handler was started but was replaced by a higher-priority exception
      unsigned int HARDFAULTPENDED   : 1;   // Hard fault pended; Hard fault handler was started but was replaced by a higher-priority exception
#else
      // None
#endif
    } bits;
  } syshndctrl;                          // System Handler Control and State Register (0xE000ED24)

  union {
    unsigned char value;
    struct {
      unsigned char IACCVIOL    : 1;     // Instruction access violation
      unsigned char DACCVIOL    : 1;     // Data access violation
      unsigned char UnusedBits  : 1;
      unsigned char MUNSTKERR   : 1;     // Unstacking error
      unsigned char MSTKERR     : 1;     // Stacking error
      unsigned char MLSPERR     : 1;     // Floating-point lazy state preservation (M4/M7)
      unsigned char UnusedBits2 : 1;
      unsigned char MMARVALID   : 1;     // Indicates the MMAR is valid
    } bits;
  } mfsr;                                // Memory Management Fault Status Register (0xE000ED28)
  unsigned int mmar;                     // Memory Management Fault Address Register (0xE000ED34)

  union {
    unsigned char value;
    struct {
      unsigned char IBUSERR    : 1;      // Instruction access violation
      unsigned char PRECISERR  : 1;      // Precise data access violation
      unsigned char IMPREISERR : 1;      // Imprecise data access violation
      unsigned char UNSTKERR   : 1;      // Unstacking error
      unsigned char STKERR     : 1;      // Stacking error
      unsigned char LSPERR     : 1;      // Floating-point lazy state preservation (M4/M7)
      unsigned char UnusedBits : 1;
      unsigned char BFARVALID  : 1;      // Indicates BFAR is valid
    } bits;
  } bfsr;                                // Bus Fault Status Register (0xE000ED29)
  unsigned int bfar;                     // Bus Fault Manage Address Register (0xE000ED38)

  union {
    unsigned short value;
    struct {
      unsigned short UNDEFINSTR : 1;     // Attempts to execute an undefined instruction
      unsigned short INVSTATE   : 1;     // Attempts to switch to an invalid state (e.g., ARM)
      unsigned short INVPC      : 1;     // Attempts to do an exception with a bad value in the EXC_RETURN number
      unsigned short NOCP       : 1;     // Attempts to execute a coprocessor instruction
#if (CMB_CPU_PLATFORM_TYPE == CMB_CPU_ARM_CORTEX_M33)
      unsigned short STKOF      : 1;     // Indicates a stack overflow error has occured
      unsigned short UnusedBits : 3;
#else
      unsigned short UnusedBits : 4;
#endif
      unsigned short UNALIGNED  : 1;     // Indicates that an unaligned access fault has taken place
      unsigned short DIVBYZERO0 : 1;     // Indicates a divide by zero has taken place (can be set only if DIV_0_TRP is set)
    } bits;
  } ufsr;                                // Usage Fault Status Register (0xE000ED2A)

  union {
    unsigned int value;
    struct {
      unsigned int UnusedBits  : 1;
      unsigned int VECTBL      : 1;      // Indicates hard fault is caused by failed vector fetch
      unsigned int UnusedBits2 : 28;
      unsigned int FORCED      : 1;      // Indicates hard fault is taken because of bus fault/memory management fault/usage fault
      unsigned int DEBUGEVT    : 1;      // Indicates hard fault is triggered by debug event
    } bits;
  } hfsr;                                // Hard Fault Status Register (0xE000ED2C)

  union {
    unsigned int value;
    struct {
      unsigned int HALTED   : 1;         // Halt requested in NVIC
      unsigned int BKPT     : 1;         // BKPT instruction executed
      unsigned int DWTTRAP  : 1;         // DWT match occurred
      unsigned int VCATCH   : 1;         // Vector fetch occurred
      unsigned int EXTERNAL : 1;         // EDBGRQ signal asserted
    } bits;
  } dfsr;                                // Debug Fault Status Register (0xE000ED30)

  unsigned int afsr;                     // Auxiliary Fault Status Register (0xE000ED3C), Vendor controlled (optional)
};

/* assert for developer. */
#define CMB_ASSERT(EXPR)                                                       \
if (!(EXPR))                                                                   \
{                                                                              \
    cmb_println("(%s) has assert failed at %s.", #EXPR, __FUNCTION__);         \
    while (1);                                                                 \
}

/* ELF(Executable and Linking Format) file extension name for each compiler */
#if defined(__CC_ARM) || defined(__CLANG_ARM)
    #define CMB_ELF_FILE_EXTENSION_NAME          ".axf"
#elif defined(__ICCARM__)
    #define CMB_ELF_FILE_EXTENSION_NAME          ".out"
#elif defined(__GNUC__)
    #define CMB_ELF_FILE_EXTENSION_NAME          ".elf"
#else
    #error "not supported compiler"
#endif

#ifndef cmb_println
    #error "cmb_println isn't defined in 'cmb_cfg.h'"
#endif

#ifndef CMB_CPU_PLATFORM_TYPE
    #error "CMB_CPU_PLATFORM_TYPE isn't defined in 'cmb_cfg.h'"
#endif

#if (defined(CMB_USING_BARE_METAL_PLATFORM) && defined(CMB_USING_OS_PLATFORM))
    #error "CMB_USING_BARE_METAL_PLATFORM and CMB_USING_OS_PLATFORM only one of them can be used"
#elif defined(CMB_USING_OS_PLATFORM)
    #if !defined(CMB_OS_PLATFORM_TYPE)
        #error "CMB_OS_PLATFORM_TYPE isn't defined in 'cmb_cfg.h'"
    #endif /* !defined(CMB_OS_PLATFORM_TYPE) */
    #if (CMB_OS_PLATFORM_TYPE == CMB_OS_PLATFORM_RTT)
        #include <rtthread.h>
    #elif (CMB_OS_PLATFORM_TYPE == CMB_OS_PLATFORM_UCOSII)
        #include <ucos_ii.h>
    #elif (CMB_OS_PLATFORM_TYPE == CMB_OS_PLATFORM_UCOSIII)
        #include <os.h>
    #elif (CMB_OS_PLATFORM_TYPE == CMB_OS_PLATFORM_FREERTOS)
        #include <FreeRTOS.h>  
        extern uint32_t *vTaskStackAddr(void);/* need to modify the FreeRTOS/tasks source code */
        extern uint32_t vTaskStackSize(void);
        extern char * vTaskName(void);
    #else
        #error "not supported OS type"
    #endif /* (CMB_OS_PLATFORM_TYPE == CMB_OS_PLATFORM_RTT) */
#endif /* (defined(CMB_USING_BARE_METAL_PLATFORM) && defined(CMB_USING_OS_PLATFORM)) */

/* include or export for supported cmb_get_msp, cmb_get_psp and cmb_get_sp function */
#if defined(__CC_ARM)
    static __inline __asm uint32_t cmb_get_msp(void) {
        mrs r0, msp
        bx lr
    }
    static __inline __asm uint32_t cmb_get_psp(void) {
        mrs r0, psp
        bx lr
    }
    static __inline __asm uint32_t cmb_get_sp(void) {
        mov r0, sp
        bx lr
    }
#elif defined(__CLANG_ARM)
    __attribute__( (always_inline) ) static __inline uint32_t cmb_get_msp(void) {
        uint32_t result;
        __asm volatile ("mrs %0, msp" : "=r" (result) );
        return (result);
    }
    __attribute__( (always_inline) ) static __inline uint32_t cmb_get_psp(void) {
        uint32_t result;
        __asm volatile ("mrs %0, psp" : "=r" (result) );
        return (result);
    }
    __attribute__( (always_inline) ) static __inline uint32_t cmb_get_sp(void) {
        uint32_t result;
        __asm volatile ("mov %0, sp" : "=r" (result) );
        return (result);
    }
#elif defined(__ICCARM__)
/* IAR iccarm specific functions */
/* Close Raw Asm Code Warning */  
#pragma diag_suppress=Pe940    
    static uint32_t cmb_get_msp(void)
    {
      __asm("mrs r0, msp");
      __asm("bx lr");        
    }
    static uint32_t cmb_get_psp(void)
    {
      __asm("mrs r0, psp");
      __asm("bx lr");        
    }
    static uint32_t cmb_get_sp(void)
    {
      __asm("mov r0, sp");
      __asm("bx lr");       
    }
#pragma diag_default=Pe940  
#elif defined(__GNUC__)
    __attribute__( ( always_inline ) ) static inline uint32_t cmb_get_msp(void) {
        register uint32_t result;
        __asm volatile ("MRS %0, msp\n" : "=r" (result) );
        return(result);
    }
    __attribute__( ( always_inline ) ) static inline uint32_t cmb_get_psp(void) {
        register uint32_t result;
        __asm volatile ("MRS %0, psp\n" : "=r" (result) );
        return(result);
    }
    __attribute__( ( always_inline ) ) static inline uint32_t cmb_get_sp(void) {
        register uint32_t result;
        __asm volatile ("MOV %0, sp\n" : "=r" (result) );
        return(result);
    }
#else
    #error "not supported compiler"
#endif

#endif /* _CMB_DEF_H_ */
