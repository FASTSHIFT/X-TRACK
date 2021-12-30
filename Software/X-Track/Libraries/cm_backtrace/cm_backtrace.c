/*
 * This file is part of the CmBacktrace Library.
 *
 * Copyright (c) 2016-2019, Armink, <armink.ztl@gmail.com>
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
 * Function: Initialize function and other general function.
 * Created on: 2016-12-15
 */

#include "cm_backtrace.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#if __STDC_VERSION__ < 199901L
    #error "must be C99 or higher. try to add '-std=c99' to compile parameters"
#endif

#if defined(__CC_ARM) || defined(__CLANG_ARM)
    #define SECTION_START(_name_)                _name_##$$Base
    #define SECTION_END(_name_)                  _name_##$$Limit
    #define IMAGE_SECTION_START(_name_)          Image$$##_name_##$$Base
    #define IMAGE_SECTION_END(_name_)            Image$$##_name_##$$Limit
    #define CSTACK_BLOCK_START(_name_)           SECTION_START(_name_)
    #define CSTACK_BLOCK_END(_name_)             SECTION_END(_name_)
    #define CODE_SECTION_START(_name_)           IMAGE_SECTION_START(_name_)
    #define CODE_SECTION_END(_name_)             IMAGE_SECTION_END(_name_)

    extern const int CSTACK_BLOCK_START(CMB_CSTACK_BLOCK_NAME);
    extern const int CSTACK_BLOCK_END(CMB_CSTACK_BLOCK_NAME);
    extern const int CODE_SECTION_START(CMB_CODE_SECTION_NAME);
    extern const int CODE_SECTION_END(CMB_CODE_SECTION_NAME);
#elif defined(__ICCARM__)
    #pragma section=CMB_CSTACK_BLOCK_NAME
    #pragma section=CMB_CODE_SECTION_NAME
#elif defined(__GNUC__)
    extern const int CMB_CSTACK_BLOCK_START;
    extern const int CMB_CSTACK_BLOCK_END;
    extern const int CMB_CODE_SECTION_START;
    extern const int CMB_CODE_SECTION_END;
#else
    #error "not supported compiler"
#endif

enum {
    PRINT_MAIN_STACK_CFG_ERROR,
    PRINT_FIRMWARE_INFO,
    PRINT_ASSERT_ON_THREAD,
    PRINT_ASSERT_ON_HANDLER,
    PRINT_THREAD_STACK_INFO,
    PRINT_MAIN_STACK_INFO,
    PRINT_THREAD_STACK_OVERFLOW,
    PRINT_MAIN_STACK_OVERFLOW,
    PRINT_CALL_STACK_INFO,
    PRINT_CALL_STACK_ERR,
    PRINT_FAULT_ON_THREAD,
    PRINT_FAULT_ON_HANDLER,
    PRINT_REGS_TITLE,
    PRINT_HFSR_VECTBL,
    PRINT_MFSR_IACCVIOL,
    PRINT_MFSR_DACCVIOL,
    PRINT_MFSR_MUNSTKERR,
    PRINT_MFSR_MSTKERR,
    PRINT_MFSR_MLSPERR,
    PRINT_BFSR_IBUSERR,
    PRINT_BFSR_PRECISERR,
    PRINT_BFSR_IMPREISERR,
    PRINT_BFSR_UNSTKERR,
    PRINT_BFSR_STKERR,
    PRINT_BFSR_LSPERR,
    PRINT_UFSR_UNDEFINSTR,
    PRINT_UFSR_INVSTATE,
    PRINT_UFSR_INVPC,
    PRINT_UFSR_NOCP,
#if (CMB_CPU_PLATFORM_TYPE == CMB_CPU_ARM_CORTEX_M33)
    PRINT_UFSR_STKOF,
#endif
    PRINT_UFSR_UNALIGNED,
    PRINT_UFSR_DIVBYZERO0,
    PRINT_DFSR_HALTED,
    PRINT_DFSR_BKPT,
    PRINT_DFSR_DWTTRAP,
    PRINT_DFSR_VCATCH,
    PRINT_DFSR_EXTERNAL,
    PRINT_MMAR,
    PRINT_BFAR,
};

