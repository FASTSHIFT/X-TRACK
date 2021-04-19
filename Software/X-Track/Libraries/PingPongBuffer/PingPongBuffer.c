#include "PingPongBuffer.h"
#include <string.h>

void PingPongBuffer_Init(PingPongBuffer_t* ppbuf, void* buf0, void* buf1)
{
    memset(ppbuf, 0, sizeof(PingPongBuffer_t));
    ppbuf->buffer[0] = buf0;
    ppbuf->buffer[1] = buf1;
}

bool PingPongBuffer_GetReadBuf(PingPongBuffer_t* ppbuf, void** pReadBuf)
{
    if(ppbuf->readAvaliable[0])
    {
        ppbuf->readIndex = 0;
    }
    else if(ppbuf->readAvaliable[1])
    {
        ppbuf->readIndex = 1;
    }
    else
    {
        return false;
    }
    *pReadBuf = ppbuf->buffer[ppbuf->readIndex];
    return true;
}

void PingPongBuffer_SetReadDone(PingPongBuffer_t* ppbuf)
{
    ppbuf->readAvaliable[ppbuf->readIndex] = false;
}

void PingPongBuffer_GetWriteBuf(PingPongBuffer_t* ppbuf, void** pWriteBuf)
{
    if(ppbuf->writeIndex == ppbuf->readIndex)
    {
        ppbuf->writeIndex = !ppbuf->readIndex;
    }
    *pWriteBuf = ppbuf->buffer[ppbuf->writeIndex];
}

void PingPongBuffer_SetWriteDone(PingPongBuffer_t* ppbuf)
{
    ppbuf->readAvaliable[ppbuf->writeIndex] = true;
    ppbuf->writeIndex = !ppbuf->writeIndex;
}
