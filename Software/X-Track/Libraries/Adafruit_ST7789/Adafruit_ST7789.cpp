#include "Adafruit_ST7789.h"
#include "SPI.h"

#define SCREEN_USE_LITTLE_ENDIAN    0

#if defined(__STM32__)
#define SCREEN_CS_SET      GPIO_HIGH(csport,cspinmask)
#define SCREEN_DC_SET      GPIO_HIGH(dcport,dcpinmask)
#define SCREEN_SCK_SET     GPIO_HIGH(sckport,sckpinmask)
#define SCREEN_MOSI_SET    GPIO_HIGH(mosiport,mosipinmask)

#define SCREEN_CS_CLR      GPIO_LOW(csport,cspinmask)
#define SCREEN_DC_CLR      GPIO_LOW(dcport,dcpinmask)
#define SCREEN_SCK_CLR     GPIO_LOW(sckport,sckpinmask)
#define SCREEN_MOSI_CLR    GPIO_LOW(mosiport,mosipinmask)
#else
#define SCREEN_CS_SET      digitalWrite(cs_pin,HIGH)
#define SCREEN_DC_SET      digitalWrite(dc_pin,HIGH)
#define SCREEN_SCK_SET     digitalWrite(sck_pin,HIGH)
#define SCREEN_MOSI_SET    digitalWrite(mosi_pin,HIGH)

#define SCREEN_CS_CLR      digitalWrite(cs_pin,LOW)
#define SCREEN_DC_CLR      digitalWrite(dc_pin,LOW)
#define SCREEN_SCK_CLR     digitalWrite(sck_pin,LOW)
#define SCREEN_MOSI_CLR    digitalWrite(mosi_pin,LOW)
#endif


Adafruit_ST7789::Adafruit_ST7789(uint8_t cs, uint8_t dc, uint8_t rst, SPIClass* spix, int16_t w, int16_t h)
    : Adafruit_GFX(w, h)
    , cs_pin(cs)
    , dc_pin(dc)
    , rst_pin(rst)
    , sck_pin(NOT_A_PIN)
    , mosi_pin(NOT_A_PIN)
    , spi(spix)
{
#if defined(__STM32__)
    csport = digitalPinToPort(cs);
    cspinmask = digitalPinToBitMask(cs);
    dcport = digitalPinToPort(dc);
    dcpinmask = digitalPinToBitMask(dc);
#endif
}

Adafruit_ST7789::Adafruit_ST7789(uint8_t cs, uint8_t dc, uint8_t rst, uint8_t sck, uint8_t mosi, int16_t w, int16_t h)
    : Adafruit_GFX(w, h)
    , cs_pin(cs)
    , dc_pin(dc)
    , rst_pin(rst)
    , sck_pin(sck)
    , mosi_pin(mosi)
    , spi(NULL)
{

#if defined(__STM32__)
    csport = digitalPinToPort(cs);
    cspinmask = digitalPinToBitMask(cs);
    dcport = digitalPinToPort(dc);
    dcpinmask = digitalPinToBitMask(dc);
    sckport = digitalPinToPort(sck);
    sckpinmask = digitalPinToBitMask(sck);
    mosiport = digitalPinToPort(mosi);
    mosipinmask = digitalPinToBitMask(mosi);
#endif
}