static const char * const print_info[] = {
#if (CMB_PRINT_LANGUAGE == CMB_PRINT_LANGUAGE_ENGLISH)
    #include "Languages/en-US/cmb_en_US.h"
#elif (CMB_PRINT_LANGUAGE == CMB_PRINT_LANGUAGE_CHINESE)
    #include "Languages/zh-CN/cmb_zh_CN.h"
#elif (CMB_PRINT_LANGUAGE == CMB_PRINT_LANGUAGE_CHINESE_UTF8)
    #include "Languages/zh-CN/cmb_zh_CN_UTF8.h"
#else
    #error "CMB_PRINT_LANGUAGE defined error in 'cmb_cfg.h'"
#endif
};

static char fw_name[CMB_NAME_MAX] = {0};
static char hw_ver[CMB_NAME_MAX] = {0};
static char sw_ver[CMB_NAME_MAX] = {0};
static uint32_t main_stack_start_addr = 0;
static size_t main_stack_size = 0;
static uint32_t code_start_addr = 0;
static size_t code_size = 0;
static bool init_ok = false;
static char call_stack_info[CMB_CALL_STACK_MAX_DEPTH * (8 + 1)] = { 0 };
static bool on_fault = false;
static bool stack_is_overflow = false;
static struct cmb_hard_fault_regs regs;

#if (CMB_CPU_PLATFORM_TYPE == CMB_CPU_ARM_CORTEX_M4) || (CMB_CPU_PLATFORM_TYPE == CMB_CPU_ARM_CORTEX_M7) || \
    (CMB_CPU_PLATFORM_TYPE == CMB_CPU_ARM_CORTEX_M33)
static bool statck_has_fpu_regs = false;
#endif

static bool on_thread_before_fault = false;

/**
 * library initialize
 */
void cm_backtrace_init(const char *firmware_name, const char *hardware_ver, const char *software_ver) {
    strncpy(fw_name, firmware_name, CMB_NAME_MAX - 1);
    strncpy(hw_ver, hardware_ver, CMB_NAME_MAX - 1);
    strncpy(sw_ver, software_ver, CMB_NAME_MAX - 1);

#if defined(__CC_ARM) || defined(__CLANG_ARM)
    main_stack_start_addr = (uint32_t)&CSTACK_BLOCK_START(CMB_CSTACK_BLOCK_NAME);
    main_stack_size = (uint32_t)&CSTACK_BLOCK_END(CMB_CSTACK_BLOCK_NAME) - main_stack_start_addr;
    code_start_addr = (uint32_t)&CODE_SECTION_START(CMB_CODE_SECTION_NAME);
    code_size = (uint32_t)&CODE_SECTION_END(CMB_CODE_SECTION_NAME) - code_start_addr;
#elif defined(__ICCARM__)
    main_stack_start_addr = (uint32_t)__section_begin(CMB_CSTACK_BLOCK_NAME);
    main_stack_size = (uint32_t)__section_end(CMB_CSTACK_BLOCK_NAME) - main_stack_start_addr;
    code_start_addr = (uint32_t)__section_begin(CMB_CODE_SECTION_NAME);
    code_size = (uint32_t)__section_end(CMB_CODE_SECTION_NAME) - code_start_addr;
#elif defined(__GNUC__)
    main_stack_start_addr = (uint32_t)(&CMB_CSTACK_BLOCK_START);
    main_stack_size = (uint32_t)(&CMB_CSTACK_BLOCK_END) - main_stack_start_addr;
    code_start_addr = (uint32_t)(&CMB_CODE_SECTION_START);
    code_size = (uint32_t)(&CMB_CODE_SECTION_END) - code_start_addr;
#else
    #error "not supported compiler"
#endif

    if (main_stack_size == 0) {
        cmb_println(print_info[PRINT_MAIN_STACK_CFG_ERROR]);
        return;
    }

    init_ok = true;
}

/**
 * print firmware information, such as: firmware name, hardware version, software version
 */
void cm_backtrace_firmware_info(void) {
    cmb_println(print_info[PRINT_FIRMWARE_INFO], fw_name, hw_ver, sw_ver);
}

#ifdef CMB_USING_OS_PLATFORM
/**
 * Get current thread stack information
 *
 * @param sp stack current pointer
 * @param start_addr stack start address
 * @param size stack size
 */
