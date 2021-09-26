/*
 * MIT License
 * Copyright (c) 2021 BG5HHP
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
#ifndef __MCU_CFG_H
#define __MCU_CFG_H

#define CFG_SERIAL_RX_BUFFER_SIZE   512

#if defined(STM32F429_439xx)
    #define CFG_SPI1_CLOCK (F_CPU/2)
    #define CFG_SPI2_CLOCK (F_CPU/4)
    #define CFG_SPI3_CLOCK (F_CPU/4)
#endif

// #define CFG_ENABLE_ADC_DMA 0

// #define CFG_ENABLE_RTC_CLKSRC_HSE

#define CFG_ENABLE_BACKTRACE 1

#define RTC_Set RTCx_Set
#endif
