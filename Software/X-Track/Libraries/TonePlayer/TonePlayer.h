#ifndef __TONE_PLAYER_H
#define __TONE_PLAYER_H

#include <stdint.h>

class TonePlayer
{
    typedef void(*CallbackFunction_t)(uint32_t, uint16_t);
public:
    typedef struct
    {
        uint16_t Freq;
        uint16_t Time;
        uint16_t Volume;
    } MusicNode_t;
    
    enum Speed{
        SPEED_HALF = 512,
        SPEED_NORMAL = 256,
        SPEED_DOUBLE = 128
    };

    TonePlayer();
    ~TonePlayer() {}
    void SetMusic(const MusicNode_t* music, uint16_t length);
    void SetCallback(CallbackFunction_t func)
    {
        CallbackFunction = func;
    }
    void Play();
    void Play(const MusicNode_t* music, uint16_t length)
    {
        SetMusic(music, length);
        Play();
    }
    void Stop();
    void SetSpeed(uint16_t speed);
    bool Update(uint32_t tick);
private:
    uint16_t Length;
    uint16_t CurrentPos;
    uint32_t NextTime;
    uint16_t Speed;
    const MusicNode_t* CurrentMusic;
    CallbackFunction_t CallbackFunction;
};

#endif