static void get_cur_thread_stack_info(uint32_t sp, uint32_t *start_addr, size_t *size) {
    CMB_ASSERT(start_addr);
    CMB_ASSERT(size);

#if (CMB_OS_PLATFORM_TYPE == CMB_OS_PLATFORM_RTT)
    *start_addr = (uint32_t) rt_thread_self()->stack_addr;
    *size = rt_thread_self()->stack_size;
#elif (CMB_OS_PLATFORM_TYPE == CMB_OS_PLATFORM_UCOSII)
    extern OS_TCB *OSTCBCur;

    *start_addr = (uint32_t) OSTCBCur->OSTCBStkBottom;
    *size = OSTCBCur->OSTCBStkSize * sizeof(OS_STK);
#elif (CMB_OS_PLATFORM_TYPE == CMB_OS_PLATFORM_UCOSIII)
    extern OS_TCB *OSTCBCurPtr; 
    
    *start_addr = (uint32_t) OSTCBCurPtr->StkBasePtr;
    *size = OSTCBCurPtr->StkSize * sizeof(CPU_STK_SIZE);
#elif (CMB_OS_PLATFORM_TYPE == CMB_OS_PLATFORM_FREERTOS)   
    *start_addr = (uint32_t)vTaskStackAddr();
    *size = vTaskStackSize() * sizeof( StackType_t );
#endif
}

/**
 * Get current thread name
 */
static const char *get_cur_thread_name(void) {
#if (CMB_OS_PLATFORM_TYPE == CMB_OS_PLATFORM_RTT)
    return rt_thread_self()->name;
#elif (CMB_OS_PLATFORM_TYPE == CMB_OS_PLATFORM_UCOSII)
    extern OS_TCB *OSTCBCur;

#if OS_TASK_NAME_SIZE > 0 || OS_TASK_NAME_EN > 0
        return (const char *)OSTCBCur->OSTCBTaskName;
#else
        return NULL;
#endif /* OS_TASK_NAME_SIZE > 0 || OS_TASK_NAME_EN > 0 */

#elif (CMB_OS_PLATFORM_TYPE == CMB_OS_PLATFORM_UCOSIII)
    extern OS_TCB *OSTCBCurPtr; 
    
    return (const char *)OSTCBCurPtr->NamePtr;
#elif (CMB_OS_PLATFORM_TYPE == CMB_OS_PLATFORM_FREERTOS)
    return vTaskName();
#endif
}

#endif /* CMB_USING_OS_PLATFORM */

#ifdef CMB_USING_DUMP_STACK_INFO
/**
 * dump current stack information
 */
static void dump_stack(uint32_t stack_start_addr, size_t stack_size, uint32_t *stack_pointer) {
    if (stack_is_overflow) {
        if (on_thread_before_fault) {
            cmb_println(print_info[PRINT_THREAD_STACK_OVERFLOW], stack_pointer);
        } else {
            cmb_println(print_info[PRINT_MAIN_STACK_OVERFLOW], stack_pointer);
        }
        if ((uint32_t) stack_pointer < stack_start_addr) {
            stack_pointer = (uint32_t *) stack_start_addr;
        } else if ((uint32_t) stack_pointer > stack_start_addr + stack_size) {
            stack_pointer = (uint32_t *) (stack_start_addr + stack_size);
        }
    }
    cmb_println(print_info[PRINT_THREAD_STACK_INFO]);
    for (; (uint32_t) stack_pointer < stack_start_addr + stack_size; stack_pointer++) {
        cmb_println("  addr: %08x    data: %08x", stack_pointer, *stack_pointer);
    }
    cmb_println("====================================");
}
#endif /* CMB_USING_DUMP_STACK_INFO */

/* check the disassembly instruction is 'BL' or 'BLX' */
static bool disassembly_ins_is_bl_blx(uint32_t addr) {
    uint16_t ins1 = *((uint16_t *)addr);
    uint16_t ins2 = *((uint16_t *)(addr + 2));

#define BL_INS_MASK         0xF800
#define BL_INS_HIGH         0xF800
#define BL_INS_LOW          0xF000
#define BLX_INX_MASK        0xFF00
#define BLX_INX             0x4700

    if ((ins2 & BL_INS_MASK) == BL_INS_HIGH && (ins1 & BL_INS_MASK) == BL_INS_LOW) {
        return true;
    } else if ((ins2 & BLX_INX_MASK) == BLX_INX) {
        return true;
    } else {
        return false;
    }
}

