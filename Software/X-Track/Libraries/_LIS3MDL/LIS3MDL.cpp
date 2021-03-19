#include "LIS3MDL.h"
#include "Wire.h"
#include <math.h>

#define LIS3MDL_WHO_AM_I     0x0F
#define LIS3MDL_CTRL_REG1    0x20
#define LIS3MDL_CTRL_REG2    0x21
#define LIS3MDL_CTRL_REG3    0x22
#define LIS3MDL_CTRL_REG4    0x23
#define LIS3MDL_CTRL_REG5    0x24
#define LIS3MDL_STATUS_REG   0x27
#define LIS3MDL_OUT_X_L      0x28
#define LIS3MDL_OUT_X_H      0x29
#define LIS3MDL_OUT_Y_L      0x2A
#define LIS3MDL_OUT_Y_H      0x2B
#define LIS3MDL_OUT_Z_L      0x2A
#define LIS3MDL_OUT_Z_H      0x2B
#define LIS3MDL_TEMP_OUT_L   0x2E
#define LIS3MDL_TEMP_OUT_H   0x2F
#define LIS3MDL_INT_CFG      0x30
#define LIS3MDL_INT_SRC      0x31
#define LIS3MDL_INT_THS_L    0x32
#define LIS3MDL_INT_THS_H    0x33

bool LIS3MDL::Init(uint8_t addr)
{
    Address = addr;
    
    /** temperature sensor enabled;
      * High-performance mode
      * ODR = 100Hz
      * FAST_ODR enabled
      * self-test disabled
      */
    WriteReg(LIS3MDL_CTRL_REG1, B11111100);
    
    /** Full-scale configuration ¡À4 gauss
      * Reboot memory content normal mode
      * Configuration registers and user register reset function Default value
      */
    WriteReg(LIS3MDL_CTRL_REG2, B00000000);
    
    /** Low-power mode configuration Default value: 0
      * SPI serial interface mode selection. Default value: 0
      * Operating mode selection Continuous-conversion mode
      */
    WriteReg(LIS3MDL_CTRL_REG3, B00000010);
    
    /** Z-axis operating mode selection High-performance mode
      * LSb at lower address
      */
    WriteReg(LIS3MDL_CTRL_REG4, B00001100);
    
    /** FAST_READ disabled
      * Block data update for magnetic data continuous update
      */
    WriteReg(LIS3MDL_CTRL_REG5, B00000000);
    
    /** disable X interrupt request
      * disable Y interrupt request
      * disable Z interrupt request
      * Interrupt active configuration on INT low
      * Latch interrupt request interrupt request latched
      * Interrupt enable on INT pin disable
      */
    WriteReg(LIS3MDL_INT_CFG, B00000000);

    return IsConnected();
}

uint8_t LIS3MDL::GetStatus()
{
    return ReadReg(LIS3MDL_STATUS_REG);
}

bool LIS3MDL::IsConnected()
{
    return (ReadReg(LIS3MDL_WHO_AM_I) == B00111101);
}

void LIS3MDL::GetAxis(int16_t* x, int16_t* y, int16_t* z)
{
    uint8_t x_low  = ReadReg(LIS3MDL_OUT_X_L);
    uint8_t x_high = ReadReg(LIS3MDL_OUT_X_H);
    uint8_t y_low  = ReadReg(LIS3MDL_OUT_Y_L);
    uint8_t y_high = ReadReg(LIS3MDL_OUT_Y_H);
    uint8_t z_low  = ReadReg(LIS3MDL_OUT_Z_L);
    uint8_t z_high = ReadReg(LIS3MDL_OUT_Z_H);
    
    *x = x_high << 8 | x_low;
    *y = y_high << 8 | y_low;
    *z = z_high << 8 | z_low;
    
    for(uint8_t reg = 0x20; reg <= 0x33; reg++)
    {
        Serial.println(ReadReg(reg));
    }
}

int16_t LIS3MDL::GetTemperature()
{
    uint8_t temp_low = ReadReg(LIS3MDL_TEMP_OUT_L);
    uint8_t temp_high = ReadReg(LIS3MDL_TEMP_OUT_H);
    return (temp_high << 8 | temp_low);
}

void LIS3MDL::SetTemperatureSensorEnable(bool en)
{
    uint8_t val = ReadReg(LIS3MDL_CTRL_REG1);
    en ? bitSet(val, 7) : bitClear(val, 7);
    WriteReg(LIS3MDL_CTRL_REG1, val);
}

void LIS3MDL::SetOperatingMode(OperatingMode_t mode)
{
    uint8_t val = ReadReg(LIS3MDL_CTRL_REG1);
    val &= ~B01100000;
    val |= mode;
    WriteReg(LIS3MDL_CTRL_REG1, val);
}

void LIS3MDL::SetFastODREnable(bool en)
{
    uint8_t val = ReadReg(LIS3MDL_CTRL_REG1);
    en ? bitSet(val, 1) : bitClear(val, 1);
    WriteReg(LIS3MDL_CTRL_REG1, val);
}

void LIS3MDL::SetSelfTestEnable(bool en)
{
    uint8_t val = ReadReg(LIS3MDL_CTRL_REG1);
    en ? bitSet(val, 0) : bitClear(val, 0);
    WriteReg(LIS3MDL_CTRL_REG1, val);
}

void LIS3MDL::SetOutputDataRateConfig(OutputDataRateConfig_t config)
{
    uint8_t val = ReadReg(LIS3MDL_CTRL_REG1);
    val &= ~B11100;
    val |= config;
    WriteReg(LIS3MDL_CTRL_REG1, val);
}

void LIS3MDL::SetCalibration(
    int16_t x_offset,
    int16_t y_offset,
    int16_t z_offset,
    float y_scale,
    float z_scale
)
{
    MagOffset.X = x_offset;
    MagOffset.Y = y_offset;
    MagOffset.Z = z_offset;
    MagScale.Y = y_scale;
    MagScale.Z = z_scale;
}

void LIS3MDL::SetConversionMode(ConversionMode_t mode)
{
    uint8_t val = ReadReg(LIS3MDL_CTRL_REG3);
    val &= ~B00000011;
    val |= mode;
    WriteReg(LIS3MDL_CTRL_REG3, val);
}

void LIS3MDL::WriteReg(uint8_t reg, uint8_t data)
{
    Wire.beginTransmission(Address);
    Wire.write(reg);
    Wire.write(data);
    Wire.endTransmission();
}

uint8_t LIS3MDL::ReadReg(uint8_t reg)
{
    Wire.beginTransmission(Address);
    Wire.write(reg);
    Wire.endTransmission();

    Wire.requestFrom(Address, 1);
    uint8_t data = Wire.read();
    Wire.endTransmission();

    return data;
}
