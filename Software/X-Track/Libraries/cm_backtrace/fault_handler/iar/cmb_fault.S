;/*
; * This file is part of the CmBacktrace Library.
; *
; * Copyright (c) 2016, Armink, <armink.ztl@gmail.com>
; *
; * Permission is hereby granted, free of charge, to any person obtaining
; * a copy of this software and associated documentation files (the
; * 'Software'), to deal in the Software without restriction, including
; * without limitation the rights to use, copy, modify, merge, publish,
; * distribute, sublicense, and/or sell copies of the Software, and to
; * permit persons to whom the Software is furnished to do so, subject to
; * the following conditions:
; *
; * The above copyright notice and this permission notice shall be
; * included in all copies or substantial portions of the Software.
; *
; * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
; * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
; * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
; * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
; * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
; * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
; * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
; *
; * Function: Fault handler by EWARM assembly code
; * Created on: 2016-12-16
; */

    SECTION    .text:CODE(2)
    THUMB
    REQUIRE8
    PRESERVE8

; NOTE: If use this file's HardFault_Handler, please comments the HardFault_Handler code on other file.
    IMPORT cm_backtrace_fault
    EXPORT HardFault_Handler

HardFault_Handler:
    MOV     r0, lr                  ; get lr
    MOV     r1, sp                  ; get stack pointer (current is MSP)
    BL      cm_backtrace_fault

Fault_Loop
    BL      Fault_Loop              ;while(1)

    END
