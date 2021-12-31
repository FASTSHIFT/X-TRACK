#include "BQ27220.h"
#include "HAL/HAL_Config.h"
#include "Wire.h"

/////// DEFINE

#define I2C_DELAY           2
#define I2C_BEFORE_TRANS    uint8_t __ORIGIN_DELAY = Wire.i2c_delay;Wire.i2c_delay = I2C_DELAY;
#define I2C_AFTER_TRANS     Wire.i2c_delay = __ORIGIN_DELAY;

#define I2C_READ_DELAY_US   150
#define I2C_WRITE_DELAY_MS  2000

#define REG_GAUGE_CTRL_L 0x00
#define REG_GAUGE_CTRL_H 0x01

#define REG_BATTERY_STATUS_L 0x0A
#define REG_BATTERY_STATUS_H 0x0B

#define REG_VOLTAGE_L 0x08
#define REG_VOLTAGE_H 0x09

#define REG_CURRENT_L 0x0C
#define REG_CURRENT_H 0x0D

#define REG_AVERAGE_POWER_L 0x24
#define REG_AVERAGE_POWER_H 0x25

#define REG_TIME_TO_EMPTY_L 0x16
#define REG_TIME_TO_EMPTY_H 0x17

#define REG_TIME_TO_FULL_L  0x18
#define REG_TIME_TO_FULL_H  0x19

#define REG_REMAINING_CAP_L 0x10
#define REG_REMAINING_CAP_H 0x11

#define REG_FULL_CHARGE_CAP_L 0x12
#define REG_FULL_CHARGE_CAP_H 0x13

#define REG_DESIGN_CAP_L 0x3C
#define REG_DESIGN_CAP_H 0x3D

#define REG_CYCLE_COUNT_L  0x2A
#define REG_CYCLE_COUNT_H  0x2B

#define REG_CHARGE_VOLTAGE_L 0x30
#define REG_CHARGE_VOLTAGE_H 0x31

#define REG_CHARGE_CURRENT_L 0x32
#define REG_CHARGE_CURRENT_H 0x33

#define REG_OPERATION_STATUS_L 0x3A
#define REG_OPERATION_STATUS_H 0x3B

#define REG_DATA_MEMORY_L 0x3E
#define REG_DATA_MEMORY_H 0x3F

#define REG_MAC_DATA 0x40
#define REG_MAC_CHECK_SUM 0x60
#define REG_MAC_DATA_LEN 0x61


/////// UTIL
static uint16_t u8_to_u16(uint8_t msb, uint8_t lsb){
    uint16_t x = ((0xFFFF & (uint16_t)msb) << 8) + lsb;
    return x;
}

/////// PUBLIC

BQ27220::BQ27220(uint8_t addr){
    Address = addr;
}

bool BQ27220::init(void){
    // TODO
    return true;
}

bool BQ27220::setCapacity(uint16_t design_capacity, uint16_t full_charge_capacity){
    if (ReadRegU16(REG_DESIGN_CAP_L) == design_capacity && ReadRegU16(REG_FULL_CHARGE_CAP_L) == full_charge_capacity){
        return false;
    }
    
    uint8_t operation_status_lsb = ReadReg(REG_OPERATION_STATUS_L);
    if ((operation_status_lsb & 0b110) == 0b110){
        // UNSEAL fule
        WriteReg(REG_GAUGE_CTRL_L, 0x14, 0x04);
        delay_ms(50);
        WriteReg(REG_GAUGE_CTRL_L, 0x72, 0x36);
        delay_ms(50);
    }
    if ((operation_status_lsb & 0b110) != 0b010){
        // FULL ACCESS
        WriteReg(REG_GAUGE_CTRL_L, 0xFF, 0xFF);
        delay_ms(50);
        WriteReg(REG_GAUGE_CTRL_L, 0xFF, 0xFF);
        delay_ms(50);
    }
    // ENTER CFG UPDATE
    WriteReg(REG_GAUGE_CTRL_L, 0x90, 0x00);
    delay_ms(200);
    // wait FUEL enter Config mode
    while (1){
        uint8_t status = ReadReg(REG_OPERATION_STATUS_H);
        if ((status & 0b100) == 0b100)
            break;
        delay_ms(100);
    }

    uint8_t dc_msb = (uint8_t)(design_capacity >> 8);
    uint8_t dc_lsb = (uint8_t)(design_capacity & 0xFF);
    
    uint8_t fcc_msb = (uint8_t)(full_charge_capacity >> 8);
    uint8_t fcc_lsb = (uint8_t)(full_charge_capacity & 0xFF);

    // Write Design Capacity
    WriteReg(REG_DATA_MEMORY_L, 0x9F, 0x92);
    delay_ms(I2C_WRITE_DELAY_MS);

    WriteReg(REG_MAC_DATA, dc_msb, dc_lsb);
    // Write checksum
    uint8_t checksum = 0xFF - (0x9F + 0x92 + dc_msb + dc_lsb) & 0xFF;
    WriteReg(REG_MAC_CHECK_SUM, checksum);
    // DataLen fixed 0x06 [0x9f, 0x92, dc_msb, dc_lsb, check_sum, datalen]
    WriteReg(REG_MAC_DATA_LEN, 0x06);
    delay_ms(200);

    // Write Full Chaeging Capacity
    WriteReg(REG_DATA_MEMORY_L, 0x9D, 0x92);
    delay_ms(I2C_WRITE_DELAY_MS);
    WriteReg(REG_MAC_DATA, fcc_msb, fcc_lsb);
    // Write checksum
    checksum = 0xFF - (0x9D + 0x92 + fcc_msb + fcc_lsb) & 0xFF;
    WriteReg(REG_MAC_CHECK_SUM, checksum);
    // DataLen fixed 0x06 [0x9d, 0x92, dc_msb, dc_lsb, check_sum, datalen]
    WriteReg(REG_MAC_DATA_LEN, 0x06);
    delay_ms(200);

    // Config finish
    WriteReg(REG_GAUGE_CTRL_L, 0x91, 0x00); // EXIT_CFG_IPDATE_REINIT
    // wait
    while (1){
        uint8_t status = ReadReg(0x3B);
        if ((status & 0b100) != 0b100)
            break;
        delay_ms(100);
    }
    // SEALED
    WriteReg(REG_GAUGE_CTRL_L, 0x30, 0x00);
	return true;
}

