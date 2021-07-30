## 编译
* 参见: [编译说明](https://github.com/FASTSHIFT/X-TRACK/blob/main/Software/README.md)

## 硬件修改
* 去除**D4**二极管，防止充电时自动开机，导致充电变慢和发热
* GPS模块需要由**HT1818Z3G5L**更换为**ATGM336H**(购买链接在下方)，否则外壳放不下
* 地磁计**LIS3MDL**和加速度计**LSM6DSM**在该系统中作用不大，又由于芯片涨价导致整机成本过高，可以不安装，但需要在源代码的**Config.h**中的**CONFIG_SENSOR_ENABLE**宏开关设置0，并重新编译固件
* MCU: AT32F403ACGU7如果定不到货，可以尝试[@johnsbark](https://github.com/johnsbark)设计的[改进版硬件](https://github.com/FASTSHIFT/X-TRACK/tree/main/Hardware/X-TRACK%20%E6%94%B9%E8%BF%9B%E7%89%88)方案（红框内的MCU都可以进行尝试）
![_@~ (1`XC( S{MQFDXTSZQQ](https://user-images.githubusercontent.com/26767803/126936789-e303282b-f0cd-4758-b713-bd6ed797a0aa.png)


## 特殊器件购买链接
* 电池 (选择 683030) https://detail.tmall.com/item.htm?id=634173891714
* 旋转编码器 https://item.taobao.com/item.htm?id=598028191617
* 屏幕 (选择 裸屏) https://item.taobao.com/item.htm?id=600653923469
* 调试口 (选择 4P弯针) https://item.taobao.com/item.htm?id=522574977704
* 码表底座 (选择 竖向按装拓展片孔距M3*20~21mm[塑胶材料 3g]) https://item.taobao.com/item.htm?id=599529362799
* GPS模块 (ATGM336H) https://item.taobao.com/item.htm?&id=603186739934
* Pad (非实体元件，只是焊接电池线的焊盘)
* AON3400 (AO3400 N沟道MOS管)

## 焊接建议
* 不建议新手尝试自己焊接
* SD卡座、旋转编码器、蜂鸣器建议用细尖头烙铁焊接，不容易烫坏元件
