#include "HAL.h"
#include "Wire.h"

int HAL::I2C_Scan()
{
    if(!Wire.begin())
    {
        Serial.println("I2C: init failed");
        return -1;
    }

    uint8_t error, address;
    int nDevices;

    Serial.println("I2C: device scanning...");

    nDevices = 0;
    for (address = 1; address < 127; address++ )
    {
        // The i2c_scanner uses the return value of
        // the Write.endTransmisstion to see if
        // a device did acknowledge to the address.
        Wire.beginTransmission(address);
        error = Wire.endTransmission();

        if (error == 0)
        {
            Serial.print("I2C: device found at address 0x");
            if (address < 16)
                Serial.print("0");
            Serial.print(address, HEX);
            Serial.println(" !");

            nDevices++;
        }
        else if (error == 4)
        {
            Serial.print("I2C: unknow error at address 0x");
            if (address < 16)
                Serial.print("0");
            Serial.println(address, HEX);
        }
    }

    Serial.printf("I2C: %d devices was found\r\n", nDevices);
    return nDevices;
}
