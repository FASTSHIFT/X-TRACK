/*
 * This file is part of the CmBacktrace Library.
 *
 * Copyright (c) 2016, Armink, <armink.ztl@gmail.com>
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
 * Function: It is an head file for this library. You can see all be called functions.
 * Created on: 2016-12-15
 */

#ifndef _CORTEXM_BACKTRACE_H_
#define _CORTEXM_BACKTRACE_H_

#ifdef __cplusplus
extern "C" {
#endif
    
#include "cmb_def.h"

void cm_backtrace_init(const char *firmware_name, const char *hardware_ver, const char *software_ver);
void cm_backtrace_firmware_info(void);
size_t cm_backtrace_call_stack(uint32_t *buffer, size_t size, uint32_t sp);
void cm_backtrace_assert(uint32_t sp);
void cm_backtrace_fault(uint32_t fault_handler_lr, uint32_t fault_handler_sp);

#ifdef __cplusplus
}
#endif

#endif /* _CORTEXM_BACKTRACE_H_ */