/**
 * backtrace function call stack
 *
 * @param buffer call stack buffer
 * @param size buffer size
 * @param sp stack pointer
 *
 * @return depth
 */
size_t cm_backtrace_call_stack(uint32_t *buffer, size_t size, uint32_t sp) {
    uint32_t stack_start_addr = main_stack_start_addr, pc;
    size_t depth = 0, stack_size = main_stack_size;
    bool regs_saved_lr_is_valid = false;

    if (on_fault) {
        if (!stack_is_overflow) {
            /* first depth is PC */
            buffer[depth++] = regs.saved.pc;
            /* fix the LR address in thumb mode */
            pc = regs.saved.lr - 1;
            if ((pc >= code_start_addr) && (pc <= code_start_addr + code_size) && (depth < CMB_CALL_STACK_MAX_DEPTH)
                    && (depth < size)) {
                buffer[depth++] = pc;
                regs_saved_lr_is_valid = true;
            }
        }

#ifdef CMB_USING_OS_PLATFORM
        /* program is running on thread before fault */
        if (on_thread_before_fault) {
            get_cur_thread_stack_info(sp, &stack_start_addr, &stack_size);
        }
    } else {
        /* OS environment */
        if (cmb_get_sp() == cmb_get_psp()) {
            get_cur_thread_stack_info(sp, &stack_start_addr, &stack_size);
        }
#endif /* CMB_USING_OS_PLATFORM */

    }

    if (stack_is_overflow) {
        if (sp < stack_start_addr) {
            sp = stack_start_addr;
        } else if (sp > stack_start_addr + stack_size) {
            sp = stack_start_addr + stack_size;
        }
    }

    /* copy called function address */
    for (; sp < stack_start_addr + stack_size; sp += sizeof(size_t)) {
        /* the *sp value may be LR, so need decrease a word to PC */
        pc = *((uint32_t *) sp) - sizeof(size_t);
        /* the Cortex-M using thumb instruction, so the pc must be an odd number */
        if (pc % 2 == 0) {
            continue;
        }
        /* fix the PC address in thumb mode */
        pc = *((uint32_t *) sp) - 1;
        if ((pc >= code_start_addr + sizeof(size_t)) && (pc <= code_start_addr + code_size) && (depth < CMB_CALL_STACK_MAX_DEPTH)
                /* check the the instruction before PC address is 'BL' or 'BLX' */
                && disassembly_ins_is_bl_blx(pc - sizeof(size_t)) && (depth < size)) {
            /* the second depth function may be already saved, so need ignore repeat */
            if ((depth == 2) && regs_saved_lr_is_valid && (pc == buffer[1])) {
                continue;
            }
            buffer[depth++] = pc;
        }
    }

    return depth;
}

/**
 * dump function call stack
 *
 * @param sp stack pointer
 */
static void print_call_stack(uint32_t sp) {
    size_t i, cur_depth = 0;
    uint32_t call_stack_buf[CMB_CALL_STACK_MAX_DEPTH] = {0};

    cur_depth = cm_backtrace_call_stack(call_stack_buf, CMB_CALL_STACK_MAX_DEPTH, sp);

    for (i = 0; i < cur_depth; i++) {
        sprintf(call_stack_info + i * (8 + 1), "%08lx", (unsigned long)call_stack_buf[i]);
        call_stack_info[i * (8 + 1) + 8] = ' ';
    }

    if (cur_depth) {
        cmb_println(print_info[PRINT_CALL_STACK_INFO], fw_name, CMB_ELF_FILE_EXTENSION_NAME, cur_depth * (8 + 1),
                call_stack_info);
    } else {
        cmb_println(print_info[PRINT_CALL_STACK_ERR]);
    }
}

/**
 * backtrace for assert
 *
 * @param sp the stack pointer when on assert occurred
 */
