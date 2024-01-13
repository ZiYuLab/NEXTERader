#include <cstdio>
#include "HikCam.hpp"
HikCam::HikCam() = default;
HikCam::~HikCam() {
    nRet = MV_CC_FreeImageBuffer(handle, (&pFrame));
    /*
    if(nRet == MV_OK)
         printf("[Camera] free image buffer succeed!\n");
    else printf("[Camera] free image buffer failed!\n");
     */
}

int HikCam::StartDevice(int serial_number)
{   //打开设备
    MV_CC_DEVICE_INFO_LIST stDeviceList;
    memset(&stDeviceList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));

    //枚举设备
    nRet = MV_CC_EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE, &stDeviceList);
    if(MV_OK != nRet)
    {
        printf("EnumDevices failed! nRet [%x]\n", nRet);
        return -1;
    }
    if(stDeviceList.nDeviceNum > 0) //设备数量不为0
    {
        nRet = MV_CC_CreateHandle(&handle, stDeviceList.pDeviceInfo[serial_number]);
        if(MV_OK != nRet)
        {
            printf("CreateHandle failed! nRet [%x]\n", nRet);
            return -1;
        }
        nRet = MV_CC_OpenDevice(handle);
        if(MV_OK == nRet)
        {
            printf("[Camera] open succeed!\n");
            return serial_number;
        }
        if(MV_OK != nRet) {
            printf("[Camera] open failed!\n");
            return -1;
        }
    }
    else
    {
    printf("[Camera] Find No Devices!\n");
    return -1;
    }
    return serial_number;
}


bool HikCam::SetStreamOn()
{
    //开始采集
    //设置触发模式为off
    this->nRet = MV_CC_SetEnumValue(handle, "TriggerMode", 0);
    if(MV_OK != nRet)
    {
        printf("[Camera] SetEnumValue failed! nRet [%x]!\n", nRet);
        return false;
    }

    //设置采集模式为连续采集
    this->nRet = MV_CC_SetEnumValue(handle, "AcquisitionMode", 2);
    if(MV_OK != nRet)
    {
        printf("[Camera] SetEnumValue failed! nRet [%x]!\n", nRet);
        return false;
    }
    //开始取流
    nRet = MV_CC_StartGrabbing(handle);
    if(MV_OK == nRet)
    {
        printf("[Camera] StartGrabbing succeed!\n");
        return true;
    }
    else
    {
        printf("[Camera] StartGrabbing failed! nRet [%x]\n", nRet);
        return false;
    }
}


bool HikCam::SetPixelFormat(int format)
{
    /*
    MVCC_ENUMVALUE value ;
    MVCC_ENUMVALUE *p = &value;
    nRet = MV_OK
    nRet = MV_CC_GetPixelFormat(handle, p);

    if(nRet != MV_OK) {
            printf("%d",value.nSupportValue);
    }
                       */
    nRet = MV_CC_SetPixelFormat(handle, format);
    if(nRet != MV_OK) {
        printf("[Camera] setPixelFormat failed! nRet [%x]\n", nRet);
        return false;
    }

    return true;
}
//设置像素格式
bool HikCam::SetResolution(int width, int height)
{
    nRet = MV_OK;
    nRet = MV_CC_SetIntValue(this->handle, "Width", width);
    if(nRet != MV_OK)
    {
        printf("[Camera] setResolution failed! nRet [%x]\n", nRet);
        return false;
    }
    nRet = MV_CC_SetIntValue(this->handle, "Height", height);

    if(nRet == MV_OK)
    {
        printf("[Camera] setResolution succeed!\n");
        return true;
    }
    else
    {
        printf("setResolution failed!\n");
        printf("[Camera] setResolution failed! nRet [%x]\n", nRet);
        return false;
    }
}

bool HikCam::SetExposureTime(float ExposureTime)
{   //设置曝光时间
    nRet = MV_CC_SetFloatValue(this->handle, "ExposureTime", ExposureTime);
    if(nRet == MV_OK)
    {
        printf("[Camera] 曝光值设置成功\n");
        return true;
    }
    else
    {
        printf("[Camera] 曝光值设置失败\n");
        return false;
    }
}

bool HikCam::SetFrameRate(float FrameRate)
{
    nRet = MV_CC_SetFrameRate(handle,FrameRate);
    if(nRet == MV_OK)
    {
        printf("[Camera] 帧率设置成功\n");
        return true;
    }
    else
    {
        printf("[Camera] 帧率设置失败\n");
        return false;
    }
}
float HikCam::GetFrameRate()
{
    nRet = MV_CC_GetFloatValue(handle, "ResultingFrameRate", pFrameRate);
    return FrameRate.fCurValue;
}

bool HikCam::SetGAIN(float ExpGain)
{   //曝光增益
    nRet = MV_CC_SetFloatValue(handle, "Gain", ExpGain);
    if(nRet == MV_OK)
    {
        printf("[CAMERA] 设置曝光增益成功！\n");
        return true;
    }
    else
    {
        printf("[CAMERA] 设置曝光增益失败！\n");
        return false;
    }
}

bool HikCam::Set_Auto_BALANCE()
{   //自动白平衡（具有记忆功能）
    this->nRet = MV_CC_SetEnumValue(this->handle, "BalanceWhiteAuto", 1);
    if(nRet != MV_OK)
    {
        printf("[CAMERA] 自动白平衡设置失败！\n");
        return false;
    }
    else
    {
        printf("[CAMERA] 自动白平衡设置成功！\n");
        return true;
    }
}

