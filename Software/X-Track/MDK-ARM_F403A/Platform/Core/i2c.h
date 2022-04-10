/*
 * MIT License
 * Copyright (c) 2019 _VIFEXTech
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef __HARDWAREI2C_H
#define __HARDWAREI2C_H

#include <stdint.h>
#include "mcu_type.h"

void I2Cx_Init(I2C_Type *I2Cx, uint32_t baudRate);
void I2Cx_Read(I2C_Type *I2Cx, uint8_t slaveAddr, uint8_t regAddr, void *buf, uint32_t length);
void I2Cx_Write(I2C_Type *I2Cx, uint8_t slaveAddr, uint8_t regAddr, void *buf, uint32_t length);
uint8_t I2Cx_ReadReg(I2C_Type *I2Cx, uint8_t slaveAddr, uint8_t regAddr);
void I2Cx_WriteReg(I2C_Type *I2Cx, uint8_t slaveAddr, uint8_t regAddr, uint8_t value);

#endif

