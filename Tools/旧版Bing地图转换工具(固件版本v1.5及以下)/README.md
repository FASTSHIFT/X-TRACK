# 地图下载说明

## 1.选择要下载的地图区域
* 1.在`MapDownloadGen`文件夹里打开`maptile.exe`执行文件，打开后根据你自己所在地区勾选范围（注意范围不要选太广，否则地图文件会**很大**），鼠标左键单击勾选范围，选定后双击左键。点击确认后将地图保存在 X-TRACK 目录下。

https://user-images.githubusercontent.com/33819388/120956796-f0893100-c786-11eb-9e0b-aac6e733e459.mp4

## 2.下载地图

* 打开`Downloader.exe`执行文件，将上一步保存的`MapInfos.xmp`拖入程序里，点击**Download**

https://user-images.githubusercontent.com/33819388/120956786-ea935000-c786-11eb-9e22-5ec11aec1194.mp4


### ＳＴＡＴＵＳ提示信息：

|  状态       | 含义        |
|  ----     | ----         |
|   S:E     |   文件已存在                   |
|   S:F     |   文件保存失败                  |
|   C:数字    |   网络问题，数字为HTTP状态码     |
|   L:0     |   下载下来的文件长度为零         |


## 3.转换地图文件
* 下载完成后，使用`Tools`文件夹下的`TilesConverterForLVGL.exe`进行转换（[使用教程](https://github.com/FASTSHIFT/X-TRACK/tree/main/Images/MapConverter%E4%BD%BF%E7%94%A8%E6%95%99%E7%A8%8B)）
![image](https://github.com/FASTSHIFT/X-TRACK/blob/main/Images/MapConverter%E4%BD%BF%E7%94%A8%E6%95%99%E7%A8%8B/1.png)
![image](https://github.com/FASTSHIFT/X-TRACK/blob/main/Images/MapConverter%E4%BD%BF%E7%94%A8%E6%95%99%E7%A8%8B/2.png)
![image](https://github.com/FASTSHIFT/X-TRACK/blob/main/Images/MapConverter%E4%BD%BF%E7%94%A8%E6%95%99%E7%A8%8B/3.png)
![image](https://github.com/FASTSHIFT/X-TRACK/blob/main/Images/MapConverter%E4%BD%BF%E7%94%A8%E6%95%99%E7%A8%8B/4.png)
![image](https://github.com/FASTSHIFT/X-TRACK/blob/main/Images/MapConverter%E4%BD%BF%E7%94%A8%E6%95%99%E7%A8%8B/5.png)
![image](https://github.com/FASTSHIFT/X-TRACK/blob/main/Images/MapConverter%E4%BD%BF%E7%94%A8%E6%95%99%E7%A8%8B/6.png)
![image](https://github.com/FASTSHIFT/X-TRACK/blob/main/Images/MapConverter%E4%BD%BF%E7%94%A8%E6%95%99%E7%A8%8B/7.png)

## 4.批量重命名地图文件
* 转换完成后，使用`Tools`文件夹下`RenameBin.bat`脚本批量重命名地图文件。

### 上述步骤无误后，便可将MAP文件夹拷贝到SD卡里
   