void cm_backtrace_assert(uint32_t sp) {
    CMB_ASSERT(init_ok);

#ifdef CMB_USING_OS_PLATFORM
    uint32_t cur_stack_pointer = cmb_get_sp();
#endif

    cmb_println("");
    cm_backtrace_firmware_info();

#ifdef CMB_USING_OS_PLATFORM
    /* OS environment */
    if (cur_stack_pointer == cmb_get_msp()) {
        cmb_println(print_info[PRINT_ASSERT_ON_HANDLER]);

#ifdef CMB_USING_DUMP_STACK_INFO
        dump_stack(main_stack_start_addr, main_stack_size, (uint32_t *) sp);
#endif /* CMB_USING_DUMP_STACK_INFO */

    } else if (cur_stack_pointer == cmb_get_psp()) {
        cmb_println(print_info[PRINT_ASSERT_ON_THREAD], get_cur_thread_name());

#ifdef CMB_USING_DUMP_STACK_INFO
        uint32_t stack_start_addr;
        size_t stack_size;
        get_cur_thread_stack_info(sp, &stack_start_addr, &stack_size);
        dump_stack(stack_start_addr, stack_size, (uint32_t *) sp);
#endif /* CMB_USING_DUMP_STACK_INFO */

    }

#else

    /* bare metal(no OS) environment */
#ifdef CMB_USING_DUMP_STACK_INFO
    dump_stack(main_stack_start_addr, main_stack_size, (uint32_t *) sp);
#endif /* CMB_USING_DUMP_STACK_INFO */

#endif /* CMB_USING_OS_PLATFORM */

    print_call_stack(sp);
}

#if (CMB_CPU_PLATFORM_TYPE != CMB_CPU_ARM_CORTEX_M0)
/**
 * fault diagnosis then print cause of fault
 */
static void fault_diagnosis(void) {
    if (regs.hfsr.bits.VECTBL) {
        cmb_println(print_info[PRINT_HFSR_VECTBL]);
    }
    if (regs.hfsr.bits.FORCED) {
        /* Memory Management Fault */
        if (regs.mfsr.value) {
            if (regs.mfsr.bits.IACCVIOL) {
                cmb_println(print_info[PRINT_MFSR_IACCVIOL]);
            }
            if (regs.mfsr.bits.DACCVIOL) {
                cmb_println(print_info[PRINT_MFSR_DACCVIOL]);
            }
            if (regs.mfsr.bits.MUNSTKERR) {
                cmb_println(print_info[PRINT_MFSR_MUNSTKERR]);
            }
            if (regs.mfsr.bits.MSTKERR) {
                cmb_println(print_info[PRINT_MFSR_MSTKERR]);
            }

#if (CMB_CPU_PLATFORM_TYPE == CMB_CPU_ARM_CORTEX_M4) || (CMB_CPU_PLATFORM_TYPE == CMB_CPU_ARM_CORTEX_M7) || \
    (CMB_CPU_PLATFORM_TYPE == CMB_CPU_ARM_CORTEX_M33)
            if (regs.mfsr.bits.MLSPERR) {
                cmb_println(print_info[PRINT_MFSR_MLSPERR]);
            }
#endif

            if (regs.mfsr.bits.MMARVALID) {
                if (regs.mfsr.bits.IACCVIOL || regs.mfsr.bits.DACCVIOL) {
                    cmb_println(print_info[PRINT_MMAR], regs.mmar);
                }
            }
        }
        /* Bus Fault */
        if (regs.bfsr.value) {
            if (regs.bfsr.bits.IBUSERR) {
                cmb_println(print_info[PRINT_BFSR_IBUSERR]);
            }
            if (regs.bfsr.bits.PRECISERR) {
                cmb_println(print_info[PRINT_BFSR_PRECISERR]);
            }
            if (regs.bfsr.bits.IMPREISERR) {
                cmb_println(print_info[PRINT_BFSR_IMPREISERR]);
            }
            if (regs.bfsr.bits.UNSTKERR) {
                cmb_println(print_info[PRINT_BFSR_UNSTKERR]);
            }
            if (regs.bfsr.bits.STKERR) {
                cmb_println(print_info[PRINT_BFSR_STKERR]);
            }

#if (CMB_CPU_PLATFORM_TYPE == CMB_CPU_ARM_CORTEX_M4) || (CMB_CPU_PLATFORM_TYPE == CMB_CPU_ARM_CORTEX_M7) || \
    (CMB_CPU_PLATFORM_TYPE == CMB_CPU_ARM_CORTEX_M33)
            if (regs.bfsr.bits.LSPERR) {
                cmb_println(print_info[PRINT_BFSR_LSPERR]);
            }
#endif

            if (regs.bfsr.bits.BFARVALID) {
                if (regs.bfsr.bits.PRECISERR) {
                    cmb_println(print_info[PRINT_BFAR], regs.bfar);
                }
            }

        }
        /* Usage Fault */
        if (regs.ufsr.value) {
            if (regs.ufsr.bits.UNDEFINSTR) {
                cmb_println(print_info[PRINT_UFSR_UNDEFINSTR]);
            }
            if (regs.ufsr.bits.INVSTATE) {
                cmb_println(print_info[PRINT_UFSR_INVSTATE]);
            }
            if (regs.ufsr.bits.INVPC) {
                cmb_println(print_info[PRINT_UFSR_INVPC]);
            }
            if (regs.ufsr.bits.NOCP) {
                cmb_println(print_info[PRINT_UFSR_NOCP]);
            }
#if (CMB_CPU_PLATFORM_TYPE == CMB_CPU_ARM_CORTEX_M33)
            if (regs.ufsr.bits.STKOF) {
                cmb_println(print_info[PRINT_UFSR_STKOF]);
            }
#endif
            if (regs.ufsr.bits.UNALIGNED) {
                cmb_println(print_info[PRINT_UFSR_UNALIGNED]);
            }
            if (regs.ufsr.bits.DIVBYZERO0) {
                cmb_println(print_info[PRINT_UFSR_DIVBYZERO0]);
            }
        }
    }
    /* Debug Fault */
    if (regs.hfsr.bits.DEBUGEVT) {
        if (regs.dfsr.value) {
            if (regs.dfsr.bits.HALTED) {
                cmb_println(print_info[PRINT_DFSR_HALTED]);
            }
            if (regs.dfsr.bits.BKPT) {
                cmb_println(print_info[PRINT_DFSR_BKPT]);
            }
            if (regs.dfsr.bits.DWTTRAP) {
                cmb_println(print_info[PRINT_DFSR_DWTTRAP]);
            }
            if (regs.dfsr.bits.VCATCH) {
                cmb_println(print_info[PRINT_DFSR_VCATCH]);
            }
            if (regs.dfsr.bits.EXTERNAL) {
                cmb_println(print_info[PRINT_DFSR_EXTERNAL]);
            }
        }
    }
}
#endif /* (CMB_CPU_PLATFORM_TYPE != CMB_CPU_ARM_CORTEX_M0) */

