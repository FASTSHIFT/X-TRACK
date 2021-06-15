#include "LSM6DSM.h"
#include "Wire.h"
#include <math.h>

//Register map
#define FUNC_CFG_ACCESS         0x01
#define SENSOR_SYNC_TIME_FRAME  0x04
#define SENSOR_SYNC_RES_RATIO   0x05
#define FIFO_CTRL1              0x06
#define FIFO_CTRL2              0x07
#define FIFO_CTRL3              0x08
#define FIFO_CTRL4              0x09
#define FIFO_CTRL5              0x0A
#define DRDY_PULSE_CFG          0x0B
#define INT1_CTRL               0x0D
#define INT2_CTRL               0x0E
#define WHO_AM_I                0x0F
#define CTRL1_XL                0x10
#define CTRL2_G                 0x11
#define CTRL3_C                 0x12
#define CTRL4_C                 0x13
#define CTRL5_C                 0x14
#define CTRL6_C                 0x15
#define CTRL7_G                 0x16
#define CTRL8_XL                0x17
#define CTRL9_XL                0x18
#define CTRL10_C                0x19
#define MASTER_CONFIG           0x1A
#define WAKE_UP_SRC             0x1B
#define TAP_SRC                 0x1C
#define D6D_SRC                 0x1D
#define STATUS_REG              0x1E
#define STATUS_SPIAux           0x1E
#define OUT_TEMP_L              0x20
#define OUT_TEMP_H              0x21
#define OUTX_L_G                0x22
#define OUTX_H_G                0x23
#define OUTY_L_G                0x24
#define OUTY_H_G                0x25
#define OUTZ_L_G                0x26
#define OUTZ_H_G                0x27
#define OUTX_L_XL               0x28
#define OUTX_H_XL               0x29
#define OUTY_L_XL               0x2A
#define OUTY_H_XL               0x2B
#define OUTZ_L_XL               0x2C
#define OUTZ_H_XL               0x2D
/*skip sensorhub secion1
#define ........
.
.
.
#define ........
*/
#define FIFO_STATUS1            0x3A
#define FIFO_STATUS2            0x3B
#define FIFO_STATUS3            0x3C
#define FIFO_STATUS4            0x3D
#define FIFO_DATA_OUT_L         0x3E
#define FIFO_DATA_OUT_H         0x3F
#define TIMESTAMP0_REG          0x40
#define TIMESTAMP1_REG          0x41
#define TIMESTAMP2_REG          0x42
#define STEP_TIMESTAMP_L        0x49
#define STEP_TIMESTAMP_H        0x4A
#define STEP_COUNTER_L          0x4B
#define STEP_COUNTER_H          0x4C
/*skip sensorhub secion2
#define ........
.
.
.
#define ........
*/
#define FUNC_SRC1               0x53
#define FUNC_SRC2               0x54
#define WRIST_TILT_IA           0x55
#define TAP_CFG                 0x58
#define TAP_THS_6D              0x59
#define INT_DUR2                0x5A
#define WAKE_UP_THS             0x5B
#define WAKE_UP_DUR             0x5C
#define FREE_FALL               0x5D
#define MD1_CFG                 0x5E
#define MD2_CFG                 0x5F
#define MASTER_CMD_CODE         0x60
#define SENS_SYNC_SPI_ERROR_CODE    0x61
#define OUT_MAG_RAW_X_L         0x66
#define OUT_MAG_RAW_X_H         0x67
#define OUT_MAG_RAW_Y_L         0x68
#define OUT_MAG_RAW_Y_H         0x69
#define OUT_MAG_RAW_Z_L         0x6A
#define OUT_MAG_RAW_Z_H         0x6B
#define INT_OIS                 0x6F
#define CTRL1_OIS               0x70
#define CTRL2_OIS               0x71
#define CTRL3_OIS               0x72
#define X_OFS_USR               0x73
#define Y_OFS_USR               0x74
#define Z_OFS_USR               0x75
//Accessing the registers blow needs to write 0xa0 to FUNC_CFG_ACCESS
//Embedded function registers(bank A)
#define CONFIG_PEDO_THS_MIN     0x0F
#define SM_THS                  0x13
#define PEDO_DEB_REG            0x14
#define STEP_COUNT_DELTA        0x15
//Embedded function registers(bank B)
#define A_WRIST_TILT_LAT        0x50
#define A_WRIST_TILT_THS        0x54
#define A_WRIST_TILT_Mask       0x59

#define ACC_POWER_DOWN          (0x00 << 4)
#define ACC_ODR_1_6_HZ          (0x0b << 4)
#define ACC_ODR_12_5_HZ         (0x01 << 4)
#define ACC_ODR_26_HZ           (0x02 << 4)
#define ACC_ODR_52_HZ           (0x03 << 4)
#define ACC_ODR_104_HZ          (0x04 << 4)
#define ACC_ODR_208_HZ          (0x05 << 4)
#define ACC_ODR_416_HZ          (0x06 << 4)
#define ACC_ODR_833_HZ          (0x07 << 4)
#define ACC_ODR_1_66_KHZ        (0x08 << 4)
#define ACC_ODR_3_33_KHZ        (0x09 << 4)
#define ACC_ODR_6_66_KHZ        (0x0a << 4)
#define ACC_SCALE_2_G           (0x00 << 2)
#define ACC_SCALE_16_G          (0x01 << 2)
#define ACC_SCALE_4_G           (0x02 << 2)
#define ACC_SCALE_8_G           (0x03 << 2)

