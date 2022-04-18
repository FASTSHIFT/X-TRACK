/*
 * MIT License
 * Copyright (c) 2017 - 2022 _VIFEXTech
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
#include "SPI.h"

#define SPI1_CLOCK                     (F_CPU)
#define SPI2_CLOCK                     (F_CPU)
#define SPI3_CLOCK                     (F_CPU)

SPIClass::SPIClass(SPI_Type* spix)
    : SPIx(spix)
    , SPI_Clock(0)
{
    memset(&SPI_InitStructure, 0, sizeof(SPI_InitStructure));
}

void SPIClass::SPI_Settings(
    uint16_t SPI_Mode_x,
    uint16_t SPI_DataSize_x,
    uint16_t SPI_MODEx,
    uint16_t SPI_NSS_x,
    uint16_t SPI_BaudRatePrescaler_x,
    uint16_t SPI_FirstBit_x)
{
    uint16_t SPI_CPOL_x;
    uint16_t SPI_CPHA_x;
    SPI_Enable(SPIx, DISABLE);

    switch(SPI_MODEx)
    {
    case 0:
        SPI_CPOL_x = SPI_CPOL_LOW;
        SPI_CPHA_x = SPI_CPHA_1EDGE;
        break;
    case 1:
        SPI_CPOL_x = SPI_CPOL_LOW;
        SPI_CPHA_x = SPI_CPHA_2EDGE;
        break;
    case 2:
        SPI_CPOL_x = SPI_CPOL_HIGH;
        SPI_CPHA_x = SPI_CPHA_1EDGE;
        break;
    case 3:
        SPI_CPOL_x = SPI_CPOL_HIGH;
        SPI_CPHA_x = SPI_CPHA_2EDGE;
        break;
    default:
        return;
    }

    SPI_DefaultInitParaConfig(&SPI_InitStructure);
    SPI_InitStructure.SPI_TransMode = SPI_TRANSMODE_FULLDUPLEX;
    SPI_InitStructure.SPI_Mode = SPI_Mode_x;
    SPI_InitStructure.SPI_FrameSize = SPI_DataSize_x;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_x;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_x;
    SPI_InitStructure.SPI_NSSSEL = SPI_NSS_x;
    SPI_InitStructure.SPI_MCLKP = SPI_BaudRatePrescaler_x;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_x;
    SPI_InitStructure.SPI_CPOLY = 3;
    SPI_Init(SPIx, &SPI_InitStructure);

    SPI_Enable(SPIx, ENABLE);
}

void SPIClass::begin(void)
{
    SPI_I2S_Reset(SPIx);
    if(SPIx == SPI1)
    {
        SPI_Clock = SPI1_CLOCK;
        RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_SPI1, ENABLE);
        pinMode(PA5, OUTPUT_AF_PP);
        pinMode(PA6, OUTPUT_AF_PP);
        pinMode(PA7, OUTPUT_AF_PP);
    }
    else if(SPIx == SPI2)
    {
        SPI_Clock = SPI2_CLOCK;
        RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_SPI2, ENABLE);
        pinMode(PB13, OUTPUT_AF_PP);
        pinMode(PB14, OUTPUT_AF_PP);
        pinMode(PB15, OUTPUT_AF_PP);
    }
#ifdef SPI3
    else if(SPIx == SPI3)
    {
        SPI_Clock = SPI3_CLOCK;
        RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_SPI3, ENABLE);
        pinMode(PB3, OUTPUT_AF_PP);
        pinMode(PB4, OUTPUT_AF_PP);
        pinMode(PB5, OUTPUT_AF_PP);
    }
#endif
    else
    {
        return;
    }

    SPI_Settings(
        SPI_MODE_MASTER,
        SPI_FRAMESIZE_8BIT,
        SPI_MODE0,
        SPI_NSSSEL_SOFT,
        SPI_MCLKP_8,
        SPI_FIRSTBIT_MSB
    );
}

void SPIClass::begin(uint32_t clock, uint16_t dataOrder, uint16_t dataMode)
{
    begin();
    setClock(clock);
    setBitOrder(dataOrder);
    setDataMode(dataMode);
    SPI_Enable(SPIx, ENABLE);
}

void SPIClass::begin(SPISettings settings)
{
    begin();
    setClock(settings.clock);
    setBitOrder(settings.bitOrder);
    setDataMode(settings.dataMode);
    SPI_Enable(SPIx, ENABLE);
}

void SPIClass::beginSlave(void)
{
    begin();
    SPI_Settings(
        SPI_MODE_SLAVE,
        SPI_FRAMESIZE_8BIT,
        SPI_MODE0,
        SPI_NSSSEL_HARD,
        SPI_MCLKP_16,
        SPI_FIRSTBIT_MSB
    );
    SPI_Enable(SPIx, ENABLE);
}

void SPIClass::end(void)
{
    SPI_Enable(SPIx, DISABLE);
}

void SPIClass::setClock(uint32_t clock)
{
    if(clock == 0)
    {
        return;
    }

    static const uint16_t mclkpMap[] =
    {
        SPI_MCLKP_2,
        SPI_MCLKP_2,
        SPI_MCLKP_4,
        SPI_MCLKP_8,
        SPI_MCLKP_16,
        SPI_MCLKP_32,
        SPI_MCLKP_64,
        SPI_MCLKP_128,
        SPI_MCLKP_256,
        SPI_MCLKP_512,
        SPI_MCLKP_1024,
    };
    const uint8_t mapSize = sizeof(mclkpMap) / sizeof(mclkpMap[0]);
    uint32_t clockDiv = SPI_Clock / clock;
    uint8_t mapIndex = 0;

    while(clockDiv > 1)
    {
        clockDiv = clockDiv >> 1;
        mapIndex++;
    }

    if(mapIndex >= mapSize)
    {
        mapIndex = mapSize - 1;
    }

    uint16_t mclkp = mclkpMap[mapIndex];

    SPI_InitStructure.SPI_MCLKP = mclkp;
    SPI_Init(SPIx, &SPI_InitStructure);
    SPI_Enable(SPIx, ENABLE);
}

void SPIClass::setClockDivider(uint32_t Div)
{
    if(Div == 0)
    {
        Div = 1;
    }
#if SPI_CLASS_AVR_COMPATIBILITY_MODE
    setClock(16000000 / Div); // AVR:16MHz
#else
    setClock(SPI_Clock / Div);
#endif
}

void SPIClass::setBitOrder(uint16_t bitOrder)
{
    SPI_InitStructure.SPI_FirstBit = (bitOrder == MSBFIRST) ? SPI_FIRSTBIT_MSB : SPI_FIRSTBIT_LSB;
    SPI_Init(SPIx, &SPI_InitStructure);
    SPI_Enable(SPIx, ENABLE);
}

/*  Victor Perez. Added to test changing datasize from 8 to 16 bit modes on the fly.
*   Input parameter should be SPI_CR1_DFF set to 0 or 1 on a 32bit word.
*
*/
void SPIClass::setDataSize(uint16_t datasize)
{
    SPI_InitStructure.SPI_FrameSize = datasize;
    SPI_Init(SPIx, &SPI_InitStructure);
    SPI_Enable(SPIx, ENABLE);
}

