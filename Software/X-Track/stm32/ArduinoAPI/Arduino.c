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
#include "Arduino.h"

/**
  * @brief  配置引脚输入输出模式
  * @param  Pin: 引脚编号
  * @param  pinMode_x: 模式
  * @retval 无
  */
void pinMode(uint8_t Pin, pinMode_TypeDef pinMode_x)
{
    if(!IS_PIN(Pin))
        return;
    
    if(pinMode_x == INPUT_ANALOG_DMA)
    {
        if(!IS_ADC_PIN(Pin))
            return;
        
        pinMode(Pin, INPUT_ANALOG);
        ADC_DMA_Register(PIN_MAP[Pin].ADC_Channel);
    }
    else if(pinMode_x == PWM)
    {
        PWM_Init(Pin, 1000, 10000);
    }
    else
    {
        GPIOx_Init(
            PIN_MAP[Pin].GPIOx, 
            PIN_MAP[Pin].GPIO_Pin_x, 
            pinMode_x, 
            GPIO_Speed_100MHz
        );
    }
}

/**
  * @brief  将数字HIGH(1)或LOW(0)值写入数字引脚
  * @param  Pin:引脚编号
  * @param  val: 写入值
  * @retval 无
  */
void digitalWrite(uint8_t Pin, uint8_t val)
{
    if(!IS_PIN(Pin))
        return;
    
    val ? digitalWrite_HIGH(Pin) : digitalWrite_LOW(Pin);
}

/**
  * @brief  读取引脚电平
  * @param  Pin: 引脚编号
  * @retval 引脚电平
  */
uint8_t digitalRead(uint8_t Pin)
{
    if(!IS_PIN(Pin))
        return 0;
    
    return digitalRead_FAST(Pin);
}

/**
  * @brief  将模拟值(PWM占空比)写入引脚
  * @param  Pin: 引脚编号
  * @param  val: PWM占空比
  * @retval PWM占空比
  */
uint16_t analogWrite(uint8_t Pin, uint16_t val)
{
    if(!IS_PIN(Pin))
        return 0;
    
    return (IS_PWM_PIN(Pin) ? pwmWrite(Pin, val) : 0);
}

/**
  * @brief  从指定的模拟引脚读取值
  * @param  Pin: 引脚编号
  * @retval ADC值：0~4095
  */
uint16_t analogRead(uint8_t Pin)
{
    return (IS_ADC_PIN(Pin) ? ADCx_GetValue(PIN_MAP[Pin].ADCx, PIN_MAP[Pin].ADC_Channel) : 0);
}

/**
  * @brief  从指定的引脚读取值(DMA方式)
  * @param  Pin: 引脚编号
  * @retval ADC值：0~4095
  */
uint16_t analogRead_DMA(uint8_t Pin)
{
    if(!IS_ADC_PIN(Pin))
        return 0;
    
    return ADC_DMA_GetValue(PIN_MAP[Pin].ADC_Channel);
}

/**
  * @brief  一次移出一个字节的数据，从最左端或最小(最右边)开始
  * @param  dataPin: 输出每个位的 pin
  * @param  clockPin: 将 dataPin 设置为正确值后要切换的 pin (int)
  * @param  bitOrder: MSBFIRST / LSBFIRST
  * @param  value: 要移出的数据(字节)
  * @retval 无
  */
void shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t value)
{
    uint8_t i;
    if(!(IS_PIN(dataPin) && IS_PIN(clockPin)))
        return;
    
    digitalWrite_LOW(clockPin);
    for (i = 0; i < 8; i++)
    {
        int bit = bitOrder == LSBFIRST ? i : (7 - i);
        digitalWrite(dataPin, (value >> bit) & 0x1);
        togglePin(clockPin);
        togglePin(clockPin);
    }
}

