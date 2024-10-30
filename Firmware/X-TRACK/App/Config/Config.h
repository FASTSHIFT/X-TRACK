/*
 * MIT License
 * Copyright (c) 2021 - 2023 _VIFEXTech
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
#ifndef __CONFIG_H
#define __CONFIG_H

/* clang-format off */

/*=========================
  Application configuration
 *=========================*/

/* Default time zone (GMT+ X)*/
#define CONFIG_TIME_ZONE_DEFAULT              8

/* Default language code
 * zh-CN: Chinese (PRC)
 * en-US: English (United States)
 *
 * Language coding standards: https://www.andiamo.co.uk/resources/iso-language-codes
 */
#define CONFIG_LANGUAGE_DEFAULT               "zh-CN"

/* Default weight (kilogram) */
#define CONFIG_WEIGHT_DEFAULT                 65

/* Default wheel diameter (millimeter) */
#define CONFIG_WHEEL_DIAMETER_DEFAULT         700

/* Default GNSS data refresh period (milliseconds) */
#ifndef CONFIG_GNSS_UPDATE_PERIOD
#  define CONFIG_GNSS_UPDATE_PERIOD           1000
#endif

/* Default latitude and longitude */
#define CONFIG_GNSS_LONGITUDE_DEFAULT         116.391332f
#define CONFIG_GNSS_LATITUDE_DEFAULT          39.907415f

/* Track file storage directory */
#define CONFIG_TRACK_RECORD_FILE_DIR_NAME     "Track"

/* Coordinate conversion
 * true:  GCJ-02 (GCJ-02 Coordinate System)
 * false: WGS-84 (World Geodetic System)
 */
#define CONFIG_MAP_COORD_TRANSFORM_ENABLE     true

/* Default map file storage directory */
#define CONFIG_MAP_DIR_PATH_DEFAULT           "/MAP"

/* Default map file extension name */
#define CONFIG_MAP_EXT_NAME_DEFAULT           "png"

/* Watchdog timeout (seconds) */
#ifndef CONFIG_WATCHDOG_TIMEOUT
#define CONFIG_WATCHDOG_TIMEOUT               10
#endif

/* clang-format on */

#endif /* __CONFIG_H */
