/**
 * Copyright (c) 20011-2017 Bill Greiman
 * This file is part of the SdFat library for SD memory cards.
 *
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
#ifndef SdFatTestSuite_h
#define SdFatTestSuite_h
#include "SdFat.h"
#include "FreeStack.h"

#if defined(__arm__) && !defined(strcmp_P)
#define strcmp_P(a, b) strcmp((a), (b))
#endif  // strcmp_P

#if defined(__arm__) && !defined(strncpy_P)
#define strncpy_P(s, t, n) strncpy(s, t, n)
#endif  // strncpy_P

#if defined(__arm__) && !defined(strlen_P)
#define strlen_P(str) strlen(str)
#endif  // strlen_P

#define testVerifyBool(result) testVerify_P(result, PSTR(#result))
#define testVerifyMsg(result, msg) testVerify_P(result, PSTR(msg))
#define testVerifyStr(result, expect) testVerify_P(result, PSTR(expect))

void testBegin();
void testEnd();
void testVerify_P(bool b, PGM_P msg);
void testVerify_P(char* result, PGM_P expect);
#endif  // SdFatTestSuite_h
