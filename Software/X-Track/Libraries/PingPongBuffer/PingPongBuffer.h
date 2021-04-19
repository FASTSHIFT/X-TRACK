#ifndef __PING_PONG_BUFFER_H
#define __PING_PONG_BUFFER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

typedef struct {
    void* buffer[2];
    bool writeIndex;
    bool readIndex;
    bool readAvaliable[2];
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
