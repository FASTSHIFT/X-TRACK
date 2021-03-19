#ifndef __LIS3MDL_H
#define __LIS3MDL_H

#include "Arduino.h"

#define LIS3MDL_I2C_ADDRESS     0x1C

class LIS3MDL
{
public:
    LIS3MDL() {}
    ~LIS3MDL() {}

    typedef enum
    {
        OperatingMode_LowPower             = B0000000,
        OperatingMode_MediumPerformance    = B0100000,
        OperatingMode_HighPerformance      = B1000000,
        OperatingMode_UltraHighPerformance = B1100000,
    } OperatingMode_t;
    
    typedef enum
    {
        ODR_0_625_HZ = B00000,
        ODR_1_24_HZ  = B00100,
        ODR_2_5_HZ   = B01000,
        ODR_5_HZ     = B01100,
        ODR_10_HZ    = B10000,
        ODR_20_HZ    = B10100,
        ODR_40_HZ    = B11000,
        ODR_80_HZ    = B11100,
    }OutputDataRateConfig_t;
    
    typedef enum
    {
        ConversionMode_Continuous = B00,
        ConversionMode_Single     = B01,
        ConversionMode_Powndown   = B10
    } ConversionMode_t;

    bool Init(uint8_t addr = LIS3MDL_I2C_ADDRESS);
    bool IsConnected();
    int16_t GetTemperature();
    void GetAxis(int16_t* x, int16_t* y, int16_t* z);
    float GetAngle(int* buf, float ax, float ay);
    uint8_t GetStatus();
    
    void SetCalibration(
        int16_t x_offset,
        int16_t y_offset,
        int16_t z_offset,
        float y_scale,
        float z_scale
    );

    void SetTemperatureSensorEnable(bool en);
    void SetOperatingMode(OperatingMode_t mode);
    void SetFastODREnable(bool en);
    void SetSelfTestEnable(bool en);
    void SetOutputDataRateConfig(OutputDataRateConfig_t config);
    void SetConversionMode(ConversionMode_t mode);
private:
    uint8_t Address;
    void WriteReg(uint8_t reg, uint8_t data);
    uint8_t ReadReg(uint8_t reg);
    struct
    {
        int16_t X;
        int16_t Y;
        int16_t Z;
    } MagOffset;

    struct
    {
        float Y;
        float Z;
    } MagScale;
};

#endif
