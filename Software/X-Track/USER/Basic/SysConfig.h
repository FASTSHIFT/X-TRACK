/*
 * MIT License
 * Copyright (c) 2021 _VIFEXTech
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the follo18wing conditions:
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
#ifndef __SYSCONFIG_H
#define __SYSCONFIG_H

#define SYS_FIRMWARE_NAME    "X-Track"    //固件名称
#define SYS_SOFTWARE_VERSION "v0.2"       //软件版本
#define SYS_HARDWARE_VERSION "v1.0"       //硬件版本

#define NULL_PIN            PD0

/* Screen */
#define SCREEN_CS_PIN    PB0
#define SCREEN_DC_PIN    PA4
#define SCREEN_RST_PIN   PA6
#define SCREEN_SCK_PIN   PA5
#define SCREEN_MOSI_PIN  PA7
#define SCREEN_BLK_PIN   PB1

/* Battery */
#define BAT_DET_PIN      PA1
#define BAT_CHG_DET_PIN  PA11

/* Buzzer */
#define BUZZ_PIN         PA0

/* GPS */
#define GPS_TX_PIN       PA3
#define GPS_RX_PIN       PA2

/* IMU */
#define IMU_INT1_PIN     PB10
#define IMU_INT2_PIN     PB11

/* I2C */
#define MCU_SDA_PIN      PB7
#define MCU_SDL_PIN      PB6

/* Encoder */
#define ENCODER_B_PIN    PB5
#define ENCODER_A_PIN    PB4
#define ENCODER_PUSH_PIN PB3

/* Power */
#define POWER_EN_PIN     PA12

/* USART */
#define MCU_RX_PIN       PA10
#define MCU_TX_PIN       PA9

/* SD CARD */
#define SD_CD_PIN        PA8
#define SD_MOSI_PIN      PB15
#define SD_MISO_PIN      PB14
#define SD_SCK_PIN       PB13
#define SD_CS_PIN        PB12

#endif
