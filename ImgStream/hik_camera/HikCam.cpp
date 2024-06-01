#include <cstdio>
#include "HikCam.hpp"

bool PrintDeviceInfo(MV_CC_DEVICE_INFO* pstMVDevInfo)
{
    if (NULL == pstMVDevInfo)
    {
        printf("The Pointer of pstMVDevInfo is NULL!\n");
        return false;
    }
    if (pstMVDevInfo->nTLayerType == MV_GIGE_DEVICE)
    {
        int nIp1 = ((pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0xff000000) >> 24);
        int nIp2 = ((pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x00ff0000) >> 16);
        int nIp3 = ((pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x0000ff00) >> 8);
        int nIp4 = (pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x000000ff);
        // ch:打印当前相机ip和用户自定义名字 | en:print current ip and user defined name
        printf("Device Model Name: %s\n", pstMVDevInfo->SpecialInfo.stGigEInfo.chModelName);
        printf("CurrentIp: %d.%d.%d.%d\n" , nIp1, nIp2, nIp3, nIp4);
        printf("UserDefinedName: %s\n\n" , pstMVDevInfo->SpecialInfo.stGigEInfo.chUserDefinedName);
    }
    else if (pstMVDevInfo->nTLayerType == MV_USB_DEVICE)
    {
        printf("Device Model Name: %s\n", pstMVDevInfo->SpecialInfo.stUsb3VInfo.chModelName);
        printf("UserDefinedName: %s\n\n", pstMVDevInfo->SpecialInfo.stUsb3VInfo.chUserDefinedName);
    }
    else
    {
        printf("Not support.\n");
    }
    return true;
}

HikCam::HikCam()
{
    spdlog::info("Start Hik Device");


    memset(&stDeviceList_, 0, sizeof(MV_CC_DEVICE_INFO_LIST));

    // 枚举设备
    nRet = MV_CC_EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE, &stDeviceList_);
    if (MV_OK != nRet)
    {
        spdlog::error("Cam MV_CC_EnumDevices fail! nRet {}", nRet);
        return ;
    }

    if (stDeviceList_.nDeviceNum > 0)
    {
        for (int i = 0; i < stDeviceList_.nDeviceNum; i++)
        {
            spdlog::info("Cam Device: {}", i);
            MV_CC_DEVICE_INFO * pDeviceInfo = stDeviceList_.pDeviceInfo[i];

            if (pDeviceInfo == nullptr)
            {
                return ;
            }
            PrintDeviceInfo(pDeviceInfo);
            deviceNum_ = i + 1;
        }
    }
    else
    {
        spdlog::error("Cam Find No Devices!");
        return ;
    }
}
HikCam::~HikCam()
{

//    nRet = MV_CC_FreeImageBuffer(handleCamLeft_, (&pFrame));
//    nRet = MV_CC_FreeImageBuffer(, (&pFrame));
    stopDevice();

}

