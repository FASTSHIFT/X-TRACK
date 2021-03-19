#include "MapConv.h"
#include <stdio.h>
#include <math.h>

#ifndef PI
#   define PI 3.1415926535897932384626433832795
#endif

#ifndef constrain
#   define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#endif

MapConv::MapConv()
    : EarthRadius(6378137)
    , LongitudeMin(-180.0)
    , LongitudeMax(180.0)
    , LongitudeCalibration(0.00590)
    , LatitudeMin(-85.05112878)
    , LatitudeMax(85.05112878)
    , LatitudeCalibration(0.0015)
    , MapLevelMax(15)
    , MapLevelMin(3)
    , MapTileSize(256)
{
    MapLevel = 15;
    MapKeyRootName = "MAP/MapInfos";
    MapKeyFileName = "map.bin";
}

int MapConv::GetMapInfo(
    double longitude, double latitude,
    char* path, uint32_t len,
    uint32_t* mapX, uint32_t* mapY,
    MapTile_t* mapTile
)
{
    uint32_t x, y;
    ConvertMapCoordinate(longitude, latitude, &x, &y);

    *mapX = x;
    *mapY = y;

    ConvertMapTile(x, y, mapTile);
    int ret = ConvertMapPath(x, y, path, len);

    return ret;
}

int MapConv::ConvertMapPath(uint32_t x, uint32_t y, char* path, uint32_t len)
{
    uint32_t tileX = x / MapTileSize;
    uint32_t tileY = y / MapTileSize;
    int ret = snprintf(
        path, len,
        "%s/%lX/%lX/%s", MapKeyRootName, tileX, tileY, MapKeyFileName
    );

    return ret;
}

void MapConv::SetLevel(int level)
{
    MapLevel = constrain(level, MapLevelMin, MapLevelMax);
}

uint32_t MapConv::GetMapSize()
{
    uint32_t seed = MapTileSize;
    return seed << MapLevel;
}

void MapConv::GetMapTile(double longitude, double latitude, MapTile_t* mapTile)
{
    uint32_t x, y;
    ConvertMapCoordinate(longitude, latitude, &x, &y);
    ConvertMapTile(x, y, mapTile);   
}

void MapConv::ConvertMapCoordinate(
    double longitude, double latitude,
    uint32_t* mapX, uint32_t* mapY
)
{
    longitude += LongitudeCalibration;
    latitude += LatitudeCalibration;

    longitude = constrain(longitude, LongitudeMin, LongitudeMax);
    latitude = constrain(latitude, LatitudeMin, LatitudeMax);

    double x = (longitude + 180.0) / 360.0;
    double sinLatitude = sin(latitude * PI / 180.0);
    double y = 0.5 - log((1.0 + sinLatitude) / (1.0 - sinLatitude)) / (4.0 * PI);

    uint32_t mapSize = GetMapSize();
    double xSize = x * mapSize + 0.5;
    double ySize = y * mapSize + 0.5;
    mapSize--;
    *mapX = constrain(xSize, 0, mapSize);
    *mapY = constrain(ySize, 0, mapSize);
}

void MapConv::ConvertMapTile(uint32_t x, uint32_t y, MapTile_t* mapTile)
{
    mapTile->tileX = x / MapTileSize;
    mapTile->tileY = y / MapTileSize;
    mapTile->subX = x % MapTileSize;
    mapTile->subY = y % MapTileSize;
}
