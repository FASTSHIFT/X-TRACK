#ifndef __MAP_CONV_H
#define __MAP_CONV_H

#include <stdint.h>

class MapConv {
public:
    MapConv();
    ~MapConv() {};

    typedef struct {
        uint32_t tileX;
        uint32_t tileY;
        int16_t subX;
        int16_t subY;
    }MapTile_t;

    void SetLevel(int level);
    void SetRootName(const char* rootName)
    {
        MapKeyRootName = rootName;
    }
    void SetFileName(const char* fileName)
    {
        MapKeyFileName = fileName;
    }
    void SetCalibration(double longitudeCab, double latitudeCab)
    {
        LongitudeCalibration = longitudeCab;
        LatitudeCalibration = latitudeCab;
    }
    int GetMapInfo(
        double longitude, double latitude,
        char* path, uint32_t len,
        uint32_t* mapX, uint32_t* mapY,
        MapTile_t* mapTile
    );

    int GetLevel()
    {
        return MapLevel;
    }
    uint16_t GetMapTileSize()
    {
        return MapTileSize;
    }

    int ConvertMapPath(uint32_t x, uint32_t y, char* path, uint32_t len);
    void ConvertMapCoordinate(
        double longitude, double latitude,
        uint32_t* mapX, uint32_t* mapY
    );
    void ConvertMapTile(uint32_t x, uint32_t y, MapTile_t* mapTile);
    void GetMapTile(double longitude, double latitude, MapTile_t* mapTile);

private: 
    const int32_t EarthRadius;

    const double LongitudeMin;
    const double LongitudeMax;
    double LongitudeCalibration;

    const double LatitudeMin;
    const double LatitudeMax;
    double LatitudeCalibration;

    const int MapLevelMax;
    const int MapLevelMin;
    const uint16_t MapTileSize;

    int MapLevel;
    const char* MapKeyRootName;
    const char* MapKeyFileName;

    uint32_t GetMapSize();
};

#endif
