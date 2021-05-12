#ifndef __DATA_PROC_DEF_H
#define __DATA_PROC_DEF_H

#include <stdint.h>

namespace DataProc
{

typedef enum {
    RECORDER_STOP,
    RECORDER_START,
    RECORDER_PAUSE,
}Recorder_State_t;

typedef struct {
    Recorder_State_t state;
    uint16_t time;
    uint32_t pointCnt;
}Recorder_Info_t;

}


#endif
