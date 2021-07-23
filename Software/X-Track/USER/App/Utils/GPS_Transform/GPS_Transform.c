#include "GPS_Transform.h"
#include "Config/Config.h"

void GPS_Transform(double wgLat, double wgLon, double* mgLat, double* mgLon)
{
    *mgLat = wgLat + CONFIG_MAP_CONV_CALIBRATION_LAT;
    *mgLon = wgLon + CONFIG_MAP_CONV_CALIBRATION_LNG;
};
