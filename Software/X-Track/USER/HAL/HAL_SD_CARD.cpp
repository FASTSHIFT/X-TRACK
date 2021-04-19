#include "HAL.h"
#include "SdFat.h"

SdFat SD(&SPI_2);

static bool SD_IsReady = false;
static bool SD_LastState = true;

void HAL::SD_Init()
{
#ifdef SD_CD_PIN
    pinMode(SD_CD_PIN, INPUT_PULLUP);
    if(digitalRead(SD_CD_PIN))
    {
        Serial.println("SD: CARD was not insert");
        return;
    }
#endif

    SD_IsReady = SD.begin(SD_CS_PIN, SD_SCK_MHZ(30));

    if(!SD_IsReady)
    {
        Serial.println("SD: CARD ERROR");
    }
}

bool HAL::SD_GetReady()
{
    return SD_IsReady;
}

void HAL::SD_Update()
{
    bool isInsert = (digitalRead(SD_CD_PIN) == LOW);
    
    if(isInsert != SD_LastState)
    {
        if(isInsert)
        {
            SD_Init();
            Audio_PlayMusic("DeviceInsert");
        }
        else
        {
            Audio_PlayMusic("DevicePullout");
        }
        SD_LastState = isInsert;
    }
}
