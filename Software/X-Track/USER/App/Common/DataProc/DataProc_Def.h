#ifndef __DATA_PROC_DEF_H
#define __DATA_PROC_DEF_H

#include <stdint.h>

namespace DataProc
{

/* Recorder */
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

/* Storage */
typedef enum {
    STORAGE_CMD_LOAD,
    STORAGE_CMD_SAVE,
    STORAGE_CMD_ADD,
    STORAGE_CMD_REMOVE
}Storage_Cmd_t;

typedef enum {
    STORAGE_TYPE_UNKNOW,
    STORAGE_TYPE_INT,
    STORAGE_TYPE_FLOAT,
    STORAGE_TYPE_DOUBLE,
    STORAGE_TYPE_STRING
}Storage_Type_t;

typedef struct {
    Storage_Cmd_t cmd;
    const char* key;
    void* value;
    uint16_t size;
    Storage_Type_t type;
}Storage_Info_t;

}


#endif
