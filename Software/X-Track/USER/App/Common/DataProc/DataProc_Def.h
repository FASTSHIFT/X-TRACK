#ifndef __DATA_PROC_DEF_H
#define __DATA_PROC_DEF_H

#include <stdint.h>

namespace DataProc
{

/* Recorder */
typedef enum
{
    RECORDER_CMD_START,
    RECORDER_CMD_PAUSE,
    RECORDER_CMD_CONTINUE,
    RECORDER_CMD_STOP,
} Recorder_Cmd_t;

typedef struct
{
    Recorder_Cmd_t cmd;
    uint16_t time;
} Recorder_Info_t;

/* Storage */
typedef enum
{
    STORAGE_CMD_LOAD,
    STORAGE_CMD_SAVE,
    STORAGE_CMD_ADD,
    STORAGE_CMD_REMOVE
} Storage_Cmd_t;

typedef enum
{
    STORAGE_TYPE_UNKNOW,
    STORAGE_TYPE_INT,
    STORAGE_TYPE_FLOAT,
    STORAGE_TYPE_DOUBLE,
    STORAGE_TYPE_STRING
} Storage_Type_t;

typedef struct
{
    Storage_Cmd_t cmd;
    const char* key;
    void* value;
    uint16_t size;
    Storage_Type_t type;
} Storage_Info_t;

#define STORAGE_VALUE_REG(act, data, dataType)\
do{\
    DataProc::Storage_Info_t info; \
    info.cmd = DataProc::STORAGE_CMD_ADD; \
    info.key = #data; \
    info.value = &data; \
    info.size = sizeof(data); \
    info.type = dataType; \
    account->Notify("Storage", &info, sizeof(info)); \
}while(0)

/* StatusBar */
typedef struct
{
    bool showLabelRec;
    const char* labelRecStr;
} StatusBar_Info_t;

/* MusicPlayer */
typedef struct
{
    const char* music;
} MusicPlayer_Info_t;

/* SysConfig */
typedef enum
{
    SYSCONFIG_CMD_LOAD,
    SYSCONFIG_CMD_SAVE,
} SysConfig_Cmd_t;

typedef struct
{
    SysConfig_Cmd_t cmd;
} SysConfig_Info_t;

}

#endif
