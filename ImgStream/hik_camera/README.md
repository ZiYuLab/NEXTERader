# 海康相机调用

```cpp
#include "HikCam.hpp"

class HikCam Hik;
Hik.StartDevice(0)//打开0号相机
Hik.SetResolution(1280, 1024);//设置分辨率
Hik.SetPixelFormat(17301512);//设置像素格式PixelType_Gvsp_BayerGR8
Hik.SetExposureTime(3000);//设置曝光时间
Hik.SetGAIN(10.0);//设置增益
Hik.SetFrameRate(120);//设置帧率上限
Hik.SetStreamOn();//开始取流

Hik.GetMat(img); //获得图像