# 地图下载说明

## 1.下载安装[Crimson/地图下载器](https://gitee.com/CrimsonHu/java_map_download/)

![image](https://github.com/FASTSHIFT/X-TRACK/blob/main/Images/%E5%9C%B0%E5%9B%BE%E4%B8%8B%E8%BD%BD%E6%95%99%E7%A8%8B/1-%E4%B8%8B%E8%BD%BD%E5%AE%89%E8%A3%85Crimson%E5%9C%B0%E5%9B%BE%E4%B8%8B%E8%BD%BD%E5%99%A8.png)

![image](https://github.com/FASTSHIFT/X-TRACK/blob/main/Images/%E5%9C%B0%E5%9B%BE%E4%B8%8B%E8%BD%BD%E6%95%99%E7%A8%8B/2-%E8%BF%90%E8%A1%8C%E4%B8%8B%E8%BD%BD%E5%99%A8.png)

## 2.选择要下载的地图区域
 * 选择自己所在区域下载即可，**切勿选择过大范围**，下载区域设置小一些后续下载会快很多。
![image](https://github.com/FASTSHIFT/X-TRACK/blob/main/Images/%E5%9C%B0%E5%9B%BE%E4%B8%8B%E8%BD%BD%E6%95%99%E7%A8%8B/3-%E9%80%89%E6%8B%A9%E4%B8%8B%E8%BD%BD%E5%8C%BA%E5%9F%9F.png)
## 3.下载地图
 * 下载地图等级根据自己喜好选择，等级越高地图越详细，但是文件也越大（建议只勾选**0~16级**，如果出现0~2级下载失败的情况，可以不勾选）。命名格式为`/z/x/y.png`格式，保存路径新建一个`MAP`文件夹
 
![image](https://github.com/FASTSHIFT/X-TRACK/blob/main/Images/%E5%9C%B0%E5%9B%BE%E4%B8%8B%E8%BD%BD%E6%95%99%E7%A8%8B/4-%E5%9C%B0%E5%9B%BE%E4%B8%8B%E8%BD%BD%E9%85%8D%E7%BD%AE.png)
![image](https://github.com/FASTSHIFT/X-TRACK/blob/main/Images/%E5%9C%B0%E5%9B%BE%E4%B8%8B%E8%BD%BD%E6%95%99%E7%A8%8B/5-%E4%B8%8B%E8%BD%BD%E5%AE%8C%E6%88%90.png)

## 4.转换地图文件
* 下载完成后，使用`Tools`文件夹下的`TilesConverterForLVGL.exe`进行转换（[使用教程](https://github.com/FASTSHIFT/X-TRACK/tree/main/Images/MapConverter%E4%BD%BF%E7%94%A8%E6%95%99%E7%A8%8B)）
![image](https://github.com/FASTSHIFT/X-TRACK/blob/main/Images/MapConverter%E4%BD%BF%E7%94%A8%E6%95%99%E7%A8%8B/1.png)
![image](https://github.com/FASTSHIFT/X-TRACK/blob/main/Images/MapConverter%E4%BD%BF%E7%94%A8%E6%95%99%E7%A8%8B/2.png)
![image](https://github.com/FASTSHIFT/X-TRACK/blob/main/Images/MapConverter%E4%BD%BF%E7%94%A8%E6%95%99%E7%A8%8B/3.png)
![image](https://github.com/FASTSHIFT/X-TRACK/blob/main/Images/MapConverter%E4%BD%BF%E7%94%A8%E6%95%99%E7%A8%8B/4.png)
![image](https://github.com/FASTSHIFT/X-TRACK/blob/main/Images/MapConverter%E4%BD%BF%E7%94%A8%E6%95%99%E7%A8%8B/5.png)
![image](https://github.com/FASTSHIFT/X-TRACK/blob/main/Images/MapConverter%E4%BD%BF%E7%94%A8%E6%95%99%E7%A8%8B/6.png)
![image](https://github.com/FASTSHIFT/X-TRACK/blob/main/Images/MapConverter%E4%BD%BF%E7%94%A8%E6%95%99%E7%A8%8B/7.png)

## 5.拷贝"MAP"文件夹至SD卡根目录
### 注意：
 * SD卡需要格式化为**FAT32格式**。
 * SD卡插入开机以后，会在第一次关机时自动创建`SystemSave.json`文件，其中：
 > 通过`"mapDirPath"`设置地图源文件夹，默认路径为`"/MAP"`（SD卡可以储存多个地图源，通过此选项切换）
 > 
 > 通过`"mapWGS84"`设置坐标系统，如果发现地图偏移请尝试修改。
 * `SystemSave.json`文件详细说明见[编译说明](https://github.com/FASTSHIFT/X-TRACK/blob/main/Software/README.md)。