void SPIClass::setDataMode(uint8_t dataMode)
{
    /* Notes.  As far as I can tell, the AVR numbers for dataMode appear to match the numbers required by the STM32

    From the AVR doc http://www.atmel.com/images/doc2585.pdf section 2.4

    SPI Mode    CPOL    CPHA    Shift SCK-edge  Capture SCK-edge
    0           0       0       Falling         Rising
    1           0       1       Rising          Falling
    2           1       0       Rising          Falling
    3           1       1       Falling         Rising


    On the STM32 it appears to be

    bit 1 - CPOL : Clock polarity
        (This bit should not be changed when communication is ongoing)
        0 : CLK to 0 when idle
        1 : CLK to 1 when idle

    bit 0 - CPHA : Clock phase
        (This bit should not be changed when communication is ongoing)
        0 : The first clock transition is the first data capture edge
        1 : The second clock transition is the first data capture edge

    If someone finds this is not the case or sees a logic error with this let me know ;-)
     */
    uint16_t SPI_CPOL_x;
    uint16_t SPI_CPHA_x;
    SPI_Enable(SPIx, DISABLE);

    switch(dataMode)
    {
    case SPI_MODE0:
        SPI_CPOL_x = SPI_CPOL_LOW;
        SPI_CPHA_x = SPI_CPHA_1EDGE;
        break;
    case SPI_MODE1:
        SPI_CPOL_x = SPI_CPOL_LOW;
        SPI_CPHA_x = SPI_CPHA_2EDGE;
        break;
    case SPI_MODE2:
        SPI_CPOL_x = SPI_CPOL_HIGH;
        SPI_CPHA_x = SPI_CPHA_1EDGE;
        break;
    case SPI_MODE3:
        SPI_CPOL_x = SPI_CPOL_HIGH;
        SPI_CPHA_x = SPI_CPHA_2EDGE;
        break;
    default:
        return;
    }

    SPI_InitStructure.SPI_CPOL = SPI_CPOL_x;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_x;
    SPI_Init(SPIx, &SPI_InitStructure);
    SPI_Enable(SPIx, ENABLE);
}

