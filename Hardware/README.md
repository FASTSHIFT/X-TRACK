## 编译
* 参见: [编译说明](https://github.com/FASTSHIFT/X-TRACK/blob/main/Software/README.md)

## 硬件修改
* PCB打样的厚度选择**0.8mm**或**1mm**。
* 去除**D4**二极管，防止充电时自动开机，导致充电变慢和发热。
* 注意**D2**二极管方向，方向是和其他二极管相反的。否则会发生开机一段时间自动关机的情况。
* GPS模块需要由**HT1818Z3G5L**更换为**ATGM336H**(购买链接在下方)，否则外壳放不下。关于接线，GPS模块的**TX**脚接板子的**T焊盘**,**RX**脚接板子的**R焊盘**。注意GPS的天线**不要**碰到旁边的法拉电容，否则导致模块发热且收不到信号。
* 地磁计**LIS3MDL**和加速度计**LSM6DSM**在该系统中作用不大，又由于芯片涨价导致整机成本过高，可以不安装。
* MCU:**AT32F435CGU7**或**AT32F403ACGU7**
如果定不到货，可以尝试[@johnsbark](https://github.com/johnsbark)设计的[改进版硬件](https://github.com/FASTSHIFT/X-TRACK/tree/main/Hardware/X-TRACK%20%E6%94%B9%E8%BF%9B%E7%89%88)方案（红框内的MCU都可以进行尝试）。

![image](https://github.com/FASTSHIFT/X-TRACK/blob/main/Images/MCU%E9%80%89%E5%9E%8B/AT32F435.png)
![image](https://github.com/FASTSHIFT/X-TRACK/blob/main/Images/MCU%E9%80%89%E5%9E%8B/AT32F403A.png)

* 关于其他品牌MCU替换的性能需求: 主频 >150M，RAM >=200K，FLASH >=512K，SPI速度 >=50Mbps。 

## 特殊器件购买链接
* 电池 (选择 683030) https://detail.tmall.com/item.htm?id=634173891714
* 旋转编码器 https://item.taobao.com/item.htm?id=598028191617
* 屏幕 (选择 裸屏) https://item.taobao.com/item.htm?id=600653923469
* 调试口 (选择 4P弯针) https://item.taobao.com/item.htm?id=522574977704
* 码表底座 (选择 竖向按装拓展片孔距M3*20~21mm[塑胶材料 3g]) https://item.taobao.com/item.htm?id=599529362799
* GPS模块 (ATGM336H) https://item.taobao.com/item.htm?id=603186739934
* AON3400 (AO3400 N沟道MOS管) https://item.taobao.com/item.htm?id=522574089119
* 侧盖螺丝 https://item.taobao.com/item.htm?id=522574685586
* Pad (焊接电池线的焊盘，非实体元件)

## 焊接
* [焊接教程](https://github.com/FASTSHIFT/X-TRACK/blob/main/Hardware/%E7%84%8A%E6%8E%A5%E8%B0%83%E8%AF%95%E6%8C%87%E5%8C%97.md)

## 常见故障排查
> GPS连不上
* 到开阔地带进行定位
* 进入系统信息页面查看GPS栏是否能获取到UTC时间
* 如果一直无法获取UTC时间，则检查GPS模块和MCU的硬件连接，确保通信波特率为9600
* 检查GPS天线朝向，参考下图

![image](https://github.com/FASTSHIFT/X-TRACK/blob/main/Images/%E5%86%85%E9%83%A8%E8%A3%85%E9%85%8D%E5%9B%BE%E5%8F%82%E8%80%83.jpg)

> 地图不显示
* 在首次定位成功前，默认加载的地图区域是北京，等待定位成功或者修改**SystemSave.json**里的经纬度配置
* 检查状态栏是否有SD卡图标
* 检查地图文件路径是否为`/MAP/z/x/y.bin`
* **不要使用旧版地图工具**

> 不开机 & 关不了机
* 长按编码器开关机
* 检查供电是否正常，MCU和其他器件上的电压是否是3.3V

> 开机了但黑屏
* 检查屏幕排线和MCU间是否短路或虚焊
* 检查背光是否有电压

> SD卡无法读取
* 在系统信息页面查看"Storage"项，是否能正常读取到SD卡容量和类型
* 检查SD卡座和MCU间是否有短路或虚焊
* 检查SD卡格式是否为FAT32

> SD卡内没有生成**SystemSave.json**配置文件
* **SystemSave.json**在第一次关机后自动生成

> 地图坐标有恒定偏移
* 尝试修改SD卡根目录下的**SystemSave.json**里的**useWGS84**选项[配置地图坐标系统](https://github.com/FASTSHIFT/X-TRACK/tree/main/Software#%E7%B3%BB%E7%BB%9F%E9%85%8D%E7%BD%AE%E6%96%87%E4%BB%B6)

> 速度不准
* 不同厂家的GPS模块性能不一，测得的速度可能会有小幅度跳动(<±2KM/h)
* 检查GPS天线朝向，参考第一点
* 更换GPS模块

> PC模拟器上无法显示地图/显示颜色异常
* MAP地图文件夹放在X-TRACK工程的根目录`~/X-TRACK/MAP/`
* 修改**SystemSave.json**里的经纬度配置到希望显示地图的地方
* 确认生成的地图文件命名格式为`/z/x/y.png`
* 修改**SystemSave.json**的`"sysConfig.mapExtName"`项目，将`"bin"`改为`"png"`

## 2.0寸屏版本
https://github.com/FASTSHIFT/X-TRACK/tree/2.0-inch by[@HanfG](https://github.com/HanfG)
