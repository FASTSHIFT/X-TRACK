#include "HAL.h"
#include "TonePlayer/TonePlayer.h"

static TonePlayer player;

/*音符频率对应表*/
typedef enum
{
    L1 = 262,
    L1h = 277,
    L2 = 294,
    L2h = 311,
    L3 = 330,
    L4 = 349,
    L4h = 370,
    L5 = 392,
    L5h = 415,
    L6 = 440,
    L6h = 466,
    L7 = 494,
    M1 = 523,
    M1h = 554,
    M2 = 587,
    M2h = 622,
    M3 = 659,
    M4 = 698,
    M4h = 740,
    M5 = 784,
    M5h = 831,
    M6 = 880,
    M6h = 932,
    M7 = 988,
    H1 = 1046,
    H1h = 1109,
    H2 = 1175,
    H2h = 1245,
    H3 = 1318,
    H4 = 1397,
    H4h = 1480,
    H5 = 1568,
    H5h = 1661,
    H6 = 1760,
    H6h = 1865,
    H7 = 1976,
} Tone_TypeDef;

static void Tone_Callback(uint32_t freq, uint16_t volume)
{
    HAL::Buzz_Tone(freq);
}

#define ASTR_T_TIME 180
#define ASTR_S_TIME 20
static const TonePlayer::MusicNode_t Music_Astronomia[] = 
{
    /*1-1*/
    {M4, ASTR_T_TIME},
    {0, ASTR_S_TIME},
    {M4, ASTR_T_TIME},
    {0, ASTR_S_TIME},
    {M4, ASTR_T_TIME},
    {0, ASTR_S_TIME},
    {M4, ASTR_T_TIME},
    {0, ASTR_S_TIME},
    {M6, ASTR_T_TIME},
    {0, ASTR_S_TIME},
    {M6, ASTR_T_TIME},
    {0, ASTR_S_TIME},
    {M6, ASTR_T_TIME},
    {0, ASTR_S_TIME},
    {M6, ASTR_T_TIME},
    {0, ASTR_S_TIME},
    
    /*1-2*/
    {M5, ASTR_T_TIME},
    {0, ASTR_S_TIME},
    {M5, ASTR_T_TIME},
    {0, ASTR_S_TIME},
    {M5, ASTR_T_TIME},
    {0, ASTR_S_TIME},
    {M5, ASTR_T_TIME},
    {0, ASTR_S_TIME},
    {H1, ASTR_T_TIME},
    {0, ASTR_S_TIME},
    {H1, ASTR_T_TIME},
    {0, ASTR_S_TIME},
    {H1, ASTR_T_TIME},
    {0, ASTR_S_TIME},
    {H1, ASTR_T_TIME},
    {0, ASTR_S_TIME},
    
    /*1-3*/
    {H2, ASTR_T_TIME},
    {0, ASTR_S_TIME},
    {H2, ASTR_T_TIME},
    {0, ASTR_S_TIME},
    {H2, ASTR_T_TIME},
    {0, ASTR_S_TIME},
    {H2, ASTR_T_TIME},
    {0, ASTR_S_TIME},
    {H2, ASTR_T_TIME},
    {0, ASTR_S_TIME},
    {H2, ASTR_T_TIME},
    {0, ASTR_S_TIME},
    {H2, ASTR_T_TIME},
    {0, ASTR_S_TIME},
    {H2, ASTR_T_TIME},
    {0, ASTR_S_TIME},
    
    /*1-4*/
    {M5, ASTR_T_TIME},
    {M4, ASTR_T_TIME},
    {M3, ASTR_T_TIME},
    {M1, ASTR_T_TIME},
    {M2, ASTR_T_TIME},
    {0, ASTR_T_TIME},
    {M2, ASTR_T_TIME},
    {M6, ASTR_T_TIME},
    
    /*2-1*/
    {M5, ASTR_T_TIME},
    {0, ASTR_T_TIME},
    {M4, ASTR_T_TIME},
    {0, ASTR_T_TIME},
    {M3, ASTR_T_TIME},
    {0, ASTR_T_TIME},
    {M3, ASTR_T_TIME},
    {M3, ASTR_T_TIME},
    
    /*2-2*/
    {M5, ASTR_T_TIME},
    {0, ASTR_T_TIME},
    {M4, ASTR_T_TIME},
    {M3, ASTR_T_TIME},
    {M2, ASTR_T_TIME},
    {0, ASTR_T_TIME},
    {M2, ASTR_T_TIME},
    {H4, ASTR_T_TIME},
    
    /*2-3*/
    {H3, ASTR_T_TIME},
    {H4, ASTR_T_TIME},
    {H3, ASTR_T_TIME},
    {H4, ASTR_T_TIME},
    {M2, ASTR_T_TIME},
    {0, ASTR_T_TIME},
    {M2, ASTR_T_TIME},
    {H4, ASTR_T_TIME},
    
    /*2-4*/
    {H3, ASTR_T_TIME},
    {H4, ASTR_T_TIME},
    {H3, ASTR_T_TIME},
    {H4, ASTR_T_TIME},
    {M2, ASTR_T_TIME},
    {0, ASTR_T_TIME},
    {M2, ASTR_T_TIME},
    {M6, ASTR_T_TIME},
    
    /*3-1*/
    {M5, ASTR_T_TIME},
    {0, ASTR_T_TIME},
    {M4, ASTR_T_TIME},
    {0, ASTR_T_TIME},
    {M3, ASTR_T_TIME},
    {0, ASTR_T_TIME},
    {M3, ASTR_T_TIME},
    {M3, ASTR_T_TIME},
    
    /*3-2*/
    {M5, ASTR_T_TIME},
    {0, ASTR_T_TIME},
    {M4, ASTR_T_TIME},
    {M3, ASTR_T_TIME},
    {M2, ASTR_T_TIME},
    {0, ASTR_T_TIME},
    {M2, ASTR_T_TIME},
    {H4, ASTR_T_TIME},
    
    /*3-3*/
    {H3, ASTR_T_TIME},
    {H4, ASTR_T_TIME},
    {H3, ASTR_T_TIME},
    {H4, ASTR_T_TIME},
    {M2, ASTR_T_TIME},
    {0, ASTR_T_TIME},
    {M2, ASTR_T_TIME},
    {H4, ASTR_T_TIME},
    
    /*3-4*/
    {H3, ASTR_T_TIME},
    {H4, ASTR_T_TIME},
    {H3, ASTR_T_TIME},
    {H4, ASTR_T_TIME},
    {M4, ASTR_T_TIME},
    {M4, ASTR_T_TIME},
    {M4, ASTR_T_TIME},
    {M4, ASTR_T_TIME},
    
    /*4-1*/
    {M6, ASTR_T_TIME},
    {M6, ASTR_T_TIME},
    {M6, ASTR_T_TIME},
    {M6, ASTR_T_TIME},
    {M5, ASTR_T_TIME},
    {M5, ASTR_T_TIME},
    {M5, ASTR_T_TIME},
    {M5, ASTR_T_TIME},
    
    /*4-2*/
    {H1, ASTR_T_TIME},
    {H1, ASTR_T_TIME},
    {H1, ASTR_T_TIME},
    {H1, ASTR_T_TIME},
    {H2, ASTR_T_TIME},
    {H2, ASTR_T_TIME},
    {H2, ASTR_T_TIME},
    {H2, ASTR_T_TIME},
    
    /*4-3*/
    {H2, ASTR_T_TIME},
    {H2, ASTR_T_TIME},
    {H2, ASTR_T_TIME},
    {H2, ASTR_T_TIME},
};