bool BQ27220::refreshData(void){
    uint8_t bat_status_low = ReadReg(REG_BATTERY_STATUS_L);
    uint8_t bat_status_high = ReadReg(REG_BATTERY_STATUS_H);
    should_power_off = (bat_status_low & 0b00000010) != 0;
    if ((bat_status_low & 0b00001000) == 0){ // !BATTPRES
        battery_status = NO_BATTERY;
        voltage = 0;
        current = 0;
        average_power = 0;
        time_to_empty = 0;
        time_to_full = 0;
        remaining_capacity = 0;
        fullcharge_capacity = 0;
        cycle_count = 0;
        charge_voltage = 0;
        charge_current = 0;
    }else{
        if (bat_status_low & 0b00000001){ // DISCHARGING
            if (bat_status_high & 0b10000000 || should_power_off){ // Full-discharge
                battery_status = EMPTY;
            }else{ // discharging
                battery_status = NORMAL;
            }
        }else{
            battery_status = CHARGING;
            if(bat_status_high & 0b00000010){ // full-charged
                battery_status = FULL;
            }
        }
        voltage = ReadRegU16(REG_VOLTAGE_L);
        current = (int16_t)ReadRegU16(REG_CURRENT_L);
        average_power = (int16_t)ReadRegU16(REG_AVERAGE_POWER_L);
        time_to_empty = ReadRegU16(REG_TIME_TO_EMPTY_L);
        time_to_full = ReadRegU16(REG_TIME_TO_FULL_L);
        remaining_capacity = ReadRegU16(REG_REMAINING_CAP_L);
        fullcharge_capacity = ReadRegU16(REG_FULL_CHARGE_CAP_L);
        cycle_count = ReadRegU16(REG_CYCLE_COUNT_L);
        charge_voltage = ReadRegU16(REG_CHARGE_VOLTAGE_L);
        charge_current = ReadRegU16(REG_CHARGE_CURRENT_L);
        design_capacity = ReadRegU16(REG_DESIGN_CAP_L);
    }
    return true;
}

/////// PRIVATE

void BQ27220::WriteReg(uint8_t reg, uint8_t data)
{
    I2C_BEFORE_TRANS
    Wire.beginTransmission(BQ27220_ADDR);
    Wire.write(reg);
    Wire.write(data);
    Wire.endTransmission();
    I2C_AFTER_TRANS
    delay_us(I2C_READ_DELAY_US);
}

void BQ27220::WriteReg(uint8_t reg, uint8_t data, uint8_t data2)
{
    I2C_BEFORE_TRANS
    Wire.beginTransmission(BQ27220_ADDR);
    Wire.write(reg);
    Wire.write(data);
    Wire.write(data2);
    Wire.endTransmission();
    I2C_AFTER_TRANS
    delay_us(I2C_READ_DELAY_US);
    // BQ27220::WriteReg(reg, data);
    // BQ27220::WriteReg(reg + 1, data2);
}

uint8_t BQ27220::ReadReg(uint8_t reg)
{
    I2C_BEFORE_TRANS
    Wire.beginTransmission(BQ27220_ADDR);
    Wire.write(reg);
    Wire.endTransmission();
    delay_us(I2C_READ_DELAY_US);

    Wire.requestFrom(BQ27220_ADDR, 1);
    while(Wire.available() < 1) ; 
    uint8_t data = Wire.read();
    I2C_AFTER_TRANS
    delay_us(I2C_READ_DELAY_US);
    return data;

}
uint16_t BQ27220::ReadRegU16(uint8_t reg)
{
    I2C_BEFORE_TRANS
    Wire.beginTransmission(BQ27220_ADDR);
    Wire.write(reg);
    Wire.endTransmission();
    delay_us(I2C_READ_DELAY_US);

    Wire.requestFrom(BQ27220_ADDR, 2);
    while (Wire.available() < 2);
    uint8_t low = Wire.read();
    uint8_t high = Wire.read();
    
    I2C_AFTER_TRANS
    
    delay_us(I2C_READ_DELAY_US);
    return u8_to_u16(high, low);
}


bool BQ27220::ReadIncremental(uint8_t reg, uint8_t* buf, size_t size)
{
    I2C_BEFORE_TRANS
    
    Wire.beginTransmission(BQ27220_ADDR);
    Wire.write(reg);
    Wire.endTransmission();
    delay_us(I2C_READ_DELAY_US);

    Wire.requestFrom(BQ27220_ADDR, size);

    for(int i = 0; i < size; i++)
    {
        while(Wire.available() < 1) ; 
        buf[i] = Wire.read();
    }

    I2C_AFTER_TRANS
    delay_us(I2C_READ_DELAY_US);
    return true;
}
