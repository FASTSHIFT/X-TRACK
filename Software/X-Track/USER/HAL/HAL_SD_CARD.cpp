#include "HAL.h"
#include "Config/Config.h"
#include "SdFat.h"

static SdFat SD(&CONFIG_SD_SPI);

static bool SD_IsReady = false;
static float SD_CardSizeMB = 0;

static HAL::SD_CallbackFunction_t SD_EventCallback = nullptr;

/*
 * User provided date time callback function.
 * See SdFile::dateTimeCallback() for usage.
 */
static void SD_GetDateTime(uint16_t* date, uint16_t* time)
{
    // User gets date and time from GPS or real-time
    // clock in real callback function
    HAL::Clock_Info_t clock;
    HAL::Clock_GetInfo(&clock);

    // return date using FAT_DATE macro to format fields
    *date = FAT_DATE(clock.year, clock.month, clock.day);

    // return time using FAT_TIME macro to format fields
    *time = FAT_TIME(clock.hour, clock.minute, clock.second);
}

static bool SD_CheckDir(const char* path)
{
    bool retval = true;
    if(!SD.exists(path))
    {
        Serial.printf("SD: Auto create path \"%s\"...", path);
        retval = SD.mkdir(path);
        Serial.println(retval ? "success" : "failed");
    }
    return retval;
}

bool HAL::SD_Init()
{
    bool retval = true;

    pinMode(CONFIG_SD_CD_PIN, INPUT_PULLUP);
    if(digitalRead(CONFIG_SD_CD_PIN))
    {
        Serial.println("SD: CARD was not inserted");
        retval = false;
    }

    retval = SD.begin(CONFIG_SD_CS_PIN, SD_SCK_MHZ(30));

    if(retval)
    {
#define CONV_MB(size) (size*0.000512f)
        SD_CardSizeMB = CONV_MB(SD.card()->cardSize());
        SdFile::dateTimeCallback(SD_GetDateTime);
        SD_CheckDir(CONFIG_TRACK_RECORD_FILE_DIR_NAME);
        Serial.println("SD: Init success");
    }
    else
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

float HAL::SD_GetCardSizeMB()
{
    return SD_CardSizeMB;
}

static void SD_Check(bool isInsert)
{
    if(isInsert)
    {
        bool ret = HAL::SD_Init();

        if(ret && SD_EventCallback)
        {
            SD_EventCallback(true);
        }

        HAL::Audio_PlayMusic(ret ? "DeviceInsert" : "Error");
    }
    else
    {
        SD_IsReady = false;

        if(SD_EventCallback)
        {
            SD_EventCallback(false);
        }

        HAL::Audio_PlayMusic("DevicePullout");
    }
}

void HAL::SD_SetEventCallback(SD_CallbackFunction_t callback)
{
    SD_EventCallback = callback;
}

void HAL::SD_Update()
{
    bool isInsert = (digitalRead(CONFIG_SD_CD_PIN) == LOW);
    __ValueMonitor(isInsert, SD_Check(isInsert));
}