bool HikCam::startDevice(int leftDeviceNum, int rightDeviceNum)
{
    if (leftDeviceNum >= deviceNum_ || rightDeviceNum >= deviceNum_)
    {
        spdlog::error("Cam Error Device Num: {} or {} !", leftDeviceNum, rightDeviceNum);
        return false;
    }

    // 创建句柄
    nRet = MV_CC_CreateHandle(&handleCamLeft_, stDeviceList_.pDeviceInfo[leftDeviceNum]);
    if (nRet != MV_OK)
    {
        spdlog::error("MV_CC_CreateHandle Right fail! nRet {}", nRet);
        return false;
    }
    nRet = MV_CC_CreateHandle(&handleCamRight_, stDeviceList_.pDeviceInfo[rightDeviceNum]);
    if (nRet != MV_OK)
    {
        spdlog::error("MV_CC_CreateHandle Right fail! nRet {}", nRet);
        return false;
    }


    // 打开设备
    nRet = MV_CC_OpenDevice(handleCamLeft_);
    if (nRet != MV_OK)
    {
        printf("MV_CC_OpenDevice fail! nRet [%x]\n", nRet);
        spdlog::error("MV_CC_OpenDevice fail! LEFT nRet {}", nRet);
        return false;
    }
    nRet = MV_CC_OpenDevice(handleCamRight_);
    if (nRet != MV_OK)
    {
        printf("MV_CC_OpenDevice fail! nRet [%x]\n", nRet);
        spdlog::error("MV_CC_OpenDevice fail! RIGHT nRet {}", nRet);
        return false;
    }

    // 关掉自动帧率
    nRet = MV_CC_SetBoolValue(handleCamLeft_, "AcquisitionFrameRateEnable", false);
    if (MV_OK != nRet)
    {
        printf("set AcquisitionFrameRateEnable fail! LEFT nRet [%x]\n", nRet);
        return false;
    }
    nRet = MV_CC_SetBoolValue(handleCamRight_, "AcquisitionFrameRateEnable", false);
    if (MV_OK != nRet)
    {
        printf("set AcquisitionFrameRateEnable fail! nRet RIGHT [%x]\n", nRet);
        return false;
    }

#if 1
    // 设置触发模式为on
    // set trigger mode as on
    nRet = MV_CC_SetEnumValue(handleCamLeft_, "TriggerMode", 1);
    if (MV_OK != nRet)
    {
        printf("MV_CC_SetTriggerMode fail! nRet [%x]\n", nRet);
        spdlog::error("MV_CC_SetTriggerMode fail! LEFT nRet {}", nRet);
        return false;
    }
    nRet = MV_CC_SetEnumValue(handleCamRight_, "TriggerMode", 1);
    if (MV_OK != nRet)
    {
        printf("MV_CC_SetTriggerMode fail! nRet [%x]\n", nRet);
        spdlog::error("MV_CC_SetTriggerMode fail! RIGHT nRet {}", nRet);
        return false;
    }
    // 设置触发模式
    nRet = MV_CC_SetEnumValue(handleCamLeft_, "TriggerSource", MV_TRIGGER_SOURCE_SOFTWARE);
    if (nRet != MV_OK)
    {
        spdlog::error("MV_CC_SetTriggerSource fail! LEFT nRet {}", nRet);
        return false;
    }
    nRet = MV_CC_SetEnumValue(handleCamRight_, "TriggerSource", MV_TRIGGER_SOURCE_SOFTWARE);
    if (nRet != MV_OK)
    {
        spdlog::error("MV_CC_SetTriggerSource fail! RIGHT nRet {}", nRet);
        return false;
    }


    /*
    MVCC_ENUMVALUE value ;
    MVCC_ENUMVALUE *p = &value;
    nRet = MV_OK
    nRet = MV_CC_GetPixelFormat(handle, p);

    if(nRet != MV_OK) {
            printf("%d",value.nSupportValue);
    }
                       */
    nRet = MV_CC_SetPixelFormat(handleCamLeft_, PixelType_Gvsp_RGB8_Packed);
    if(nRet != MV_OK) {
        printf("[Camera] setPixelFormat L failed! nRet [%x]\n", nRet);
        return false;
    }
    nRet = MV_CC_SetPixelFormat(handleCamRight_, PixelType_Gvsp_RGB8_Packed);
    if(nRet != MV_OK) {
        printf("[Camera] setPixelFormat R failed! nRet [%x]\n", nRet);
        return false;
    }


    //设置曝光时间
    nRet = MV_CC_SetFloatValue(this->handleCamLeft_, "ExposureTime", 10000);
    if(nRet == MV_OK)
    {
        printf("[Camera] 曝光值L设置成功\n");
    }
    else
    {
        printf("[Camera] 曝光值L设置失败\n");
        return false;
    }

    nRet = MV_CC_SetFloatValue(this->handleCamRight_, "ExposureTime", 10000);
    if(nRet == MV_OK)
    {
        printf("[Camera] 曝光值R设置成功\n");
    }
    else
    {
        printf("[Camera] 曝光值R设置失败\n");
        return false;
    }


    //曝光增益
    nRet = MV_CC_SetFloatValue(handleCamLeft_, "Gain", 15);
    if(nRet == MV_OK)
    {
        printf("[CAMERA] 设置曝光增益L成功！\n");
    }
    else
    {
        printf("[CAMERA] 设置曝光增益L失败！\n");
        return false;
    }

    nRet = MV_CC_SetFloatValue(handleCamRight_, "Gain", 15);
    if(nRet == MV_OK)
    {
        printf("[CAMERA] 设置曝光增益R成功！\n");
    }
    else
    {
        printf("[CAMERA] 设置曝光增益R失败！\n");
        return false;
    }













#endif
    // 开始取流
    nRet = MV_CC_StartGrabbing(handleCamLeft_);
    if (nRet != MV_OK)
    {
        spdlog::error("MV_CC_StartGrabbing fail! LEFT nRet {}", nRet);
        return false;
    }
    nRet = MV_CC_StartGrabbing(handleCamRight_);
    if (nRet != MV_OK)
    {
        spdlog::error("MV_CC_StartGrabbing fail! RIGHT nRet {}", nRet);
        return false;
    }

    // 停止取流
    // end grab image
    nRet = MV_CC_StopGrabbing(handleCamLeft_);
    if (MV_OK != nRet)
    {
        spdlog::error("MV_CC_StopGrabbing fail! LEFT nRet {}", nRet);
        return false;
    }
    nRet = MV_CC_StopGrabbing(handleCamRight_);
    if (MV_OK != nRet)
    {
        spdlog::error("MV_CC_StopGrabbing fail! RIGHT nRet {}", nRet);
        return false;
    }

    nRet = MV_CC_StartGrabbing(handleCamLeft_);
    if (nRet != MV_OK)
    {
        spdlog::error("MV_CC_StartGrabbing fail! LEFT nRet {}", nRet);
        return false;
    }
    nRet = MV_CC_StartGrabbing(handleCamRight_);
    if (nRet != MV_OK)
    {
        spdlog::error("MV_CC_StartGrabbing fail! RIGHT nRet {}", nRet);
        return false;
    }

    // 获取基本取流信息
    memset(&stParamLeft_, 0, sizeof(MVCC_INTVALUE));
    memset(&stParamRight_, 0, sizeof(MVCC_INTVALUE));

    nRet = MV_CC_GetIntValue(handleCamLeft_, "PayloadSize", &stParamLeft_);
    if (MV_OK != nRet)
    {
        spdlog::error("Get PayloadSize fail! LEFT nRet {}", nRet);
        return false;
    }
    nRet = MV_CC_GetIntValue(handleCamRight_, "PayloadSize", &stParamRight_);
    if (MV_OK != nRet)
    {
        spdlog::error("Get PayloadSize fail! RIGHT nRet {}", nRet);
        return false;
    }


    pDataL_ = (unsigned char *)malloc(sizeof(unsigned char) * stParamLeft_.nCurValue);
    pDataR_ = (unsigned char *)malloc(sizeof(unsigned char) * stParamRight_.nCurValue);
    if (pDataL_ == nullptr || pDataR_ == nullptr)
    {
        return false;
    }
    nDataSizeLeft_ = stParamLeft_.nCurValue;
    nDataSizeRight_ = stParamRight_.nCurValue;

    stImageInfoRight_ = {0};

    return true;
}

