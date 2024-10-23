#include "ToneMap.h"

#define MUSIC_DEF(name) static const TonePlayer::MusicNode_t Music_##name[] =

/*开机音*/
MUSIC_DEF(Startup)
{
    {M1, 80},
    {M6, 80},
    {M3, 80},
};

/*关机音*/
MUSIC_DEF(Shutdown)
{
    {M3, 80},
    {M6, 80},
    {M1, 80},
};

/*错误提示音*/
MUSIC_DEF(Error)
{
    {100, 80},
    {0,   80},
    {100, 80},
};

/*连接成功音*/
MUSIC_DEF(Connect)
{
    {H1, 80},
    {H2, 80},
    {H3, 80},
};

/*断开连接音*/
MUSIC_DEF(Disconnect)
{
    {H3, 80},
    {H2, 80},
    {H1, 80},
};

/*信号不稳定提示音*/
MUSIC_DEF(UnstableConnect)
{
    {H1, 80},
    {0, 80},
    {H1, 80},
};

/*充电启动提示音*/
MUSIC_DEF(BattChargeStart)
{
    {L1, 80},
    {L3, 80},
};

/*充电关闭提示音*/
MUSIC_DEF(BattChargeEnd)
{
    {L3, 80},
    {L1, 80},
};

/*设备插入提示音*/
MUSIC_DEF(DeviceInsert)
{
    /*C4,A3,F3,F4*/
    {M1, 180},
    {L6, 80},
    {L4, 80},
    {M4, 160},
};

/*设备拔出提示音*/
MUSIC_DEF(DevicePullout)
{
    /*A4,F4,E4*/
    {L6, 80},
    {L4, 80},
    {L3, 80},
};

/*无操作提示音*/
MUSIC_DEF(NoOperationWarning) 
{
    {4000, 40},
    {0, 80},
    {4000, 40},
    {0, 80},
    {4000, 40},
};

/*列表数据类型定义*/
typedef struct
{
    const TonePlayer::MusicNode_t* mc;
    uint16_t length;
    const char* name;
} MusicList_t;

#define ADD_MUSIC(mc) {Music_##mc,(sizeof(Music_##mc) / sizeof(Music_##mc[0])), #mc}

/*提示音列表*/
static const MusicList_t MusicList[] =
{
    ADD_MUSIC(Startup),
    ADD_MUSIC(Shutdown),
    ADD_MUSIC(Error),
    ADD_MUSIC(Connect),
    ADD_MUSIC(Disconnect),
    ADD_MUSIC(UnstableConnect),
    ADD_MUSIC(BattChargeStart),
    ADD_MUSIC(BattChargeEnd),
    ADD_MUSIC(DeviceInsert),
    ADD_MUSIC(DevicePullout),
    ADD_MUSIC(NoOperationWarning),
};
