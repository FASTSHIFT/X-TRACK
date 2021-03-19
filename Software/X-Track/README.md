# Arduino-For-Keil AT32F4xx
> https://github.com/FASTSHIFT/Arduino-For-Keil

# 更新日志
## [v1.0] - 2019.11.10
* 1.基于AT32F4xx标准外设库(1.2.0版)，移植了全部的ArduinoAPI

## [v1.1] - 2019.11.14
* 1.更新timer库，更精确的定时频率

## [v1.2] - 2019.11.23
* 1.修复定时器库频率不对的BUG

## [v1.3] - 2019.12.1
* 1.更新timer库，使用独立的开根号运算函数，不依赖math.h库

## [v1.4] - 2019.12.6
* 1.工程框架修改，工程文件与代码文件分离
* 2.使Core里的一些函数命名更加规范更贴近标准库
* 3.添加了GNU支持

## [v1.5] - 2020.1.15
* 1.由于Print库的printf不支持浮点输出，将Print库还原，使用vsnprintf转换printf 

## [v1.6] - 2020.4.10
* 1.更新日志改为使用Markdown语法编写
* 2.去除重复的dtostrf.c、dtostrf.h
* 3.添加Libraries路径
* 4.Arduino.h添加时钟周期与时间转换的宏
* 5.Arduino.h添加WMath.h与Tone.h
* 6.添加README文件组
* 7.添加yield()函数实体

## [v1.7] - 2020.4.12
* 1.更新标准库AT32F4xx_DFP.1.3.1.pack
* 2.更新adc、gpio、timer库
* 3.去除工程的startup，使用pack内置的startup_xxx.s文件
* 4.去除全局芯片宏定义

## [v1.8] - 2020.4.24
* 1.去除FileGroup.h
* 2.GPIO快速操作宏定义函数从mcu_type.h转移到gpio.h和Arduino.h
* 3.__STM32__宏定义从Arduino.h转移到mcu_type.h
* 4.delay.c去除#include "Arduino.h"

## [v1.9] - 2020.5.17
* 1.补全输出捕获配置参数，PA8无法输出PWM的BUG已解决

## [v2.0] - 2020.10.11
* 1.修复ADC不工作的问题
* 2.添加硬件I2C库
* 3.添加RTC库
* 4.在Pin_TypeDef中添加GPIOF宏开关

## [v2.1] - 2020.11.4
* 1.更新标准库AT32F4xx_DFP.1.3.4.pack
* 2.更新硬件i2c库

## [v2.2] - 2021.1.23
* 1.Servo库适配
* 2.NVIC_SetPriorityGrouping -> NVIC_PriorityGroupConfig
* 3.添加extend_SRAM
* 4.Wire库FULL_SPEED_I2C修改