#if (CMB_CPU_PLATFORM_TYPE == CMB_CPU_ARM_CORTEX_M4) || (CMB_CPU_PLATFORM_TYPE == CMB_CPU_ARM_CORTEX_M7) || \
    (CMB_CPU_PLATFORM_TYPE == CMB_CPU_ARM_CORTEX_M33)
static uint32_t statck_del_fpu_regs(uint32_t fault_handler_lr, uint32_t sp) {
    statck_has_fpu_regs = (fault_handler_lr & (1UL << 4)) == 0 ? true : false;

    /* the stack has S0~S15 and FPSCR registers when statck_has_fpu_regs is true, double word align */
    return statck_has_fpu_regs == true ? sp + sizeof(size_t) * 18 : sp;
}
#endif

/**
 * backtrace for fault
 * @note only call once
 *
 * @param fault_handler_lr the LR register value on fault handler
 * @param fault_handler_sp the stack pointer on fault handler
 */
void cm_backtrace_fault(uint32_t fault_handler_lr, uint32_t fault_handler_sp) {
    uint32_t stack_pointer = fault_handler_sp, saved_regs_addr = stack_pointer;
    const char *regs_name[] = { "R0 ", "R1 ", "R2 ", "R3 ", "R12", "LR ", "PC ", "PSR" };

#ifdef CMB_USING_DUMP_STACK_INFO
    uint32_t stack_start_addr = main_stack_start_addr;
    size_t stack_size = main_stack_size;
#endif

    CMB_ASSERT(init_ok);
    /* only call once */
    CMB_ASSERT(!on_fault);

    on_fault = true;

    cmb_println("");
    cm_backtrace_firmware_info();

#ifdef CMB_USING_OS_PLATFORM
    on_thread_before_fault = fault_handler_lr & (1UL << 2);
    /* check which stack was used before (MSP or PSP) */
    if (on_thread_before_fault) {
        cmb_println(print_info[PRINT_FAULT_ON_THREAD], get_cur_thread_name() != NULL ? get_cur_thread_name() : "NO_NAME");
        saved_regs_addr = stack_pointer = cmb_get_psp();

#ifdef CMB_USING_DUMP_STACK_INFO
        get_cur_thread_stack_info(stack_pointer, &stack_start_addr, &stack_size);
#endif /* CMB_USING_DUMP_STACK_INFO */

    } else {
        cmb_println(print_info[PRINT_FAULT_ON_HANDLER]);
    }
#else
    /* bare metal(no OS) environment */
    cmb_println(print_info[PRINT_FAULT_ON_HANDLER]);
#endif /* CMB_USING_OS_PLATFORM */

    /* delete saved R0~R3, R12, LR,PC,xPSR registers space */
    stack_pointer += sizeof(size_t) * 8;

#if (CMB_CPU_PLATFORM_TYPE == CMB_CPU_ARM_CORTEX_M4) || (CMB_CPU_PLATFORM_TYPE == CMB_CPU_ARM_CORTEX_M7) || \
    (CMB_CPU_PLATFORM_TYPE == CMB_CPU_ARM_CORTEX_M33)
    stack_pointer = statck_del_fpu_regs(fault_handler_lr, stack_pointer);
#endif /* (CMB_CPU_PLATFORM_TYPE == CMB_CPU_ARM_CORTEX_M4) || (CMB_CPU_PLATFORM_TYPE == CMB_CPU_ARM_CORTEX_M7) */

#ifdef CMB_USING_DUMP_STACK_INFO
    /* check stack overflow */
    if (stack_pointer < stack_start_addr || stack_pointer > stack_start_addr + stack_size) {
        stack_is_overflow = true;
    }
    /* dump stack information */
    dump_stack(stack_start_addr, stack_size, (uint32_t *) stack_pointer);
#endif /* CMB_USING_DUMP_STACK_INFO */

    /* the stack frame may be get failed when it is overflow  */
    if (!stack_is_overflow) {
        /* dump register */
        cmb_println(print_info[PRINT_REGS_TITLE]);

        regs.saved.r0        = ((uint32_t *)saved_regs_addr)[0];  // Register R0
        regs.saved.r1        = ((uint32_t *)saved_regs_addr)[1];  // Register R1
        regs.saved.r2        = ((uint32_t *)saved_regs_addr)[2];  // Register R2
        regs.saved.r3        = ((uint32_t *)saved_regs_addr)[3];  // Register R3
        regs.saved.r12       = ((uint32_t *)saved_regs_addr)[4];  // Register R12
        regs.saved.lr        = ((uint32_t *)saved_regs_addr)[5];  // Link register LR
        regs.saved.pc        = ((uint32_t *)saved_regs_addr)[6];  // Program counter PC
        regs.saved.psr.value = ((uint32_t *)saved_regs_addr)[7];  // Program status word PSR

        cmb_println("  %s: %08x  %s: %08x  %s: %08x  %s: %08x", regs_name[0], regs.saved.r0,
                                                                regs_name[1], regs.saved.r1,
                                                                regs_name[2], regs.saved.r2,
                                                                regs_name[3], regs.saved.r3);
        cmb_println("  %s: %08x  %s: %08x  %s: %08x  %s: %08x", regs_name[4], regs.saved.r12,
                                                                regs_name[5], regs.saved.lr,
                                                                regs_name[6], regs.saved.pc,
                                                                regs_name[7], regs.saved.psr.value);
        cmb_println("==============================================================");
    }

    /* the Cortex-M0 is not support fault diagnosis */
#if (CMB_CPU_PLATFORM_TYPE != CMB_CPU_ARM_CORTEX_M0)
    regs.syshndctrl.value = CMB_SYSHND_CTRL;  // System Handler Control and State Register
    regs.mfsr.value       = CMB_NVIC_MFSR;    // Memory Fault Status Register
    regs.mmar             = CMB_NVIC_MMAR;    // Memory Management Fault Address Register
    regs.bfsr.value       = CMB_NVIC_BFSR;    // Bus Fault Status Register
    regs.bfar             = CMB_NVIC_BFAR;    // Bus Fault Manage Address Register
    regs.ufsr.value       = CMB_NVIC_UFSR;    // Usage Fault Status Register
    regs.hfsr.value       = CMB_NVIC_HFSR;    // Hard Fault Status Register
    regs.dfsr.value       = CMB_NVIC_DFSR;    // Debug Fault Status Register
    regs.afsr             = CMB_NVIC_AFSR;    // Auxiliary Fault Status Register

    fault_diagnosis();
#endif

    print_call_stack(stack_pointer);
}
