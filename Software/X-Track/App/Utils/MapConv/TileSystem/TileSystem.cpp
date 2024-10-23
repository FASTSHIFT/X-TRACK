#include "TileSystem.h"

#include <algorithm>
#include <math.h>
#include <string.h>

using namespace Microsoft_MapPoint;

static const double EarthRadius = 6378137;
static const double MinLatitude = -85.05112878;
static const double MaxLatitude = 85.05112878;
static const double MinLongitude = -180;
static const double MaxLongitude = 180;
static const double MATH_PI = 3.1415926535897932384626433832795;

/// <summary>  
/// Clips a number to the specified minimum and maximum values.  
/// </summary>  
/// <param name="n">The number to clip.</param>  
/// <param name="minValue">Minimum allowable value.</param>  
/// <param name="maxValue">Maximum allowable value.</param>  
/// <returns>The clipped value.</returns>  
static double Clip(double n, double minValue, double maxValue)
{
    return std::min(std::max(n, minValue), maxValue);
}


uint32_t TileSystem::MapSize(int levelOfDetail)
{
    return (uint32_t)256 << levelOfDetail;
}

double TileSystem::GroundResolution(double latitude, int levelOfDetail)
{
    latitude = Clip(latitude, MinLatitude, MaxLatitude);
    return cos(latitude * MATH_PI / 180) * 2 * MATH_PI * EarthRadius / MapSize(levelOfDetail);
}
 
double TileSystem::MapScale(double latitude, int levelOfDetail, int screenDpi)
{
    return GroundResolution(latitude, levelOfDetail) * screenDpi / 0.0254;
}

void TileSystem::LatLongToPixelXY(double latitude, double longitude, int levelOfDetail, int* pixelX, int* pixelY)
{
    latitude = Clip(latitude, MinLatitude, MaxLatitude);
    longitude = Clip(longitude, MinLongitude, MaxLongitude);

    double x = (longitude + 180) / 360;
    double sinLatitude = sin(latitude * MATH_PI / 180);
    double y = 0.5 - log((1 + sinLatitude) / (1 - sinLatitude)) / (4 * MATH_PI);

    uint32_t mapSize = MapSize(levelOfDetail);
    *pixelX = (int)Clip(x * mapSize + 0.5, 0, mapSize - 1);
    *pixelY = (int)Clip(y * mapSize + 0.5, 0, mapSize - 1);
}

void TileSystem::PixelXYToLatLong(int pixelX, int pixelY, int levelOfDetail, double* latitude, double* longitude)
{
    double mapSize = MapSize(levelOfDetail);
    double x = (Clip(pixelX, 0, mapSize - 1) / mapSize) - 0.5;
    double y = 0.5 - (Clip(pixelY, 0, mapSize - 1) / mapSize);

    *latitude = 90 - 360 * atan(exp(-y * 2 * MATH_PI)) / MATH_PI;
    *longitude = 360 * x;
}

void TileSystem::PixelXYToTileXY(int pixelX, int pixelY, int* tileX, int* tileY)
{
    *tileX = pixelX / 256;
    *tileY = pixelY / 256;
}
 
void TileSystem::TileXYToPixelXY(int tileX, int tileY, int* pixelX, int* pixelY)
{
    *pixelX = tileX * 256;
    *pixelY = tileY * 256;
}

void TileSystem::TileXYToQuadKey(int tileX, int tileY, int levelOfDetail, char* quadKeyBuffer, uint32_t len)
{
    uint32_t quadKeyIndex = 0;
    for (int i = levelOfDetail; i > 0; i--)
    {
        char digit = '0';
        int mask = 1 << (i - 1);
        if ((tileX & mask) != 0)
        {
            digit++;
        }
        if ((tileY & mask) != 0)
        {
            digit++;
            digit++;
        }
        quadKeyBuffer[quadKeyIndex] = digit;
        quadKeyIndex++;

        if (quadKeyIndex >= len - 1)
        {
            break;
        }
    }

    quadKeyBuffer[quadKeyIndex] = '\0';
}

void TileSystem::QuadKeyToTileXY(const char* quadKey, int* tileX, int* tileY, int* levelOfDetail)
{
    *tileX = *tileY = 0;
    int len = (int)strlen(quadKey);
    *levelOfDetail = len;
    for (int i = len; i > 0; i--)
    {
        int mask = 1 << (i - 1);
        switch (quadKey[len - i])
        {
        case '0':
            break;

        case '1':
            *tileX |= mask;
            break;

        case '2':
            *tileY |= mask;
            break;

        case '3':
            *tileX |= mask;
            *tileY |= mask;
            break;

        default:
            //throw new ArgumentException("Invalid QuadKey digit sequence.");
            break;
        }
    }
}