#define GYR_POWER_DOWN          (0x00 << 4)
#define GYR_ODR_12_5_HZ         (0x01 << 4)
#define GYR_ODR_26_HZ           (0x02 << 4)
#define GYR_ODR_52_HZ           (0x03 << 4)
#define GYR_ODR_104_HZ          (0x04 << 4)
#define GYR_ODR_208_HZ          (0x05 << 4)
#define GYR_ODR_416_HZ          (0x06 << 4)
#define GYR_ODR_833_HZ          (0x07 << 4)
#define GYR_ODR_1_66_KHZ        (0x08 << 4)
#define GYR_ODR_3_33_KHZ        (0x09 << 4)
#define GYR_ODR_6_66_KHZ        (0x01 << 4)
#define GYR_SCALE_250_DPS       (0x00 << 2)
#define GYR_SCALE_500_DPS       (0x01 << 2)
#define GYR_SCALE_1000_DPS      (0x02 << 2)
#define GYR_SCALE_2000_DPS      (0x03 << 2)

bool LSM6DSM::Init(uint8_t addr)
{
    Address = addr;

    SoftReset();
    delay(50);

    ConfigAcc(ACC_ODR_52_HZ, ACC_SCALE_4_G);
    ConfigGyr(GYR_POWER_DOWN, GYR_SCALE_500_DPS);
    //LSM6DSMSetScale(ACC_SCALE_4_G, GYR_SCALE_500_DPS);
    //LSM6DSMSetODR(ACC_ODR_416_HZ, GYR_POWER_DOWN);
    //SetRegisterBits(CTRL3_C, 0x20, true);    //interrupt output pads active low
    //WriteReg(CTRL7_G,     0x04);                  //Source register rounding function enable
    EnableAWT(10, 100);                          //20 degree, 400ms
    EnableTapDetection();
    EnablePedometer(1040, 6);                //debounce time = 1040ms, debounce step = 6 steps
    EnableEMbeddedFunc();
    return IsConnected();
}

bool LSM6DSM::IsConnected()
{
    return (ReadReg(WHO_AM_I) == 0x6A);
}

void LSM6DSM::ConfigAcc(uint8_t acc_odr, uint8_t acc_scale)
{
    WriteReg(CTRL1_XL, acc_odr | acc_scale);
}

void LSM6DSM::ConfigGyr(uint8_t gyr_odr, uint8_t gyr_scale)
{
    WriteReg(CTRL2_G, gyr_odr | gyr_scale);
}

void LSM6DSM::GetMotion6(
    int16_t* acc_x,
    int16_t* acc_y,
    int16_t* acc_z,
    int16_t* gyr_x,
    int16_t* gyr_y,
    int16_t* gyr_z
)
{
    uint8_t buf[12];
    ReadRegs(OUTX_L_G, buf, sizeof(buf));

    *gyr_x = (buf[3] << 8) | buf[2];
    *gyr_y = -(buf[1] << 8) | buf[0];
    *gyr_z = (buf[5] << 8) | buf[4];
    *acc_x = (buf[9] << 8) | buf[8];
    *acc_y = -(buf[7] << 8) | buf[6];
    *acc_z = (buf[11] << 8) | buf[10];
}

int16_t LSM6DSM::GetTemperature(void)
{
    uint8_t buf[2];
    ReadRegs(OUT_TEMP_L, buf, sizeof(buf));
    return buf[1] << 8 | buf[0];
}

void LSM6DSM::SoftReset(void)
{
    SetRegisterBits(CTRL3_C, 0x01, true);
}

void LSM6DSM::EnableEMbeddedFunc(void)
{
    SetRegisterBits(CTRL10_C, 0x04, true);     // Enable embedded functions
}

void LSM6DSM::DisableEMbeddedFunc(void)
{
    SetRegisterBits(CTRL10_C, 0x04, false);     // Disable embedded functions
}

