#ifndef __BQ27220_H__
#define __BQ27220_H__

#include "Arduino.h"

#define BQ27220_ADDR 0x55

class BQ27220
{
public:
/////// TYPEDEF 
    typedef enum Battery_Status{
        NO_BATTERY = 0x00,
        NORMAL = 0x01,
        FULL = 0x02,
        EMPTY = 0x03,
        CHARGING = 0x04
    } Battery_Status_t;

    typedef struct MAC_Data
    {
        uint8_t data[32];
        uint8_t size;
    } MAC_Data_t;

/////// VAR

    Battery_Status_t battery_status;

    uint16_t voltage; // mV
    int16_t current; // mA
    int16_t average_power; // mW
    uint16_t time_to_empty; // minutes
    uint16_t time_to_full; // minutes
    uint16_t remaining_capacity; // mAh
    uint16_t fullcharge_capacity; // mAh
    uint16_t design_capacity; // mAh
    uint16_t cycle_count; // times
    uint16_t charge_voltage; // mV
    uint16_t charge_current; // mA

    bool should_power_off;

    MAC_Data_t fw_version;

/////// METHOD

    BQ27220(uint8_t addr = BQ27220_ADDR);
    ~BQ27220(void) {}
    bool init(void);    
    bool refreshData(void);
    bool setCapacity(uint16_t design_capacity, uint16_t full_charge_capacity);

private:
/////// VAR
    uint8_t Address;

/////// METHOD
    void WriteReg(uint8_t reg, uint8_t data);
    void WriteReg(uint8_t reg, uint8_t data, uint8_t data2);
    uint8_t ReadReg(uint8_t reg);
    uint16_t ReadRegU16(uint8_t reg);
    bool ReadIncremental(uint8_t reg, uint8_t * buf, size_t size);
};

#endif
