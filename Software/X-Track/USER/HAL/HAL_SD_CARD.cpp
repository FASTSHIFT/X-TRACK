#include "HAL.h"
#include "Config/Config.h"
#include "SdFat.h"

static SdFat SD(&CONFIG_SD_SPI);

static bool SD_IsReady = false;
static uint32_t SD_CardSize = 0;

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

    Serial.print("SD: init...");
    retval = SD.begin(CONFIG_SD_CS_PIN, SD_SCK_MHZ(30));

    if(retval)
    {
        SD_CardSize = SD.card()->cardSize();
        SdFile::dateTimeCallback(SD_GetDateTime);
        SD_CheckDir(CONFIG_TRACK_RECORD_FILE_DIR_NAME);
        Serial.printf(
            "success, Type: %s, Size: %0.2f GB\r\n",
            SD_GetTypeName(),
            SD_GetCardSizeMB() / 1024.0f
        );
    }
    else
    {
        Serial.printf("failed: 0x%x\r\n", SD.cardErrorCode());
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
#   define CONV_MB(size) (size*0.000512f)
    return CONV_MB(SD_CardSize);
}

const char* HAL::SD_GetTypeName()
{
    const char* type = "Unknown";

    if(!SD_CardSize)
    {
        goto failed;
    }

    switch (SD.card()->type())
    {
    case SD_CARD_TYPE_SD1:
        type = "SD1";
        break;

    case SD_CARD_TYPE_SD2:
        type = "SD2";
        break;

    case SD_CARD_TYPE_SDHC:
        type = (SD_CardSize < 70000000) ? "SDHC" : "SDXC";
        break;

    default:
        break;
    }

failed:
    return type;
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
            SD_CardSize = 0;
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

    CM_VALUE_MONITOR(isInsert, SD_Check(isInsert));
}
