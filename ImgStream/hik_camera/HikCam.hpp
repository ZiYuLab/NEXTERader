
#ifndef _HIK_CAM_H_
#define _HIK_CAM_H_


#include <cmath>
#include <opencv2/opencv.hpp>
#include <MvCameraControl.h>
#include <chrono>
#include <spdlog/spdlog.h>

typedef enum GAIN_MODE_{
    R_CHANNEL,
    G_CHANNEL,
    B_CHANNEL
} GAIN_MODE;


class HikCam
{
    int deviceNum_ = 0;
    MV_CC_DEVICE_INFO_LIST stDeviceList_;

    int nRet = MV_OK;
    void* handleCamLeft_ = nullptr;
    void* handleCamRight_ = nullptr;

    MVCC_INTVALUE stParamLeft_;
    MVCC_INTVALUE stParamRight_;
    MV_FRAME_OUT_INFO_EX stImageInfoLeft_ = {0};
    MV_FRAME_OUT_INFO_EX stImageInfoRight_ = {0};

    unsigned char *pDataL_ = nullptr;
    unsigned char *pDataR_ = nullptr;

    unsigned int nDataSizeLeft_ = 0;
    unsigned int nDataSizeRight_ = 0;

    MVCC_FLOATVALUE FrameRate;
    MVCC_FLOATVALUE *pFrameRate = &FrameRate;
    MV_FRAME_OUT pFrame = {nullptr};
    int timestamp_offset = 0;

public:
    //相机库初始化
    HikCam();

    //打开设备
    bool startDevice(int leftDeviceNum, int rightDeviceNum);
    void stopDevice();
    bool getFrame(cv::Mat &cvMatL, cv::Mat &cvMatR);

    ~HikCam();
};
#endif

