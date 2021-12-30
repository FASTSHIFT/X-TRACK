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
#ifndef __HardwareSerial_h
#define __HardwareSerial_h

#include "Arduino.h"
#include "Print.h"
#include "Stream.h"

typedef enum
{
    SERIAL_7N1,
    SERIAL_7N2,
    SERIAL_7E1,
    SERIAL_7E2,
    SERIAL_7O1,
    SERIAL_7O2,
    SERIAL_7N0_5,
    SERIAL_7N1_5,
    SERIAL_7E0_5,
    SERIAL_7E1_5,
    SERIAL_7O0_5,
    SERIAL_7O1_5,

    SERIAL_8N1,
    SERIAL_8N2,
    SERIAL_8E1,
    SERIAL_8E2,
    SERIAL_8O1,
    SERIAL_8O2,
    SERIAL_8N0_5,
    SERIAL_8N1_5,
    SERIAL_8E0_5,
    SERIAL_8E1_5,
    SERIAL_8O0_5,
    SERIAL_8O1_5,

    SERIAL_9N1,
    SERIAL_9N2,
    SERIAL_9E1,
    SERIAL_9E2,
    SERIAL_9O1,
    SERIAL_9O2,
    SERIAL_9N0_5,
    SERIAL_9N1_5,
    SERIAL_9E0_5,
    SERIAL_9E1_5,
    SERIAL_9O0_5,
    SERIAL_9O1_5,
} SERIAL_Config_t;

class HardwareSerial : public Stream
{
    typedef void(*CallbackFunction_t)(HardwareSerial* serial);

public:
    HardwareSerial(usart_type* usart);

    usart_type* getUSART()
    {
        return _USARTx;
    }

    void begin(
        uint32_t baudRate,
        SERIAL_Config_t config = SERIAL_CONFIG_DEFAULT,
        uint8_t preemptionPriority = SERIAL_PREEMPTIONPRIORITY_DEFAULT,
        uint8_t subPriority = SERIAL_SUBPRIORITY_DEFAULT
    );
    void end(void);
    void attachInterrupt(CallbackFunction_t func);
    virtual int available(void);
    virtual int peek(void);
    virtual int read(void);
    virtual void flush(void);

    virtual size_t write(uint8_t n);
    inline size_t write(unsigned long n)
    {
        return write((uint8_t)n);
    }
    inline size_t write(long n)
    {
        return write((uint8_t)n);
    }
    inline size_t write(unsigned int n)
    {
        return write((uint8_t)n);
    }
    inline size_t write(int n)
    {
        return write((uint8_t)n);
    }
    using Print::write; // pull in write(str) and write(buf, size) from Print
    operator bool()
    {
        return true;
    }

    void IRQHandler();

private:
    usart_type* _USARTx;
    CallbackFunction_t _callbackFunction;
    volatile uint16_t _rxBufferHead;
    volatile uint16_t _rxBufferTail;
    uint8_t _rxBuffer[SERIAL_RX_BUFFER_SIZE];
};

#if SERIAL_1_ENABLE
extern HardwareSerial Serial;
#endif

#if SERIAL_2_ENABLE
extern HardwareSerial Serial2;
#endif

#if SERIAL_3_ENABLE
extern HardwareSerial Serial3;
#endif

#endif