/*开机音*/
static const TonePlayer::MusicNode_t Music_StartUp[] =
{
    {M1, 80},
    {M6, 80},
    {M3, 80},
};

/*关机音*/
static const TonePlayer::MusicNode_t Music_Shutdown[] =
{
    {M3, 80},
    {M6, 80},
    {M1, 80},
};

/*错误提示音*/
static const TonePlayer::MusicNode_t Music_Error[] =
{
    {100, 80},
    {0,   80},
    {100, 80},
};

/*连接成功音*/
static const TonePlayer::MusicNode_t Music_Connect[] =
{
    {H1, 80},
    {H2, 80},
    {H3, 80},
};

/*断开连接音*/
static const TonePlayer::MusicNode_t Music_Disconnect[] =
{
    {H3, 80},
    {H2, 80},
    {H1, 80},
};

/*信号不稳定提示音*/
static const TonePlayer::MusicNode_t Music_UnstableConnect[] =
{
    {H1, 80},
    {0, 80},
    {H1, 80},
};

/*充电启动提示音*/
static const TonePlayer::MusicNode_t Music_BattChargeStart[] =
{
    {L1, 80},
    {L3, 80},
};

/*充电关闭提示音*/
static const TonePlayer::MusicNode_t Music_BattChargeEnd[] =
{
    {L3, 80},
    {L1, 80},
};

/*设备插入提示音*/
static const TonePlayer::MusicNode_t Music_DeviceInsert[] =
{
    /*C4,A3,F3,F4*/
    {M1, 180},
    {L6, 80},
    {L4, 80},
    {M4, 160},
};

/*设备拔出提示音*/
static const TonePlayer::MusicNode_t Music_DevicePullout[] =
{
    /*A4,F4,E4*/
    {L6, 80},
    {L4, 80},
    {L3, 80},
};

/*无操作提示音*/
static const TonePlayer::MusicNode_t Music_NoOperationWarning[] = {
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

#define ADD_MUSIC(mc) {Music_##mc,__Sizeof(Music_##mc), #mc}

/*提示音地址储存列表*/
static const MusicList_t MusicList[] =
{
    ADD_MUSIC(StartUp),
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
    ADD_MUSIC(Astronomia)
};

void HAL::Audio_Init()
{
    player.SetCallback(Tone_Callback);
}

void HAL::Audio_Update()
{
    player.Update(millis());
}

bool HAL::Audio_PlayMusic(const char* name)
{
    for(int i = 0; i < __Sizeof(MusicList); i++)
    {
        if(strcmp(name, MusicList[i].name) == 0)
        {
            player.Play(MusicList[i].mc, MusicList[i].length);
            return true;
        }
    }
    return false;
}