void LSM6DSM::EnableAWT(int16_t angle, int16_t delay)
{
    int16_t set_delay;
    int16_t set_angle;
    set_delay = (uint8_t)(delay / 40);
    set_angle = (uint8_t)(sin(angle * PI / 180) * 64);

    if(set_delay > 0xff)
        set_delay = 0xff;
    if(set_angle > 0xff)
        set_angle = 0xff;

    WriteReg(FUNC_CFG_ACCESS,   0xa0);              // Enable access to embedded registers (bank B)
    WriteReg(A_WRIST_TILT_LAT,  set_delay);     // Set new latency in A_WRIST_TILT_LAT
    WriteReg(A_WRIST_TILT_THS,  set_angle);     // Set new threshold in A_WRIST_TILT_THS
    WriteReg(A_WRIST_TILT_Mask, 0x40);              // Set new mask in A_WRIST_TILT_Mask
    WriteReg(FUNC_CFG_ACCESS,   0x00);              // Disable access to embedded registers (bank B)
    SetRegisterBits(CTRL10_C, 0x80, true);             // Enable embedded functions(0x04)
    // Enable AWT detection (0x80)
    SetRegisterBits(DRDY_PULSE_CFG, 0x01, true);   // AWT interrupt driven to INT2 pin
}

void LSM6DSM::DisableAWT(void)
{
    SetRegisterBits(CTRL10_C, 0x80, false);             // Disable AWT detection (0x80)
    SetRegisterBits(DRDY_PULSE_CFG, 0x01, false);   // AWT interrupt disconnect to INT2 pin
}

void LSM6DSM::EnableTapDetection(void)
{
    SetRegisterBits(TAP_CFG, 0x82, true);      // Enable interrupts(0x80) and tap detection on X-axis(0x08), Y-axis(0x04), Z-axis(0x02)
    SetRegisterBits(TAP_THS_6D, 0x8c, true);   // Set tap threshold(LSB0 - MSB4, dafult value 00000)
    WriteReg(INT_DUR2, 0x7f);                           // Set Duration, Quiet and Shock time windows
    SetRegisterBits(WAKE_UP_THS, 0x80, true);// Single & double-tap enabled (0x80)SINGLE_DOUBLE_TAP = 1, if = 0, SINGLE_TAP only)
    SetRegisterBits(MD1_CFG, 0x08, true);      // (0x40)Single-tap interrupt driven to INT1 pin
    // (0x08)Double-tap interrupt driven to INT1 pin
    //SetRegisterBits(MD2_CFG, 0x08, true);    // (0x40)Single-tap interrupt driven to INT2 pin
    // (0x08)Double-tap interrupt driven to INT2 pin
}

void LSM6DSM::DisableTapDetection(void)
{
    SetRegisterBits(TAP_CFG, 0x08 | 0x04 | 0x02, false); //tap detection on X-axis(0x08), Y-axis(0x04), Z-axis(0x02)
}

void LSM6DSM::EnablePedometer(uint16_t debounce_time, uint8_t debounce_step)
{
    WriteReg(FUNC_CFG_ACCESS, 0x80);                            // Enable access to embedded functions registers (bank A)
    WriteReg(CONFIG_PEDO_THS_MIN, 0x8e);                    // PEDO_FS = ¡À4 g and configure pedometer minimum threshold value
    WriteReg(PEDO_DEB_REG, ((uint8_t)(debounce_time / 80) << 3) | (debounce_step & 0x07));
    WriteReg(FUNC_CFG_ACCESS, 0x00);                            // Disable access to embedded functions registers (bank A)
    SetRegisterBits(CTRL10_C, 0x10, true);                     // Enable embedded functions and pedometer algorithm
    //SetRegisterBits(INT1_CTRL, 0x80, true);                  // Step detector interrupt driven to INT1 pin
}

uint16_t LSM6DSM::GetCurrentStep(void)
{
    uint8_t tempL, tempH;
    tempL = ReadReg(STEP_COUNTER_L);
    tempH = ReadReg(STEP_COUNTER_H);
    return ((tempH << 8) | tempL);
}

void LSM6DSM::ResetStepCounter(void)
{
    SetRegisterBits(CTRL10_C, 0x02, true);
    delay(1);
    SetRegisterBits(CTRL10_C, 0x02, false);
}

void LSM6DSM::DisablePedometer(void)
{
    SetRegisterBits(CTRL10_C, 0x10, false);                     // Disable pedometer algorithm
}

void LSM6DSM::WriteReg(uint8_t reg, uint8_t data)
{
    Wire.beginTransmission(Address);
    Wire.write(reg);
    Wire.write(data);
    Wire.endTransmission();
}

uint8_t LSM6DSM::ReadReg(uint8_t reg)
{
    Wire.beginTransmission(Address);
    Wire.write(reg);
    Wire.endTransmission();

    Wire.requestFrom(Address, 1);
    uint8_t data = Wire.read();
    Wire.endTransmission();

    return data;
}

void LSM6DSM::ReadRegs(uint8_t reg, uint8_t* buf, uint16_t len)
{
    Wire.beginTransmission(Address);
    Wire.write(reg);
    Wire.endTransmission();

    Wire.requestFrom(Address, len);
    for(int i = 0; i < len; i++)
    {
        buf[i] = Wire.read();
    }
    Wire.endTransmission();
}

void LSM6DSM::SetRegisterBits(uint8_t reg, uint8_t data, bool setBits)
{
    uint8_t val = ReadReg(reg);
    setBits ? val |= data : val &= ~data;
    WriteReg(reg, val);
}
