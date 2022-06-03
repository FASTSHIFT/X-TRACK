#ifndef __ADAFRUIT_ST7789_H
#define __ADAFRUIT_ST7789_H

#include "Adafruit_GFX_Library/Adafruit_GFX.h"
#include "SPI.h"

#define ADAFRUIT_ST7789_SCREEN_WIDTH_DEFAULT    240
#define ADAFRUIT_ST7789_SCREEN_HEIGHT_DEFAULT   240

class Adafruit_ST7789 : public Adafruit_GFX
{
public:
    Adafruit_ST7789(
        uint8_t cs, uint8_t dc, uint8_t rst,
        SPIClass* spix = &SPI, 
        int16_t w = ADAFRUIT_ST7789_SCREEN_WIDTH_DEFAULT, int16_t h = ADAFRUIT_ST7789_SCREEN_HEIGHT_DEFAULT
    );
    Adafruit_ST7789(
        uint8_t cs, uint8_t dc, uint8_t rst, uint8_t sck, uint8_t mosi,
        int16_t w = ADAFRUIT_ST7789_SCREEN_WIDTH_DEFAULT, int16_t h = ADAFRUIT_ST7789_SCREEN_HEIGHT_DEFAULT
    );

    typedef enum
    {
        COLOR_BLACK   = 0x0000,
        COLOR_BLUE    = 0x001F,
        COLOR_CYAN    = 0x7FFF,
        COLOR_GREEN   = 0x07E0,
        COLOR_MAGENTA = 0xF81F,
        COLOR_RED     = 0xF800,
        COLOR_WHITE   = 0xFFFF,
        COLOR_YELLOW  = 0xFFE0
    } color_t;

    void begin();
    void writeCommand(uint8_t cmd);
    void writeData16(uint16_t data);
    void writeData(uint8_t data);
    void spiWrite(uint8_t data);

    virtual void drawPixel(int16_t x, int16_t y, uint16_t color);
    virtual void setAddrWindow(int16_t x0, int16_t y0, int16_t x1, int16_t y1);
    virtual void setCursor(int16_t x, int16_t y);
    virtual void setRotation(uint8_t r);
    virtual void fillScreen(uint16_t color);
    virtual void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
    virtual void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
    virtual void drawFastRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h);
    virtual void pushColor(uint16_t color);

private:
    uint8_t cs_pin, dc_pin, rst_pin, sck_pin, mosi_pin;
    SPIClass* spi;

#if defined(__STM32__)
    GPIO_TypeDef *csport, *dcport, *sckport, *mosiport;
    uint16_t cspinmask, dcpinmask, sckpinmask, mosipinmask;
#endif

};

#endif
