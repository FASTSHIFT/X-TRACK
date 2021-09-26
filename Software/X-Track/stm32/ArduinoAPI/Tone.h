/*
 * MIT License
 * Copyright (c) 2019 _VIFEXTech
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
#ifndef __TONE_H
#define __TONE_H

#include "Arduino.h"
#include "timer.h"

#define ToneVolumn_Default 80
#define ToneTimer_Default TIM1

void toneSetTimer(TIM_TypeDef* TIMx);
void tone(uint8_t Pin, uint32_t freq, uint32_t Time_ms);
void tone(uint8_t Pin, uint32_t freq);
void noTone(uint8_t Pin);

void toneBlock(uint8_t Pin, uint32_t freq, uint32_t Time_ms);
void toneBlock_Volumn(uint8_t Pin, uint32_t freq, uint32_t Time_ms, uint32_t vol);
void toneBlock_Volumn_us(uint8_t Pin, uint32_t freq, uint32_t Time_us, uint32_t vol);

#endif
