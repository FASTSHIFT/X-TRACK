#include "QMC6310.h"
#include <math.h>
#include "HAL/HAL_Config.h"
#include "Wire.h"

#define QMC6310_REG_OUT_X_L  0x01
#define QMC6310_REG_OUT_X_H  0x02
#define QMC6310_REG_OUT_Y_L  0x03
#define QMC6310_REG_OUT_Y_H  0x04
#define QMC6310_REG_OUT_Z_L  0x05
#define QMC6310_REG_OUT_Z_H  0x06

#define QMC6310_REG_STATUS   0x09
#define QMC6310_REG_CTRL1    0x0A
#define QMC6310_REG_CTRL2    0x0B

#define QMC6310_REG_SIGN_DEF 0x29

QMC6310::QMC6310(void)
{
    io_timeout = 0; // 0 = no timeout
    did_timeout = false;
}


// Did a timeout occur in read() since the last call to timeoutOccurred()?
bool QMC6310::timeoutOccurred()
{
    bool tmp = did_timeout;
    did_timeout = false;
    return tmp;
}

bool QMC6310::init(uint8_t addr)
{

    address = addr;
    /* Soft Reset */
    WriteReg(QMC6310_REG_CTRL2, 0b11000000);
    /* Chip is Ready?*/
    while (0b01 & ReadReg(QMC6310_REG_STATUS));
#if DO_SELF_TEST_ON_INIT
    /* Define the sign for X Y and Z axis */
    WriteReg(QMC6310_REG_SIGN_DEF, 0x06);
    /* set continuous mode */
    WriteReg(QMC6310_REG_CTRL1, 0b00000011); // 0x03
    /* check status register */
    if ((0b11 & ReadReg(QMC6310_REG_STATUS)) != 0b01)
        return false;
    /* enter self-test function */
    WriteReg(QMC6310_REG_CTRL2, 0b00000100); // 0x04
    /* Waiting 5 millisecond until measurement ends */
    delay_ms(5);
    /* Read data Register 01H ~ 06H, recording as datax2/datay2/dataz2 */
    ReadReg(QMC6310_REG_OUT_X_L);
    ReadReg(QMC6310_REG_OUT_X_H);
    ReadReg(QMC6310_REG_OUT_Y_L);
    ReadReg(QMC6310_REG_OUT_Y_H);
    ReadReg(QMC6310_REG_OUT_Z_L);
    ReadReg(QMC6310_REG_OUT_Z_H);
    /* Soft Reset */
    WriteReg(QMC6310_REG_CTRL2, 0b10000000);
#endif
    /* Switch to Suspend Mode */
    WriteReg(QMC6310_REG_CTRL1, QMC6310_SUSPEND_MODE);
    WriteReg(QMC6310_REG_CTRL1, QMC6310_NORMAL_MODE);
    /* Soft Reset */
    WriteReg(QMC6310_REG_CTRL2, 0b00001100);
    return true;
}

void QMC6310::enableDefault(void){
#ifdef USE_CON_MODE_AS_DEFAULT
    /* Define the sign for X Y and Z axis */
    WriteReg(QMC6310_REG_SIGN_DEF, 0x06);
    /* Define Set/Reset mode, with Set/Reset On, Field Range 8Guass */
    // WriteReg(QMC6310_REG_CTRL2, 0b00001000); // 0x08
    WriteReg(QMC6310_REG_CTRL2, 0b00000000); // 0x08
    /* set continuous mode */
    WriteReg(QMC6310_REG_CTRL1, 0b11000000 | QMC6310_CONTINUOUS_MODE); // 0xC3
#else
    /* Define the sign for X Y and Z axis */
    WriteReg(QMC6310_REG_SIGN_DEF, 0x06);
    /* Define Set/Reset mode, with Set/Reset On, Field Range 8Guass */
    WriteReg(QMC6310_REG_CTRL2, 0b00001000); // 0x08
    /* set normal mode, set ODR=100Hz */
    WriteReg(QMC6310_REG_CTRL1, 0b11000000 | NORMAL_MODE | QMC6310_ODR_100_HZ); // 0xCD

#endif
}


bool QMC6310::read(void){
    if ((0b11 & ReadReg(QMC6310_REG_STATUS)) != 0b01)
        return false;
    m.x = (int16_t)(((0xFFFF & (uint16_t)ReadReg(QMC6310_REG_OUT_X_H)) << 8) | ReadReg(QMC6310_REG_OUT_X_L));
    m.y = (int16_t)(((0xFFFF & (uint16_t)ReadReg(QMC6310_REG_OUT_Y_H)) << 8) | ReadReg(QMC6310_REG_OUT_Y_L));
    m.z = (int16_t)(((0xFFFF & (uint16_t)ReadReg(QMC6310_REG_OUT_Z_H)) << 8) | ReadReg(QMC6310_REG_OUT_Z_L));
	return true;
}

void QMC6310::vector_normalize(vector<float> *a)
{
    
    float mag = sqrt(vector_dot(a, a));
    a->x /= mag;
    a->y /= mag;
    a->z /= mag;
}

void QMC6310::WriteReg(uint8_t reg, uint8_t data)

{
    Wire.beginTransmission(QMC6310_ADDR);
    Wire.write(reg);
    Wire.write(data);
    Wire.endTransmission();
}

uint8_t QMC6310::ReadReg(uint8_t reg)
{
    Wire.beginTransmission(QMC6310_ADDR);
    Wire.write(reg);

    Wire.requestFrom(QMC6310_ADDR, 1);
    uint8_t data = Wire.read();
    Wire.endTransmission();

    return data;
}
