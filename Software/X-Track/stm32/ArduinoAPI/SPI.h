/*
 * MIT License
 * Copyright (c) 2019 _VIFEXTech
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
#ifndef __SPI_H
#define __SPI_H

#include "Arduino.h"

/**
  *@SPI1: SCK->PA5  MISO->PA6  MOSI->PA7
  *@SPI2: SCK->PB13 MISO->PB14 MOSI->PB15
  *@SPI3: SCK->PB3  MISO->PB4  MOSI->PB5
  */

#ifndef LSBFIRST
#define LSBFIRST 0
#endif
#ifndef MSBFIRST
#define MSBFIRST 1
#endif


typedef enum
{
    SPI_MODE0,
    SPI_MODE1,
    SPI_MODE2,
    SPI_MODE3
} SPI_MODE_TypeDef;

#define DATA_SIZE_8BIT   SPI_DataSize_8b
#define DATA_SIZE_16BIT  SPI_DataSize_16b

#define SPI_CLOCK_DIV2   2
#define SPI_CLOCK_DIV4   4
#define SPI_CLOCK_DIV8   8
#define SPI_CLOCK_DIV16  16
#define SPI_CLOCK_DIV32  32
#define SPI_CLOCK_DIV64  64
#define SPI_CLOCK_DIV128 128

typedef uint16_t BitOrder;
typedef SPI_TypeDef spi_dev;

typedef enum {
    SPI_STATE_IDLE,
    SPI_STATE_READY,
    SPI_STATE_RECEIVE,
    SPI_STATE_TRANSMIT,
    SPI_STATE_TRANSFER
} spi_mode_t;

class SPISettings
{
public:
    SPISettings(uint32_t clock, BitOrder bitOrder, uint8_t dataMode) {
        init_AlwaysInline(clock, bitOrder, dataMode, DATA_SIZE_8BIT);
    }
    SPISettings(uint32_t clock, BitOrder bitOrder, uint8_t dataMode, uint32_t dataSize) {
        init_AlwaysInline(clock, bitOrder, dataMode, dataSize);
    }
    SPISettings(uint32_t clock) {
        init_AlwaysInline(clock, MSBFIRST, SPI_MODE0, DATA_SIZE_8BIT);
    }
    SPISettings() {
        init_AlwaysInline(4000000, MSBFIRST, SPI_MODE0, DATA_SIZE_8BIT);
    }
private:
    void init_MightInline(uint32_t clock, BitOrder bitOrder, uint8_t dataMode, uint32_t dataSize) {
        init_AlwaysInline(clock, bitOrder, dataMode, dataSize);
    }
    void init_AlwaysInline(uint32_t clock, BitOrder bitOrder, uint8_t dataMode, uint32_t dataSize) __attribute__((__always_inline__)) {
        this->clock = clock;
        this->bitOrder = bitOrder;
        this->dataMode = dataMode;
        this->dataSize = dataSize;
    }
    uint32_t clock;
    uint32_t dataSize;
    uint32_t clockDivider;
    BitOrder bitOrder;
    uint8_t dataMode;
    uint8_t _SSPin;
    volatile spi_mode_t state;

    friend class SPIClass;
};


class SPIClass
{
public:
    SPIClass(SPI_TypeDef* _SPIx);
    void SPI_Settings(  SPI_TypeDef* SPIx,
                        uint16_t SPI_Mode_x,
                        uint16_t SPI_DataSize_x,
                        uint16_t SPI_MODEx,
                        uint16_t SPI_NSS_x,
                        uint16_t SPI_BaudRatePrescaler_x,
                        uint16_t SPI_FirstBit_x);
    void begin(void);
    void begin(uint32_t clock, uint16_t dataOrder, uint16_t dataMode);
    void begin(SPISettings settings);
    void beginSlave(uint32_t bitOrder, uint32_t mode);
    void beginSlave(void);
    void beginTransactionSlave(void);
    void beginTransaction(SPISettings settings);

    void endTransaction(void);
    void end(void);

    void setClock(uint32_t clock);
    void setClockDivider(uint32_t Div);
    void setBitOrder(uint16_t bitOrder);
    void setDataMode(uint8_t dataMode);
    void setDataSize(uint16_t datasize);

    uint16_t read(void);
    void read(uint8_t *buffer, uint32_t length);
    void write(uint16_t data);
    void write(uint16_t data, uint32_t n);
    void write(const uint8_t *data, uint32_t length);
    void write(const uint16_t *data, uint32_t length);
    uint8_t transfer(uint8_t data) const;
    uint16_t transfer16(uint16_t data) const;
    uint8_t send(uint8_t data);
    uint8_t send(uint8_t *data, uint32_t length);
    uint8_t recv(void);

private:
    SPI_TypeDef* SPIx;
    SPI_InitTypeDef  SPI_InitStructure;
    uint32_t SPI_Clock;
};

extern SPIClass SPI;
extern SPIClass SPI_2;
extern SPIClass SPI_3;

#endif