bool HikCam::getFrame(cv::Mat &cvMatL, cv::Mat &cvMatR)
{

    // 触发
    nRet = MV_CC_SetCommandValue(handleCamLeft_, "TriggerSoftware");
    if (MV_OK != nRet)
    {
        printf("failed in TriggerSoftware[%X]\n", nRet);
        spdlog::error("failed in TriggerSoftware LEFT nRet {}", nRet);
        return false;
    }
    nRet = MV_CC_SetCommandValue(handleCamRight_, "TriggerSoftware");
    if (MV_OK != nRet)
    {
        printf("failed in TriggerSoftware[%X]\n", nRet);
        spdlog::error("failed in TriggerSoftware RIGHT nRet {}", nRet);
        return false;
    }

    stImageInfoLeft_ = {0};
    stImageInfoRight_ = {0};

    nRet = MV_CC_GetOneFrameTimeout(handleCamLeft_, pDataL_, nDataSizeLeft_, &stImageInfoLeft_, 1000);
    if (MV_OK != nRet)
    {
        printf("Get One Frame failed![%x]\n", nRet);
        spdlog::error("Get One Frame failed! LEFT nRet {}", nRet);
        return false;
    }
    nRet = MV_CC_GetOneFrameTimeout(handleCamRight_, pDataR_, nDataSizeRight_, &stImageInfoRight_, 1000);
    if (MV_OK != nRet)
    {
        printf("Get One Frame failed![%x]\n", nRet);
        spdlog::error("Get One Frame failed! RIGHT nRet {}", nRet);
        return false;
    }


    cv::Mat srcL = cv::Mat(stImageInfoLeft_.nHeight, stImageInfoLeft_.nWidth, CV_8UC3);
    cv::Mat srcR = cv::Mat(stImageInfoRight_.nHeight, stImageInfoRight_.nWidth, CV_8UC3);

    memcpy(srcL.data, pDataL_, stImageInfoLeft_.nWidth * stImageInfoLeft_.nHeight * 3);
    memcpy(srcR.data, pDataR_, stImageInfoRight_.nWidth * stImageInfoRight_.nHeight * 3);

    if (srcL.empty() || srcR.empty())
    {
        spdlog::error("Frame EMPTY!!");
    }

    cv::cvtColor(srcL, cvMatL, cv::COLOR_RGB2BGR);
    cv::cvtColor(srcR, cvMatR, cv::COLOR_RGB2BGR);

    return true;

}