void Adafruit_ST7789::begin()
{
    if(spi)
    {
        spi->begin();
        spi->setClock(F_CPU);
    }
    else
    {
        pinMode(sck_pin, OUTPUT);
        pinMode(mosi_pin, OUTPUT);
    }

    pinMode(cs_pin, OUTPUT);
    pinMode(dc_pin, OUTPUT);
    pinMode(rst_pin, OUTPUT);

    digitalWrite(cs_pin, HIGH);

    digitalWrite(rst_pin, LOW);
    delay(100);
    digitalWrite(rst_pin, HIGH);
    delay(100);

    writeCommand(0x11); //Sleep out
    delay(120);

    setRotation(0);

    writeCommand(0x3A);
    writeData(0x05);

#if SCREEN_USE_LITTLE_ENDIAN
    /* Change to Little Endian */
    writeCommand(0xB0);
    writeData(0x00);  // RM = 0; DM = 00
    writeData(0xF8);  // EPF = 11; ENDIAN = 1; RIM = 0; MDT = 00 (ENDIAN -> 0 MSBFirst; 1 LSB First)
#endif

    writeCommand(0xB2);
    writeData(0x0C);
    writeData(0x0C);
    writeData(0x00);
    writeData(0x33);
    writeData(0x33);

    writeCommand(0xB7);
    writeData(0x35);

    writeCommand(0xBB);
    writeData(0x32); //Vcom=1.35V

    writeCommand(0xC2);
    writeData(0x01);

    writeCommand(0xC3);
    writeData(0x15); //GVDD=4.8V

    writeCommand(0xC4);
    writeData(0x20); //VDV, 0x20:0v

    writeCommand(0xC6);
    writeData(0x0F); //0x0F:60Hz

    writeCommand(0xD0);
    writeData(0xA4);
    writeData(0xA1);

    writeCommand(0xE0);
    writeData(0xD0);
    writeData(0x08);
    writeData(0x0E);
    writeData(0x09);
    writeData(0x09);
    writeData(0x05);
    writeData(0x31);
    writeData(0x33);
    writeData(0x48);
    writeData(0x17);
    writeData(0x14);
    writeData(0x15);
    writeData(0x31);
    writeData(0x34);

    writeCommand(0xE1);
    writeData(0xD0);
    writeData(0x08);
    writeData(0x0E);
    writeData(0x09);
    writeData(0x09);
    writeData(0x15);
    writeData(0x31);
    writeData(0x33);
    writeData(0x48);
    writeData(0x17);
    writeData(0x14);
    writeData(0x15);
    writeData(0x31);
    writeData(0x34);
    writeCommand(0x21);

    writeCommand(0x29);
}

void Adafruit_ST7789::spiWrite(uint8_t data)
{
    for(uint8_t i = 0; i < 8; i++)
    {
        SCREEN_SCK_CLR;
        (data & 0x80) ? SCREEN_MOSI_SET : SCREEN_MOSI_CLR;
        SCREEN_SCK_SET;
        data <<= 1;
    }
}

void Adafruit_ST7789::writeCommand(uint8_t cmd)
{
    SCREEN_CS_CLR;
    SCREEN_DC_CLR;
    if(spi)
        spi->transfer(cmd);
    else
        spiWrite(cmd);
    SCREEN_CS_SET;
}

void Adafruit_ST7789::writeData16(uint16_t data)
{
    SCREEN_CS_CLR;
    SCREEN_DC_SET;
    if(spi)
    {
        spi->transfer(data >> 8);
        spi->transfer(data);
    }
    else
    {
        spiWrite(data >> 8);
        spiWrite(data);
    }
    SCREEN_CS_SET;
}

void Adafruit_ST7789::writeData(uint8_t data)
{
    SCREEN_CS_CLR;
    SCREEN_DC_SET;
    if(spi)
    {
        spi->transfer(data);
    }
    else
    {
        spiWrite(data);
    }
    SCREEN_CS_SET;
}

void Adafruit_ST7789::setRotation(uint8_t r)
{
    rotation = r % 4;
    writeCommand(0x36);
    switch(rotation)
    {
    case 0:
        _width = WIDTH;
        _height = HEIGHT;

        writeData(0x00);
        break;

    case 1:
        _width = HEIGHT;
        _height = WIDTH;

        writeData(0xC0);
        break;

    case 2:
        _width = WIDTH;
        _height = HEIGHT;

        writeData(0x70);
        break;

    case 3:
        _width = HEIGHT;
        _height = WIDTH;

        writeData(0xA0);
        break;
    }
}

void Adafruit_ST7789::setAddrWindow(int16_t x0, int16_t y0, int16_t x1, int16_t y1)
{
    switch(rotation)
    {
    case 0:
        writeCommand(0x2A);
        writeData16(x0);
        writeData16(x1);

        writeCommand(0x2B);
        writeData16(y0);
        writeData16(y1);
        break;
    case 1:
        writeCommand(0x2A);
        writeData16(x0);
        writeData16(x1);

        writeCommand(0x2B);
        writeData16(y0 + 80);
        writeData16(y1 + 80);
        break;
    case 2:
        writeCommand(0x2A);
        writeData16(x0);
        writeData16(x1);

        writeCommand(0x2B);
        writeData16(y0);
        writeData16(y1);
        break;
    case 3:
        writeCommand(0x2A);
        writeData16(x0 + 80);
        writeData16(x1 + 80);

        writeCommand(0x2B);
        writeData16(y0);
        writeData16(y1);
        break;

    }

    writeCommand(0x2C);
}

