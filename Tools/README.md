# 地图下载说明

## 1.下载安装[Crimson/地图下载器](https://gitee.com/CrimsonHu/java_map_download/)

![软件打开](https://user-images.githubusercontent.com/33819388/128734201-f32f39e7-25d6-48ec-a851-99291c4a27a6.png)

## 2.选择要下载的地图区域
 * 选择自己所在区域下载即可，**切勿选择过大范围**，下载区域设置小一些后续下载会快很多。
![选择下载区域](https://user-images.githubusercontent.com/33819388/128734302-1abab0a1-b04f-42d6-8c7d-639cd4b2377d.png)
## 3.下载地图
 * 下载地图等级根据自己喜好选择，等级越高地图越详细，但是文件也越大（建议不要超过18级）。命名格式为`/z/x/y.png`格式，保存路径新建一个`MAP`文件夹
![添加任务](https://user-images.githubusercontent.com/33819388/128734395-4e5081b4-1254-4524-b517-c1e273003e07.png)
![下载地图](https://user-images.githubusercontent.com/33819388/128734446-459ab519-bdbf-45ba-aec8-ff8c48a3d73c.png)

## 4.转换地图文件
* 下载完成后，使用`Tools`文件夹下的`TilesConverterForLVGL.exe`进行转换（[使用教程](https://github.com/FASTSHIFT/X-TRACK/tree/main/Images/MapConverter%E4%BD%BF%E7%94%A8%E6%95%99%E7%A8%8B)）
![image](https://github.com/FASTSHIFT/X-TRACK/blob/main/Images/MapConverter%E4%BD%BF%E7%94%A8%E6%95%99%E7%A8%8B/1.png)
![image](https://github.com/FASTSHIFT/X-TRACK/blob/main/Images/MapConverter%E4%BD%BF%E7%94%A8%E6%95%99%E7%A8%8B/2.png)
![image](https://github.com/FASTSHIFT/X-TRACK/blob/main/Images/MapConverter%E4%BD%BF%E7%94%A8%E6%95%99%E7%A8%8B/3.png)
![image](https://github.com/FASTSHIFT/X-TRACK/blob/main/Images/MapConverter%E4%BD%BF%E7%94%A8%E6%95%99%E7%A8%8B/4.png)
![image](https://github.com/FASTSHIFT/X-TRACK/blob/main/Images/MapConverter%E4%BD%BF%E7%94%A8%E6%95%99%E7%A8%8B/5.png)
![image](https://github.com/FASTSHIFT/X-TRACK/blob/main/Images/MapConverter%E4%BD%BF%E7%94%A8%E6%95%99%E7%A8%8B/6.png)
![image](https://github.com/FASTSHIFT/X-TRACK/blob/main/Images/MapConverter%E4%BD%BF%E7%94%A8%E6%95%99%E7%A8%8B/7.png)

## 5.拷贝地图文件至SD卡根目录
### 注意：
 * SD卡需要格式化为**FAT32格式**。
 * SD卡插入开机以后，会自动创建`SystemSave.json`文件，其中：
 > 通过`"mapDirPath"`设置地图源文件夹，默认路径为`"/MAP"`（SD卡可以储存多个地图源，通过此选项切换）
 > 
 > 通过`"mapWGS84"`设置坐标系统，如果发现地图偏移请尝试修改。
 * `SystemSave.json`文件详细说明见[编译说明](https://github.com/FASTSHIFT/X-TRACK/blob/main/Software/README.md)。
