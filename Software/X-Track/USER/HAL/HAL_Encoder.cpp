#include "HAL.h"
#include "ButtonEvent/ButtonEvent.h"

static ButtonEvent EncoderPush(CONFIG_POWER_SHUTDOWM_DELAY);

static bool EncoderEnable = true;
static volatile int32_t EncoderDiff = 0;
static bool EncoderDiffDisable = false;

static void Buzz_Handler(int dir)
{
    static const uint16_t freqStart = 2000;
    static uint16_t freq = freqStart;
    static uint32_t lastRotateTime;

    if(millis() - lastRotateTime > 1000)
    {
        freq = freqStart;
    }
    else
    {
        if(dir > 0)
        {
            freq += 100;
        }

        if(dir < 0)
        {
            freq -= 100;
        }

        freq = constrain(freq, 100, 20 * 1000);
    }

    lastRotateTime = millis();
    HAL::Buzz_Tone(freq, 5);
}

static void Encoder_EventHandler()
{
    if(!EncoderEnable || EncoderDiffDisable)
    {
        return;
    }

    int dir = (digitalRead(CONFIG_ENCODER_B_PIN) == LOW ? -1 : +1);
    EncoderDiff += dir;
    Buzz_Handler(dir);
}

static void Encoder_PushHandler(ButtonEvent* btn, int event)
{
    if(event == ButtonEvent::EVENT_PRESSED)
    {
        EncoderDiffDisable = true;
    }
    else if(event == ButtonEvent::EVENT_RELEASED)
    {
        EncoderDiffDisable = false;
    }
    else if(event == ButtonEvent::EVENT_LONG_PRESSED)
    {
        HAL::Power_Shutdown();
        HAL::Audio_PlayMusic("Shutdown");
    }
}

void HAL::Encoder_Init()
{
    pinMode(CONFIG_ENCODER_A_PIN, INPUT_PULLUP);
    pinMode(CONFIG_ENCODER_B_PIN, INPUT_PULLUP);
    pinMode(CONFIG_ENCODER_PUSH_PIN, INPUT_PULLUP);

    attachInterrupt(CONFIG_ENCODER_A_PIN, Encoder_EventHandler, FALLING);

    EncoderPush.EventAttach(Encoder_PushHandler);
}

void HAL::Encoder_Update()
{
    EncoderPush.EventMonitor(Encoder_GetIsPush());
}

int32_t HAL::Encoder_GetDiff()
{
    int32_t diff = EncoderDiff;
    EncoderDiff = 0;
    return diff;
}

bool HAL::Encoder_GetIsPush()
{
    if(!EncoderEnable)
    {
        return false;
    }
    
    return (digitalRead(CONFIG_ENCODER_PUSH_PIN) == LOW);
}

void HAL::Encoder_SetEnable(bool en)
{
    EncoderEnable = en;
}