bool HikCam::Set_BALANCE(int value, unsigned int value_number)
{   //手动白平衡（具有记忆功能））
    //关闭自动白平衡
    this->nRet = MV_CC_SetEnumValue(handle, "BalanceWhiteAuto", MV_BALANCEWHITE_AUTO_OFF);
    if(nRet != MV_OK)
    {
        printf("[CAMERA] 关闭自动白平衡失败！\n");
        return false;
    }

    //设置RGB三通道白平衡值
    if(value == 0)
    {
        this->nRet = MV_CC_SetBalanceRatioRed(handle, value_number);

        if(nRet == MV_OK)
        {
            printf("[CAMERA] set R_Balance succeed！\n");
            return true;
        }
        else
        {
            printf("[CAMERA] set R_Balance failed！\n");
            return false;
        }
    }
    else if(value == 1)
    {
        this->nRet = MV_CC_SetBalanceRatioGreen(handle, value_number);

        if(nRet == MV_OK)
        {
            printf("[CAMERA] set G_Balance succeed！\n");
        }
        else
        {
            printf("[CAMERA] set G_Balance failed！\n");
            return false;
        }
    }
    else if(value == 2)
    {
        this->nRet = MV_CC_SetBalanceRatioBlue(handle, value_number);

        if(nRet == MV_OK)
        {
            printf("[CAMERA] set B_Balance succeed！\n");
        }
        else
        {
            printf("[CAMERA] set B_Balance failed！\n");
            return false;
        }
    }
    return true;
}

bool HikCam::Set_Gamma(bool set_status,double dGammaParam)
{   //设置Gamma值
    if(set_status)
    {
        nRet = MV_CC_SetEnumValue(handle, "Gamma", 1);
        if(nRet == MV_OK)
        {
            printf("[CAMERA] 设置Gamma值成功！\n");
            return true;
        }
        else
        {
            printf("[CAMERA] 设置Gamma值失败！\n");
            return false;
        }
    }
    else
    {
        nRet = MV_CC_SetEnumValue(handle, "Gamma", 0);
        if(nRet == MV_OK)
        {
            printf("[CAMERA] 关闭Gamma值成功！\n");
            return true;
        }
        else
        {
            printf("[CAMERA] 关闭Gamma值失败！\n");
            return false;
        }
    }
}

bool HikCam::Color_Correct(bool value)
{   //设置色彩校正
    if(value)
    {
        nRet = MV_CC_SetEnumValue(handle, "ColorCorrection", 1);
        if(nRet == MV_OK)
        {
            printf("[CAMERA] 设置色彩校正成功！\n");
            return true;
        }
        else
        {
            printf("[CAMERA]设置色彩校正失败！\n");
            return false;
        }
    }
    else
    {
        nRet = MV_CC_SetEnumValue(handle, "ColorCorrection", 0);
        if(nRet == MV_OK)
        {
            printf("[CAMERA] 关闭色彩校正成功！\n");
            return true;
        }
        else
        {
            printf("[CAMERA] 关闭色彩校正失败！\n");
            return false;
        }
    }
}

bool HikCam::Set_Contrast(bool set_status,int dContrastParam)
{   //设置对比度
    if(set_status)
    {
        nRet = MV_CC_SetEnumValue(handle, "Contrast", 1);
        if(nRet == MV_OK)
        {
            printf("[CAMERA] 设置对比度成功！\n");
            return true;
        }
        else
        {
            printf("[CAMERA] 设置对比度失败！\n");
            return false;
        }
    }
    else
    {
        nRet = MV_CC_SetEnumValue(handle, "Contrast", 0);
        if(nRet == MV_OK)
        {
            printf("[CAMERA] 关闭对比度成功！\n");
            return true;
        }
        else
        {
            printf("[CAMERA] 关闭对比度失败！\n");
            return false;
        }
    }
}

int HikCam::Get_TIMESTAMP() const
{   //获取时间戳
    std::chrono::_V2::steady_clock::time_point time_start = std::chrono::_V2::steady_clock::now();
    return ((int)time_start.time_since_epoch().count() - timestamp_offset);
}

void HikCam::GetMat(cv::Mat &dst){
        // ch:获取数据包大小 | en:Get payload size
        MVCC_INTVALUE stParam;
        memset(&stParam, 0, sizeof(MVCC_INTVALUE));
        nRet = MV_CC_GetIntValue(handle, "PayloadSize", &stParam);
        if (MV_OK != nRet)
        {
            printf("[CAMERA] Get PayloadSize fail! nRet [0x%x]\n", nRet);
        }

        MV_FRAME_OUT_INFO_EX stImageInfo = {0};
        memset(&stImageInfo, 0, sizeof(MV_FRAME_OUT_INFO_EX));
        auto * pData = (unsigned char *)malloc(sizeof(unsigned char) * stParam.nCurValue);
        if (NULL == pData){
        }
        unsigned int nDataSize = stParam.nCurValue;

        //从缓存区读取图像
        nRet = MV_CC_GetOneFrameTimeout(handle, pData, nDataSize, &stImageInfo, 1000);
        if(nRet != MV_OK){
            printf("[CAMERA] No data[%x]\n", nRet);
        }


        cv::Mat src = cv::Mat(stImageInfo.nHeight, stImageInfo.nWidth, CV_8UC1);
        memcpy(src.data, pData, stImageInfo.nWidth * stImageInfo.nHeight * 1);
        cv::cvtColor(src, dst, cv::COLOR_BayerGR2BGR);

        /*
        cv::Mat src = cv::Mat(stImageInfo.nHeight, stImageInfo.nWidth, CV_8UC3);
        memcpy(src.data, pData, stImageInfo.nWidth * stImageInfo.nHeight * 3);
        cv::cvtColor(src, dst, cv::COLOR_RGB2BGR);
        */

        if(pData){
            delete[] pData;
            pData = NULL;
        }
}