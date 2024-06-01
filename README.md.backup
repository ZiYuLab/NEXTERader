# NextE雷达站解决方案


西安理工大学NEXT-E战队雷达站 使用Yolov8 + tensorrt + 采用只使用一台相机的方案

## 版本信息
* 系统 ubuntu 22.04
* 显卡 nvidia A2000-12GB
* 海康驱动 MVS 2.1.2
* opencv 4.9.0 GPU版本（不过没有用上GPU的相关API）
* cuda 12.1
* cudnn 8.9.0
* TensorRT 8.6.1
* yaml-cpp 0.8.0
* fmt 10.1.1
* eigen 3.4.0 (sudo apt install libeigen3-dev)
* sophus 1.22.10（安装在/usr/local/include/sophus）

***

## 谢鸣
* Tensorrt-Yolov8 加速推理框架改自沈航空航天大学[Yolov5加速推理框架](https://github.com/tup-robomaster/TRTInferenceForYolo/tree/yolov5)
* 目前的权重模型和测试用的视频取自[华中科技大学狼牙战队开源雷达站](https://github.com/HUSTLYRM/HUST_Radar_2023)
* cmake文件编写和算法方面部分参考[中国石油大学华东的开源雷达站](https://github.com/AliceInOcean/RMRPSradar2023)

***

## 项目目录
```
.
├── 3rd #第三方库
│   ├── fmt
│   ├── sophus
│   ├── Sophus-1.22.10
│   └── yaml-cpp
├── CMakeLists.txt
├── Config #配置文件
│   └── config.yaml
├── DebugGui #GUI相关，为后续开发预留，目前只有一个标定类
│   ├── NEDemarcatePose.cpp
│   └── NEDemarcatePose.h
├── FindTensorRT.cmake
├── ImgStream #图像流类，用于管理图像
│   ├── hik_camera #海康相机调用，来自组长
│   ├── NEImgStream.cpp
│   └── NEImgStream.h
├── LICENSE.txt
├── main.cpp
├── NEInit.h #包含一些重要数据类型的重定义和一些宏的定义
├── Net #神经网络类
│   ├── NENet.cpp
│   ├── NENet.h
│   └── TRTInfer #修改过的沈航TRT战队Yolov5加速框架使其可适用yolov8
├── ReadConfig #读配置文件类
│   ├── NEConfig.cpp
│   └── NEConfig.h
├── README.md
├── Robot #机器人ID宏和管理机器人位置的类
│   ├── NERobot.h
│   ├── NERobotPosition.cpp
│   └── NERobotPosition.h
├── Serial #串口类
│   ├── include
│   └── src
├── SolvePosition #位置解算类
│   ├── NESolvePosition.cpp
│   └── NESolvePosition.h
└── 没了^_^
 
 ```



***

## 使用
* 配置相关环境如下，修改camkeList的相关配置
* 下载或取得相关数据（权重文件，RM地图数据）
* 将trt转为onnx
* 根据实际情况编辑填写配置文件
* 打开串口权限（见下）
* 编译项目运行
* 检查数据和进行场地标定（见下）

***

## 一些重要依赖的安装

### 先验注意
* 删除本地软件源的方法，方便nvidia包的重装 [教程](https://blog.csdn.net/weixin_45921929/article/details/126597125)***记住要删除预源文件，位于/val/***
* 如果出现了deb本地源已经添加但apt update后还没反应的，直接仿照cuda 在/etc/apt/sources.list.d/ 目录下手动添加

### 海康驱动和SDK
* [教程1](https://blog.csdn.net/qq_19449259/article/details/127725397)


### 注意，接下来安装的nvidia库对版本非常敏感，必须确定好tensorrt支持的版本后再安装cuda 和 cudnn 且cudnn 和 cuda 必须都使用同种方式安装（runfile + tar + tar 方式 或者 deb + deb + deb 方式）

### cuda的安装
* [cuda12.1下载地址](https://developer.nvidia.com/cuda-12-1-0-download-archive)

* [教程，修复应deb包内置显卡驱动版本过低导致内核版本不匹配](https://blog.csdn.net/m0_37644085/article/details/82192145)

* [教程1](https://zhuanlan.zhihu.com/p/581720480#:~:text=CUDA%E4%B8%8B%E8%BD%BD%E8%BF%9E%E6%8E%A5%EF%BC%9A%20developer.nvidia.com%2Fcu%20%E9%80%89%E6%8B%A9%E5%AF%B9%E5%BA%94%E7%9A%84%E7%89%88%E6%9C%AC%EF%BC%8C%E7%84%B6%E5%90%8E%E5%9C%A8%E4%B8%8B%E9%9D%A2%E5%B0%B1%E5%8F%AF%E4%BB%A5%E5%AE%89%E8%A3%85%E5%91%BD%E4%BB%A4%EF%BC%9A%20wget%20https%3A%2F%2Fdeveloper.download.nvidia.com%2Fcompute%2Fcuda%2F11.7.1%2Flocal_installers%2Fcuda_11.7.1_515.65.01_linux.run%20sudo%20sh%20cuda_11.7.1_515.65.01_linux.run,%E3%80%82%20%E8%BE%93%E5%85%A5%20accept%20%E3%80%82%20%E7%82%B9%E5%87%BB%E7%A9%BA%E6%A0%BC%EF%BC%8C%E5%8F%96%E6%B6%88%E5%AE%89%E8%A3%85%E9%A9%B1%E5%8A%A8%EF%BC%8C%E7%84%B6%E5%90%8E%E9%80%89%E6%8B%A9%20Install%20%E3%80%82%20%E7%9C%8B%E5%88%B0%E4%B8%8A%E9%9D%A2%E7%9A%84%E6%B6%88%E6%81%AF%EF%BC%8C%E5%B0%B1%E8%AF%B4%E6%98%8E%E5%AE%89%E8%A3%85%E5%AE%8C%E6%88%90%E4%BA%86%E3%80%82)
  [教程2](https://blog.csdn.net/weixin_44148028/article/details/120988088)

* [教程-cuda卸载，注意不要执行第二条卸载驱动](https://blog.csdn.net/leviopku/article/details/131725756)
* [cuda 卸载](https://blog.csdn.net/qq_53937391/article/details/131501801)


### cudnn
* [注意 使用deb方式安装的cudnn h文件不在cuda/include中，如需要可以考虑创建软链接](https://blog.csdn.net/qq_32033383/article/details/135015041)
* [教程1](https://blog.csdn.net/zxdd2018/article/details/127705627)
* [官网文档安装](https://docs.nvidia.com/deeplearning/cudnn/install-guide/index.html#installlinux-tar)

### TensorRT 安装 注意注意使用deb安装
* [务必使用官网的deb安装教程](https://developer.nvidia.com/tensorrt)
* [教程1](https://blog.csdn.net/zong596568821xp/article/details/86077553)
* [教程2](https://zhuanlan.zhihu.com/p/671495184)
* [Tensorrt 卸载](https://wenku.csdn.net/answer/81m8aqyfo2)

***

### 编译期间一些错误的记录
* 编译时链接.a文件的目录不是项目目录，使用${CMAKE_SOURCE_DIR}/(记住要加斜杠)来确定项目目录
* cannot find -lcudart: No such file or directory 参阅教程[创建软链接](https://zhuanlan.zhihu.com/p/628678809)

### 运行期间一些问题的记录
* 关于CUDA lazy loading is not enabled. 报警（cuda >= 11.7) [教程](https://blog.csdn.net/s1_0_2_4/article/details/135026761)
* 第一次运行，程序会针对运行设备进行Onnx生成Engine的过程，根据设备算力，时间在1～10分钟不等。行需要等待1-10分种用于生成

***

## 其他一些问题的解决

* 解决opencv + cuda 扩展编译时的依赖问题 [教程](https://blog.csdn.net/victoryckl/article/details/8214688)

***

## yolov8输出结果分析 按照目前使用的华中科技大学1280 模型
* yolov8输出的output0.shape为 1x19x33600
* 33600是预选框的数量
* 其中19的为前4个是框的xywh（中心坐标加宽高），与yolov5相比少了一个
* objectness，那么objectness怎么获得呢？在yolov8中objectness=后面15个类的confidence中最大的那个
* 最终的score=objectness*confidence

***

## 关于串口
* ubuntu默认是不给串口权限的[开放串口权限](https://blog.csdn.net/qq_39779233/article/details/111400187)

***

## 标定流程
* 使用键盘n键选择图片
* 点击 ENTER 选定图片
* 使用鼠标进行标定
* 再次点击ENTER 结束标定
* ESC 取消标定退出程序 DEL 删除所有重新标定

***

## 地图数据
* 务必确保所有数据共面！！！
* 地图务必按照顺时针或逆时针记录
* 以mm作为单位
* 前三个点用于计算这个平面的方程，三点不共线，且却能表示这个平面或斜面
* 不要忘记了场地平面 0,15,0 28,15,0 28,0,0 0,0,0

***

## 关于二维查找表
* 输入UV
* 输出二维坐标

***

## 相关文件和资源的下载
* [地图数据下载](https://pan.baidu.com/s/1PRFtK6fNhQZwa6yj4sCJzA?pwd=t117)
* 权重数据和相关视频请到华中科技大学狼牙战队开源雷达站处下载