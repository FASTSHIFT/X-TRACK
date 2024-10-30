/*
 * MIT License
 * Copyright (c) 2024 _VIFEXTech
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "Geo.h"
#include <cfloat>
#include <cmath>

/* clang-format off */

#define TWO_PI (2.0 * M_PI)
#define DEG_TO_RAD 0.017453292519943295769236907684886
#define RAD_TO_DEG 57.295779513082320876798154814105
#define RADIANS(deg) ((deg) * DEG_TO_RAD)
#define DEGREES(rad) ((rad) * RAD_TO_DEG)
#define SQ(x) ((x) * (x))

/* clang-format on */

static inline bool math_zero(float a)
{
    return (fabsf(a) < FLT_EPSILON);
}

double Geo::distanceBetween(double lat1, double long1, double lat2, double long2)
{
    /* returns distance in meters between two positions, both specified
     * as signed decimal-DEGREES latitude and longitude. Uses great-circle
     * distance computation for hypothetical sphere of radius 6372795 meters.
     * Because Earth is no exact sphere, rounding errors may be up to 0.5%.
     * Courtesy of Maarten Lamers
     */
    double delta = RADIANS(long1 - long2);
    double sdlong = sin(delta);
    double cdlong = cos(delta);
    lat1 = RADIANS(lat1);
    lat2 = RADIANS(lat2);
    double slat1 = sin(lat1);
    double clat1 = cos(lat1);
    double slat2 = sin(lat2);
    double clat2 = cos(lat2);
    delta = (clat1 * slat2) - (slat1 * clat2 * cdlong);
    delta = SQ(delta);
    delta += SQ(clat2 * sdlong);
    delta = sqrt(delta);
    double denom = (slat1 * slat2) + (clat1 * clat2 * cdlong);
    delta = atan2(delta, denom);
    return delta * 6372795;
}

double Geo::courseTo(double lat1, double long1, double lat2, double long2)
{
    /* returns course in DEGREES (North=0, West=270) from position 1 to position 2,
     * both specified as signed decimal-DEGREES latitude and longitude.
     * Because Earth is no exact sphere, calculated course may be off by a tiny fraction.
     * Courtesy of Maarten Lamers
     */
    double dlon = RADIANS(long2 - long1);
    lat1 = RADIANS(lat1);
    lat2 = RADIANS(lat2);
    double a1 = sin(dlon) * cos(lat2);
    double a2 = sin(lat1) * cos(lat2) * cos(dlon);
    a2 = cos(lat1) * sin(lat2) - a2;
    a2 = atan2(a1, a2);
    if (a2 < 0.0) {
        a2 += TWO_PI;
    }

    return DEGREES(a2);
}

const char* Geo::cardinal(float course)
{
    static const char* directions[] = { "N", "NNE", "NE", "ENE", "E", "ESE", "SE", "SSE", "S", "SSW", "SW", "WSW", "W", "WNW", "NW", "NNW" };
    int direction = (int)((course + 11.25f) / 22.5f);
    return directions[direction % 16];
}

float Geo::grade(float height, float distance)
{
    if (math_zero(distance)) {
        return 0;
    }

    return (height / distance) * 100.0f;
}

float Geo::slope(float height, float distance)
{
    return DEGREES(atan2f(height, distance));
}

float Geo::sealevel(float pressure, float altitude)
{
    return (pressure / powf(1 - (altitude / 44330.0f), 5.255f));
}

float Geo::pressureToAltitude(float pressure, float sealevel)
{
    return (44330.0f * (1 - powf(pressure / sealevel, 1 / 5.255f)));
}