/**
  * @brief  一次将一个字节的数据移位，从最左端或最小 (最右边) 开始
  * @param  dataPin:  输入每个位的 pin
  * @param  clockPin: 要切换到的 pin 信号从dataPin读取
  * @param  bitOrder: MSBFIRST/LSBFIRST
  * @retval 读取的值 (字节)
  */
uint32_t shiftIn(uint8_t dataPin, uint8_t clockPin, uint32_t bitOrder)
{
    uint8_t value = 0 ;
    uint8_t i ;
    
    if(!(IS_PIN(dataPin) && IS_PIN(clockPin)))
        return 0;
    
    for ( i = 0 ; i < 8 ; ++i )
    {
        digitalWrite_HIGH(clockPin) ;
        if (bitOrder == LSBFIRST )
        {
            value |= digitalRead(dataPin) << i ;
        }
        else
        {
            value |= digitalRead(dataPin) << (7 - i) ;
        }
        digitalWrite_LOW(clockPin);
    }

    return value ;
}

/**
  * @brief  将一个数字(整形)从一个范围重新映射到另一个区域
  * @param  x: 要映射的数字
  * @param  in_min: 值的当前范围的下界
  * @param  in_max: 值的当前范围的上界
  * @param  out_min: 值的目标范围的下界
  * @param  out_max: 值目标范围的上界
  * @retval 映射的值(long)
  */
long map(long x, long in_min, long in_max, long out_min, long out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

/**
  * @brief  将一个数字(浮点型)从一个范围重新映射到另一个区域
  * @param  x: 要映射的数字
  * @param  in_min: 值的当前范围的下界
  * @param  in_max: 值的当前范围的上界
  * @param  out_min: 值的目标范围的下界
  * @param  out_max: 值目标范围的上界
  * @retval 映射的值(double)
  */
double fmap(double x, double in_min, double in_max, double out_min, double out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

/**
  * @brief  在引脚上读取脉冲
  * @param  pin: 要读取脉冲的引脚编号
  * @param  value: 脉冲类型:高或低
  * @param  timeout: 等待脉冲开始的微秒数
  * @retval 脉冲的长度(以微秒计)或0, 如果没有脉冲在超时之前开始
  */
uint32_t pulseIn(uint32_t pin, uint32_t state, uint32_t timeout )
{
   // cache the IDR address and bit of the pin in order to speed up the
   // pulse width measuring loop and achieve finer resolution.  calling
   // digitalRead() instead yields much coarser resolution.
 
   __IO uint32_t *idr = portInputRegister(digitalPinToPort(pin));
   const uint32_t bit = digitalPinToBitMask(pin);
   const uint32_t stateMask = (state ? bit:0);

   uint32_t width = 0; // keep initialization out of time critical area
   
   // convert the timeout from microseconds to a number of times through
   // the initial loop; it takes 16 clock cycles per iteration.
   uint32_t numloops = 0;
   uint32_t maxloops =  timeout * ( F_CPU / 16000000);
   volatile uint32_t dummyWidth=0;
    
   if(!IS_PIN(pin))
        return 0;
   
   // wait for any previous pulse to end
   while ((*idr & bit) == stateMask)   {
      if (numloops++ == maxloops)  {
         return 0;
      }
      dummyWidth++;
   }
   
   // wait for the pulse to start
   while ((*idr & bit) != stateMask)   {
      if (numloops++ == maxloops) {
         return 0;
      }
      dummyWidth++;
   }
   
   // wait for the pulse to stop
   while ((*idr & bit) == stateMask) {
      if (numloops++ == maxloops)  {
         return 0;
      }
      width++;
   }

   // Excluding time taking up by the interrupts, it needs 16 clock cycles to look through the last while loop  
   // 5 is added as a fiddle factor to correct for interrupts etc. But ultimately this would only be accurate if it was done ona hardware timer
   
   return (uint32_t)( ( (unsigned long long)(width+5) *  (unsigned long long) 16000000.0) /(unsigned long long)F_CPU ) ;
}

void yield(void)
{
}
