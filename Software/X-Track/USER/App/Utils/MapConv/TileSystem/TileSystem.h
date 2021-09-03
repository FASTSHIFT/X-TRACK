//------------------------------------------------------------------------------  
// <copyright company="Microsoft">  
//     Copyright (c) 2006-2009 Microsoft Corporation.  All rights reserved.  
// </copyright>  
//------------------------------------------------------------------------------ 

// Reference Link: https://docs.microsoft.com/en-us/bingmaps/articles/bing-maps-tile-system?redirectedfrom=MSDN

#ifndef __MICROSOFT_MAP_POINT_TILE_SYSTEM_H
#define __MICROSOFT_MAP_POINT_TILE_SYSTEM_H

#include <stdint.h>

namespace Microsoft_MapPoint
{

namespace TileSystem
{

/// <summary>  
/// Determines the map width and height (in pixels) at a specified level  
/// of detail.  
/// </summary>  
/// <param name="levelOfDetail">Level of detail, from 1 (lowest detail)  
/// to 23 (highest detail).</param>  
/// <returns>The map width and height in pixels.</returns>  
uint32_t MapSize(int levelOfDetail);

/// <summary>  
/// Determines the ground resolution (in meters per pixel) at a specified  
/// latitude and level of detail.  
/// </summary>  
/// <param name="latitude">Latitude (in degrees) at which to measure the  
/// ground resolution.</param>  
/// <param name="levelOfDetail">Level of detail, from 1 (lowest detail)  
/// to 23 (highest detail).</param>  
/// <returns>The ground resolution, in meters per pixel.</returns>  
double GroundResolution(double latitude, int levelOfDetail);

/// <summary>  
/// Determines the map scale at a specified latitude, level of detail,  
/// and screen resolution.  
/// </summary>  
/// <param name="latitude">Latitude (in degrees) at which to measure the  
/// map scale.</param>  
/// <param name="levelOfDetail">Level of detail, from 1 (lowest detail)  
/// to 23 (highest detail).</param>  
/// <param name="screenDpi">Resolution of the screen, in dots per inch.</param>  
/// <returns>The map scale, expressed as the denominator N of the ratio 1 : N.</returns>  
double MapScale(double latitude, int levelOfDetail, int screenDpi);

/// <summary>  
/// Converts a point from latitude/longitude WGS-84 coordinates (in degrees)  
/// into pixel XY coordinates at a specified level of detail.  
/// </summary>  
/// <param name="latitude">Latitude of the point, in degrees.</param>  
/// <param name="longitude">Longitude of the point, in degrees.</param>  
/// <param name="levelOfDetail">Level of detail, from 1 (lowest detail)  
/// to 23 (highest detail).</param>  
/// <param name="pixelX">Output parameter receiving the X coordinate in pixels.</param>  
/// <param name="pixelY">Output parameter receiving the Y coordinate in pixels.</param>  
void LatLongToPixelXY(double latitude, double longitude, int levelOfDetail, int* pixelX, int* pixelY);

/// <summary>  
/// Converts a pixel from pixel XY coordinates at a specified level of detail  
/// into latitude/longitude WGS-84 coordinates (in degrees).  
/// </summary>  
/// <param name="pixelX">X coordinate of the point, in pixels.</param>  
/// <param name="pixelY">Y coordinates of the point, in pixels.</param>  
/// <param name="levelOfDetail">Level of detail, from 1 (lowest detail)  
/// to 23 (highest detail).</param>  
/// <param name="latitude">Output parameter receiving the latitude in degrees.</param>  
/// <param name="longitude">Output parameter receiving the longitude in degrees.</param>  
void PixelXYToLatLong(int pixelX, int pixelY, int levelOfDetail, double* latitude, double* longitude);

/// <summary>  
/// Converts pixel XY coordinates into tile XY coordinates of the tile containing  
/// the specified pixel.  
/// </summary>  
/// <param name="pixelX">Pixel X coordinate.</param>  
/// <param name="pixelY">Pixel Y coordinate.</param>  
/// <param name="tileX">Output parameter receiving the tile X coordinate.</param>  
/// <param name="tileY">Output parameter receiving the tile Y coordinate.</param>  
void PixelXYToTileXY(int pixelX, int pixelY, int* tileX, int* tileY);

/// <summary>  
/// Converts tile XY coordinates into pixel XY coordinates of the upper-left pixel  
/// of the specified tile.  
/// </summary>  
/// <param name="tileX">Tile X coordinate.</param>  
/// <param name="tileY">Tile Y coordinate.</param>  
/// <param name="pixelX">Output parameter receiving the pixel X coordinate.</param>  
/// <param name="pixelY">Output parameter receiving the pixel Y coordinate.</param>  
void TileXYToPixelXY(int tileX, int tileY, int* pixelX, int* pixelY);

/// <summary>  
/// Converts tile XY coordinates into a QuadKey at a specified level of detail.  
/// </summary>  
/// <param name="tileX">Tile X coordinate.</param>  
/// <param name="tileY">Tile Y coordinate.</param>  
/// <param name="levelOfDetail">Level of detail, from 1 (lowest detail)  
/// to 23 (highest detail).</param>  
/// <returns>A string containing the QuadKey.</returns>  
void TileXYToQuadKey(int tileX, int tileY, int levelOfDetail, char* quadKeyBuffer, uint32_t len);

/// <summary>  
/// Converts a QuadKey into tile XY coordinates.  
/// </summary>  
/// <param name="quadKey">QuadKey of the tile.</param>  
/// <param name="tileX">Output parameter receiving the tile X coordinate.</param>  
/// <param name="tileY">Output parameter receiving the tile Y coordinate.</param>  
/// <param name="levelOfDetail">Output parameter receiving the level of detail.</param>  
void QuadKeyToTileXY(const char* quadKey, int* tileX, int* tileY, int* levelOfDetail);
}

}

#endif
