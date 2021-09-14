#include "SystemInfosModel.h"
#include <stdio.h>

using namespace Page;

void SystemInfosModel::Init()
{
    account = new Account("SystemInfosModel", DataProc::Center(), 0, this);

    account->Subscribe("SportStatus");
    account->Subscribe("GPS");
    account->Subscribe("MAG");
    account->Subscribe("IMU");
    account->Subscribe("Clock");
    account->Subscribe("Power");
    account->Subscribe("Storage");
}

void SystemInfosModel::Deinit()
{
    if (account)
    {
        delete account;
        account = nullptr;
    }
}

void SystemInfosModel::GetSportInfo(
    float* trip,
    char* time, uint32_t len,
    float* maxSpd
)
{
    HAL::SportStatus_Info_t sport;
    account->Pull("SportStatus", &sport, sizeof(sport));
    *trip = sport.totalDistance / 1000;
    DataProc::MakeTimeString(sport.totalTime, time, len);
    *maxSpd = sport.speedMaxKph;
}

void SystemInfosModel::GetGPSInfo(
    float* lat,
    float* lng,
    float* alt,
    char* utc, uint32_t len,
    float* course,
    float* speed
)
{
    HAL::GPS_Info_t gps;
    account->Pull("GPS", &gps, sizeof(gps));
    *lat = (float)gps.latitude;
    *lng = (float)gps.longitude;
    *alt = gps.altitude;
    snprintf(
        utc, len,
        "%d-%d-%d\n%02d:%02d:%02d",
        gps.clock.year,
        gps.clock.month,
        gps.clock.day,
        gps.clock.hour,
        gps.clock.minute,
        gps.clock.second
    );
    *course = gps.course;
    *speed = gps.speed;
}

void SystemInfosModel::GetMAGInfo(
    float* dir,
    int* x,
    int* y,
    int* z
)
{
    HAL::MAG_Info_t mag;
    memset(&mag, 0, sizeof(mag));
    account->Pull("MAG", &mag, sizeof(mag));

    *dir = 0;
    *x = mag.x;
    *y = mag.y;
    *z = mag.z;
}

void SystemInfosModel::GetIMUInfo(
    int* step,
    char* info, uint32_t len
)
{
    HAL::IMU_Info_t imu;
    memset(&imu, 0, sizeof(imu));
    account->Pull("IMU", &imu, sizeof(imu));
    *step = imu.steps;
    snprintf(
        info,
        len,
        "%d\n%d\n%d\n%d\n%d\n%d",
        imu.ax,
        imu.ay,
        imu.az,
        imu.gx,
        imu.gy,
        imu.gz
    );
}

void SystemInfosModel::GetRTCInfo(
    char* dateTime, uint32_t len
)
{
    HAL::Clock_Info_t clock;
    account->Pull("Clock", &clock, sizeof(clock));
    snprintf(
        dateTime,
        len,
        "%d-%d-%d\n%02d:%02d:%02d",
        clock.year,
        clock.month,
        clock.day,
        clock.hour,
        clock.minute,
        clock.second
    );
}

void SystemInfosModel::GetBatteryInfo(
    int* usage,
    float* voltage,
    char* state, uint32_t len
)
{
    HAL::Power_Info_t power;
    account->Pull("Power", &power, sizeof(power));
    *usage = power.usage;
    *voltage = power.voltage / 1000.0f;
    strncpy(state, power.isCharging ? "CHARGE" : "DISCHARGE", len);
}

void SystemInfosModel::GetStorageInfo(
    bool* detect,
    const char** type,
    char* usage, uint32_t len
)
{
    DataProc::Storage_Basic_Info_t info;
    account->Pull("Storage", &info, sizeof(info));
    *detect = info.isDetect;
    *type = info.type;
    snprintf(
        usage, len,
        "%0.1f GB",
        info.totalSizeMB / 1024.0f
    );
}