void HikCam::stopDevice()
{
    // 停止取流
    // end grab image
    nRet = MV_CC_StopGrabbing(handleCamLeft_);
    if (MV_OK != nRet)
    {
        spdlog::error("MV_CC_StopGrabbing fail! LEFT nRet {}", nRet);
        return;
    }
    // 关闭设备
    // close device
    nRet = MV_CC_CloseDevice(handleCamLeft_);
    if (MV_OK != nRet)
    {
        spdlog::error("MV_CC_CloseDevice fail! LEFT nRet {}", nRet);
        return;
    }
    // 销毁句柄
    // destroy handle
    nRet = MV_CC_DestroyHandle(handleCamLeft_);
    if (MV_OK != nRet)
    {
        spdlog::error("MV_CC_DestroyHandle fail! LEFT nRet {}", nRet);
        return;
    }



    // 停止取流
    // end grab image
    nRet = MV_CC_StopGrabbing(handleCamRight_);
    if (MV_OK != nRet)
    {
        spdlog::error("MV_CC_StopGrabbing fail! RIGHT nRet {}", nRet);
        return;
    }
    // 关闭设备
    // close device
    nRet = MV_CC_CloseDevice(handleCamRight_);
    if (MV_OK != nRet)
    {
        spdlog::error("MV_CC_CloseDevice fail! RIGHT nRet {}", nRet);
        return;
    }
    // 销毁句柄
    // destroy handle
    nRet = MV_CC_DestroyHandle(handleCamRight_);
    if (MV_OK != nRet)
    {
        spdlog::error("MV_CC_DestroyHandle fail! RIGHT nRet {}", nRet);
        return;
    }

    if (pDataL_)
    {
        free(pDataL_);
        pDataL_ = nullptr;
    }
    if (pDataR_)
    {
        free(pDataL_);
        pDataL_ = nullptr;
    }
}



