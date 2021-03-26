#include "LIS3MDL.h"
#include <Wire.h>
#include <math.h>

// Defines ////////////////////////////////////////////////////////////////

// The Arduino two-wire interface uses a 7-bit number for the address,
// and sets the last bit correctly based on reads and writes
#define LIS3MDL_SA1_HIGH_ADDRESS  0b0011110
#define LIS3MDL_SA1_LOW_ADDRESS   0b0011100

#define TEST_REG_ERROR -1

#define LIS3MDL_WHO_ID  0x3D

// Constructors ////////////////////////////////////////////////////////////////

LIS3MDL::LIS3MDL(void)
{
    _device = device_auto;

    io_timeout = 0;  // 0 = no timeout
    did_timeout = false;
}

// Public Methods //////////////////////////////////////////////////////////////

// Did a timeout occur in read() since the last call to timeoutOccurred()?
bool LIS3MDL::timeoutOccurred()
{
    bool tmp = did_timeout;
    did_timeout = false;
    return tmp;
}

void LIS3MDL::setTimeout(uint16_t timeout)
{
    io_timeout = timeout;
}

uint16_t LIS3MDL::getTimeout()
{
    return io_timeout;
}

bool LIS3MDL::init(deviceType device, sa1State sa1)
{
    // perform auto-detection unless device type and SA1 state were both specified
    if (device == device_auto || sa1 == sa1_auto)
    {
        // check for LIS3MDL if device is unidentified or was specified to be this type
        if (device == device_auto || device == device_LIS3MDL)
        {
            // check SA1 high address unless SA1 was specified to be low
            if (sa1 != sa1_low && testReg(LIS3MDL_SA1_HIGH_ADDRESS, WHO_AM_I) == LIS3MDL_WHO_ID)
            {
                sa1 = sa1_high;
                if (device == device_auto)
                {
                    device = device_LIS3MDL;
                }
            }
            // check SA1 low address unless SA1 was specified to be high
            else if (sa1 != sa1_high && testReg(LIS3MDL_SA1_LOW_ADDRESS, WHO_AM_I) == LIS3MDL_WHO_ID)
            {
                sa1 = sa1_low;
                if (device == device_auto)
                {
                    device = device_LIS3MDL;
                }
            }
        }

        // make sure device and SA1 were successfully detected; otherwise, indicate failure
        if (device == device_auto || sa1 == sa1_auto)
        {
            return false;
        }
    }

    _device = device;

    switch ((int)device)
    {
    case device_LIS3MDL:
        address = (sa1 == sa1_high) ? LIS3MDL_SA1_HIGH_ADDRESS : LIS3MDL_SA1_LOW_ADDRESS;
        break;
    }

    return true;
}

/*
Enables the LIS3MDL's magnetometer. Also:
- Selects ultra-high-performance mode for all axes
- Sets ODR (output data rate) to default power-on value of 10 Hz
- Sets magnetometer full scale (gain) to default power-on value of +/- 4 gauss
- Enables continuous conversion mode
Note that this function will also reset other settings controlled by
the registers it writes to.
*/
void LIS3MDL::enableDefault(void)
{
    if (_device == device_LIS3MDL)
    {
        // 0x70 = 0b01110000
        // OM = 11 (ultra-high-performance mode for X and Y); DO = 100 (10 Hz ODR)
        writeReg(CTRL_REG1, 0x70);

        // 0x00 = 0b00000000
        // FS = 00 (+/- 4 gauss full scale)
        writeReg(CTRL_REG2, 0x00);

        // 0x00 = 0b00000000
        // MD = 00 (continuous-conversion mode)
        writeReg(CTRL_REG3, 0x00);

        // 0x0C = 0b00001100
        // OMZ = 11 (ultra-high-performance mode for Z)
        writeReg(CTRL_REG4, 0x0C);
    }
}

// Writes a mag register
void LIS3MDL::writeReg(uint8_t reg, uint8_t value)
{
    Wire.beginTransmission(address);
    Wire.write(reg);
    Wire.write(value);
    last_status = Wire.endTransmission();
}

// Reads a mag register
uint8_t LIS3MDL::readReg(uint8_t reg)
{
    uint8_t value;

    Wire.beginTransmission(address);
    Wire.write(reg);
    last_status = Wire.endTransmission();
    Wire.requestFrom(address, (uint8_t)1);
    value = Wire.read();
    Wire.endTransmission();

    return value;
}

// Reads the 3 mag channels and stores them in vector m
void LIS3MDL::read()
{
    Wire.beginTransmission(address);
    // assert MSB to enable subaddress updating
    Wire.write(OUT_X_L | 0x80);
    Wire.endTransmission();
    Wire.requestFrom(address, (uint8_t)6);

    uint16_t millis_start = millis();
    while (Wire.available() < 6)
    {
        if (io_timeout > 0 && ((uint16_t)millis() - millis_start) > io_timeout)
        {
            did_timeout = true;
            return;
        }
    }

    uint8_t xlm = Wire.read();
    uint8_t xhm = Wire.read();
    uint8_t ylm = Wire.read();
    uint8_t yhm = Wire.read();
    uint8_t zlm = Wire.read();
    uint8_t zhm = Wire.read();

    // combine high and low bytes
    m.x = (int16_t)(xhm << 8 | xlm);
    m.y = (int16_t)(yhm << 8 | ylm);
    m.z = (int16_t)(zhm << 8 | zlm);
}

void LIS3MDL::vector_normalize(vector<float> *a)
{
    float mag = sqrt(vector_dot(a, a));
    a->x /= mag;
    a->y /= mag;
    a->z /= mag;
}

// Private Methods //////////////////////////////////////////////////////////////

int16_t LIS3MDL::testReg(uint8_t address, regAddr reg)
{
    Wire.beginTransmission(address);
    Wire.write((uint8_t)reg);
    if (Wire.endTransmission() != 0)
    {
        return TEST_REG_ERROR;
    }

    Wire.requestFrom(address, (uint8_t)1);
    if (Wire.available())
    {
        return Wire.read();
    }
    else
    {
        return TEST_REG_ERROR;
    }
}
