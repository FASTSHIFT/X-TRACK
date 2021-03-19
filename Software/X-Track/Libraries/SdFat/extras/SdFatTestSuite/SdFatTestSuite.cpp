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
#include <SdFatTestSuite.h>
static uint16_t failCount;
static uint16_t testCount;
static Print* testOut = &Serial;
//------------------------------------------------------------------------------
static size_t strlenPGM(PGM_P str) {
  PGM_P end = str;
  while (pgm_read_byte(end++)) {}
  return end - str;
}
//------------------------------------------------------------------------------
void testBegin() {
  Serial.begin(9600);
  while (!Serial) {}  // wait for leonardo
  testOut = &Serial;
  Serial.println(F("Type any character to begin."));
  while (Serial.read() <= 0) {}
  delay(200); // Catch Due reset problem

  testOut->print(F("FreeStack: "));
  testOut->println(FreeStack());
  testOut->println();
  failCount = 0;
  testCount = 0;
}
//------------------------------------------------------------------------------
void testEnd() {
  testOut->println();
  testOut->println(F("Compiled: " __DATE__ " " __TIME__));
  testOut->print(F("FreeStack: "));
  testOut->println(FreeStack());
  testOut->print(F("Test count: "));
  testOut->println(testCount);
  testOut->print(F("Fail count: "));
  testOut->println(failCount);
}
//------------------------------------------------------------------------------
static void testResult(bool b, uint8_t n) {
 while (n++ < 60) testOut->write(' ');
  if (b) {
    testOut->println(F("..ok"));
  } else {
    testOut->println(F("FAIL"));
    failCount++;
  }
  testCount++;
}
//------------------------------------------------------------------------------
void testVerify_P(char* result, PGM_P expect) {
  testOut->write('"');
  testOut->print(result);
  testOut->print("\",\"");
  testOut->print((const __FlashStringHelper*)expect);
  testOut->write('"');
  uint8_t n = strlen(result) + strlenPGM(expect) + 5;
  testResult(!strcmp_P(result, expect), n);
}
//------------------------------------------------------------------------------
void testVerify_P(bool b, PGM_P msg) {
  testOut->print((const __FlashStringHelper*)msg);
  uint8_t n = strlenPGM(msg);
  testResult(b, n);
}