void SPIClass::beginTransaction(SPISettings settings)
{
    SPISettings(settings.clock, settings.bitOrder, settings.dataMode);

    setClock(settings.clock);
    setBitOrder(settings.bitOrder);
    setDataMode(settings.dataMode);
    setDataSize(settings.dataSize);

    SPI_Enable(SPIx, ENABLE);
}

void SPIClass::beginTransactionSlave(void)
{
    beginSlave();
}

void SPIClass::endTransaction(void)
{
    SPI_Enable(SPIx, DISABLE);
}

uint16_t SPIClass::read(void)
{
    SPI_I2S_WAIT_RX(SPIx);
    return (uint16_t)(SPI_I2S_RXDATA(SPIx));
}

void SPIClass::read(uint8_t* buf, uint32_t len)
{
    if (len == 0)
        return;

    SPI_I2S_RXDATA_VOLATILE(SPIx);
    SPI_I2S_TXDATA(SPIx, 0x00FF);

    while((--len))
    {
        SPI_I2S_WAIT_TX(SPIx);
        noInterrupts();
        SPI_I2S_TXDATA(SPIx, 0x00FF);
        SPI_I2S_WAIT_RX(SPIx);
        *buf++ = (uint8_t)SPI_I2S_RXDATA(SPIx);
        interrupts();
    }
    SPI_I2S_WAIT_RX(SPIx);
    *buf++ = (uint8_t)SPI_I2S_RXDATA(SPIx);
}

void SPIClass::write(uint16_t data)
{
    SPI_I2S_TXDATA(SPIx, data);
    SPI_I2S_WAIT_TX(SPIx);
    SPI_I2S_WAIT_BUSY(SPIx);
}

void SPIClass::write(uint16_t data, uint32_t n)
{
    while ((n--) > 0)
    {
        SPI_I2S_TXDATA(SPIx, data); // write the data to be transmitted into the SPI_DR register (this clears the TXE flag)
        SPI_I2S_WAIT_TX(SPIx); // wait till Tx empty
    }

    SPI_I2S_WAIT_BUSY(SPIx); // wait until BSY=0 before returning
}

void SPIClass::write(const uint8_t* data, uint32_t length)
{
    while (length--)
    {
        SPI_I2S_WAIT_TX(SPIx);
        SPI_I2S_TXDATA(SPIx, *data++);
    }
    SPI_I2S_WAIT_TX(SPIx);
    SPI_I2S_WAIT_BUSY(SPIx);
}

void SPIClass::write(const uint16_t* data, uint32_t length)
{
    while (length--)
    {
        SPI_I2S_WAIT_TX(SPIx);
        SPI_I2S_TXDATA(SPIx, *data++);
    }
    SPI_I2S_WAIT_TX(SPIx);
    SPI_I2S_WAIT_BUSY(SPIx);
}

uint8_t SPIClass::transfer(uint8_t wr_data) const
{
    SPI_I2S_RXDATA_VOLATILE(SPIx);
    SPI_I2S_TXDATA(SPIx, wr_data);
    SPI_I2S_WAIT_TX(SPIx);
    SPI_I2S_WAIT_BUSY(SPIx);
    return (uint8_t)SPI_I2S_RXDATA(SPIx);
}

uint16_t SPIClass::transfer16(uint16_t wr_data) const
{
    SPI_I2S_RXDATA_VOLATILE(SPIx);
    SPI_I2S_TXDATA(SPIx, wr_data);
    SPI_I2S_WAIT_TX(SPIx);
    SPI_I2S_WAIT_BUSY(SPIx);
    return (uint16_t)SPI_I2S_RXDATA(SPIx);
}

uint8_t SPIClass::send(uint8_t data)
{
    this->write(data);
    return 1;
}

uint8_t SPIClass::send(uint8_t* buf, uint32_t len)
{
    this->write(buf, len);
    return len;
}

uint8_t SPIClass::recv(void)
{
    return this->read();
}

#if SPI_CLASS_1_ENABLE
SPIClass SPI(SPI_CLASS_1_SPI);
#endif

#if SPI_CLASS_2_ENABLE
SPIClass SPI_2(SPI_CLASS_2_SPI);
#endif

#if SPI_CLASS_3_ENABLE
SPIClass SPI_3(SPI_CLASS_3_SPI);
#endif
