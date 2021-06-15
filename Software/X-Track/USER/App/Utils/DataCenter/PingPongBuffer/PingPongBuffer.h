/*
 * MIT License
 * Copyright (c) 2021 _VIFEXTech
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
#ifndef __PING_PONG_BUFFER_H
#define __PING_PONG_BUFFER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    void* buffer[2];
    volatile uint8_t writeIndex;
    volatile uint8_t readIndex;
    volatile uint8_t readAvaliable[2];
} PingPongBuffer_t;

void PingPongBuffer_Init(PingPongBuffer_t* ppbuf, void* buf0, void* buf1);
bool PingPongBuffer_GetReadBuf(PingPongBuffer_t* ppbuf, void** pReadBuf);
void PingPongBuffer_SetReadDone(PingPongBuffer_t* ppbuf);
void PingPongBuffer_GetWriteBuf(PingPongBuffer_t* ppbuf, void** pWriteBuf);
void PingPongBuffer_SetWriteDone(PingPongBuffer_t* ppbuf);

#ifdef __cplusplus
}
#endif

#endif
