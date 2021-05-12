#include "HAL.h"
#include "SdFat.h"

static SdFat SD(&SPI_2);

static bool SD_IsReady = false;

bool HAL::SD_Init()
{
    bool retval = true;

    pinMode(CONFIG_SD_CD_PIN, INPUT_PULLUP);
    if(digitalRead(CONFIG_SD_CD_PIN))
    {
        Serial.println("SD: CARD was not insert");
        retval = false;
    }

    retval = SD.begin(CONFIG_SD_CS_PIN, SD_SCK_MHZ(30));

    if(!retval)
    {
        Serial.println("SD: CARD ERROR");
    }
    
    SD_IsReady = retval;
    
    return retval;
}

bool HAL::SD_GetReady()
{
    return SD_IsReady;
}

static void SD_Check(bool isInsert)
{
    if(isInsert)
    {
        bool ret = HAL::SD_Init();
        HAL::Audio_PlayMusic(ret ? "DeviceInsert" : "Error");
    }
    else
    {
        HAL::Audio_PlayMusic("DevicePullout");
    }
}

void HAL::SD_Update()
{
    bool isInsert = (digitalRead(CONFIG_SD_CD_PIN) == LOW);
    __ValueMonitor(isInsert, SD_Check(isInsert));
}
