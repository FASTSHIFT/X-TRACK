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
#ifndef __GEO_H
#define __GEO_H

namespace Geo {

/**
 * @brief Calculate the distance between two points on the Earth's surface.
 * @param lat1 Latitude of the first point in degrees.
 * @param long1 Longitude of the first point in degrees.
 * @param lat2 Latitude of the second point in degrees.
 * @param long2 Longitude of the second point in degrees.
 * @return Distance between the two points in meters.
 */
double distanceBetween(double lat1, double long1, double lat2, double long2);

/**
 * @brief Calculate the course to reach the second point from the first point.
 * @param lat1 Latitude of the first point in degrees.
 * @param long1 Longitude of the first point in degrees.
 * @param lat2 Latitude of the second point in degrees.
 * @param long2 Longitude of the second point in degrees.
 * @return Course to reach the second point from the first point in degrees.
 */
double courseTo(double lat1, double long1, double lat2, double long2);

/**
 * @brief Calculate the cardinal direction of the course.
 * @param course Course in degrees.
 * @return Cardinal direction of the course.
 */
const char* cardinal(float course);

/**
 * @brief Calculate the grade of the slope.
 * @param height Height of the slope in meters.
 * @param distance Distance between the two points on the slope in meters.
 * @return Grade of the slope in percent.
 * @note The formula used is: grade = (height / distance) * 100.
 * @note If the distance is zero, the function returns zero.
*/
float grade(float height, float distance);

/**
 * @brief Calculate the slope of the line.
 * @param height Height of the slope in meters.
 * @param distance Distance between the two points on the slope in meters.
 * @return Slope of the line in degrees.
 * @note The formula used is: slope = atan2f(height, distance) * 180 / pi.
 * @note If the distance is zero, the function returns zero.
 */
float slope(float height, float distance);

/**
 * @brief Given a pressure (mb) taken at a specific altitude (meters),
 *        calculate the equivalent pressure (mb) at sea level.
 *        This produces pressure readings that can be used for weather measurements.
 * @param pressure Pressure (mb) taken at a specific altitude (meters).
 * @param altitude Altitude (meters) at which the pressure was taken.
 * @return Equivalent pressure (mb) at sea level.
 */
float sealevel(float pressure, float altitude);

/**
 * @brief Given a pressure (mb) and the pressure at sea level (mb),
 *        return altitude (meters) above sea level.
 * @param pressure Pressure (mb) taken at a specific altitude (meters).
 * @param sealevel Pressure (mb) at sea level.
 * @return Altitude (meters) above sea level.
 */
float pressureToAltitude(float pressure, float sealevel);

/**
 * @brief Convert kilopascals (kPa) to millibars (mb).
 * @param kpa Pressure in kilopascals.
 * @return Pressure in millibars.
 */
static inline float kpaTombar(float kpa)
{
    return kpa * 10.0f;
}

/**
 * @brief Convert millibars (mb) to kilopascals (kPa).
 * @param mb Pressure in millibars.
 * @return Pressure in kilopascals.
 */
static inline float mbarTokpa(float mb)
{
    return mb / 10.0f;
}

} // namespace Geo

#endif // __GEO_H