void Adafruit_ST7789::setCursor(int16_t x, int16_t y)
{
    cursor_x = x;
    cursor_y = y;
    if((x < 0) || (x >= _width) || (y < 0) || (y >= _height)) return;
    setAddrWindow(x, y, x, y);
}

void Adafruit_ST7789::drawPixel(int16_t x, int16_t y, uint16_t color)
{
    if((x < 0) || (x >= _width) || (y < 0) || (y >= _height)) return;
    setAddrWindow(x, y, x, y);
    pushColor(color);
}

void Adafruit_ST7789::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
    // Rudimentary clipping
    if((x >= _width) || (y >= _height) || h < 1) return;
    if((y + h - 1) >= _height) h = _height - y;
    if(x < 0) x = 0;
    if(y < 0) y = 0;

    setAddrWindow(x, y, x, y + h - 1);

    while (h--)
        pushColor(color);
}

void Adafruit_ST7789::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
    // Rudimentary clipping
    if((x >= _width) || (y >= _height) || w < 1) return;
    if((x + w - 1) >= _width)  w = _width - x;
    if(x < 0) x = 0;
    if(y < 0) y = 0;

    setAddrWindow(x, y, x + w - 1, y);

    while (w--)
        pushColor(color);
}

void Adafruit_ST7789::fillScreen(uint16_t color)
{
    setAddrWindow(0, 0, _width - 1, _height - 1);
    uint32_t size = _width * _height;

#ifdef __STM32__
    if(spi)
    {
        SPI_TypeDef* SPIx = spi->getSPI();
        SCREEN_CS_CLR;
        SCREEN_DC_SET;
        while(size--)
        {
#if SCREEN_USE_LITTLE_ENDIAN
            SPI_I2S_WAIT_TX(SPIx);
            SPI_I2S_TXDATA(SPIx, color);
            SPI_I2S_WAIT_TX(SPIx);
            SPI_I2S_TXDATA(SPIx, color >> 8);
#else
            SPI_I2S_WAIT_TX(SPIx);
            SPI_I2S_TXDATA(SPIx, color >> 8);
            SPI_I2S_WAIT_TX(SPIx);
            SPI_I2S_TXDATA(SPIx, color);
#endif
        }
        SPI_I2S_WAIT_TX(SPIx);
        SPI_I2S_WAIT_BUSY(SPIx);
        SCREEN_CS_SET;
        return;
    }
#endif

    while(size--)
    {
        pushColor(color);
    }
}

void Adafruit_ST7789::drawFastRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h)
{
#if defined(__STM32__)
    if(spi)
    {
        SPI_TypeDef* SPIx = spi->getSPI();

        setAddrWindow(x, y, x + w - 1, y + h - 1);
        uint32_t size = w * h;

        SCREEN_CS_CLR;
        SCREEN_DC_SET;
        while(size--)
        {
#if SCREEN_USE_LITTLE_ENDIAN
            SPI_I2S_WAIT_TX(SPIx);
            SPI_I2S_TXDATA(SPIx, *bitmap);
            SPI_I2S_WAIT_TX(SPIx);
            SPI_I2S_TXDATA(SPIx, *bitmap >> 8);
            bitmap++;
#else
            SPI_I2S_WAIT_TX(SPIx);
            SPI_I2S_TXDATA(SPIx, *bitmap >> 8);
            SPI_I2S_WAIT_TX(SPIx);
            SPI_I2S_TXDATA(SPIx, *bitmap++);
#endif
        }
        SPI_I2S_WAIT_TX(SPIx);
        SPI_I2S_WAIT_BUSY(SPIx);
        SCREEN_CS_SET;
    }
#else
    drawRGBBitmap(x, y, bitmap, w, h);
#endif
}

void Adafruit_ST7789::pushColor(uint16_t color)
{
#if SCREEN_USE_LITTLE_ENDIAN
    writeData(color);
    writeData(color >> 8);
#else
    writeData16(color);
#endif
}
