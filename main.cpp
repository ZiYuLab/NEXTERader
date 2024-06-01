#include <cstdio>
#include <opencv2/opencv.hpp>
#include <string>
#include <string>

#include "NEInit.h"
#include "NEImgStream.h"
#include "NEDemarcatePose.h"
//#include "NESerial.h"
//#include "NERobotPosition.h"
#include "NESolvePosition.h"
#include "NEConfig.h"
#include "NENet.h"
#include "Tracker.hpp"
#include "IOManager.hpp"
#include "StereoSolvePosition.hpp"
//#include "NEStereoPosition.hpp"
#include "RmCRC.h"

using namespace ne;


//时间戳转化为时间 毫秒级
std::string Stamp2Time(long long timestamp)
{
    int ms = timestamp % 1000;//取毫秒
    time_t tick = (time_t)(timestamp/1000);//转换时间
    struct tm tm;
    char s[40];
    tm = *localtime(&tick);
    strftime(s, sizeof(s), "%Y-%m-%d %H:%M:%S", &tm);
    std::string str(s);
    str = str+ " " + std::to_string(ms);
    return str;
}

#define SAVE_VIDEO

int main()
{

#if 0
    unsigned char testCRC[] = {0xA5, 0x0A, 0x00, 0x0A, 0xD7, 0x05, 0x03, 0x65, 0x00, 0x2A, 0x5B, 0xB4, 0x41, 0x18, 0x26, 0x07, 0x41, 0x15, 0xFE };
    //testCRC[4] = Get_CRC8_Check_Sum(testCRC, 4, 0xff);
    if (Verify_CRC16_Check_Sum(testCRC, 19))
    {
        std::cout << "----";
    }
#endif

#if 0
    HikCam hikCam;
    int id = -1, lID = -1, rID = -1;
    spdlog::info("Tap Left ID");
    std::cin >> lID;
    spdlog::info("Tap Right ID");
    std::cin >> rID;
    hikCam.startDevice(lID, rID);
    int couSaveID = 1;
    while (true)
    {
        cv::Mat imgL, imgR;
        if (!hikCam.getFrame(imgL, imgR))
        {
            continue;
        }
        cv::imshow("Left", imgL);
        cv::imshow("Right", imgR);
        char Key = cv::waitKey(1);
        if (Key == 'n')
            continue;
        if (Key == 'c')
            return 0;
        if (Key == 's')
        {
            std::string fileNameL = "../lib/BD/Left/Left-" + std::to_string(couSaveID) + ".png";
            std::string fileNameR = "../lib/BD/Right/Right-" + std::to_string(couSaveID) + ".png";
            cv::imwrite(fileNameL, imgL);
            cv::imwrite(fileNameR, imgR);
            couSaveID++;
            continue;
        }
    }
#endif


#if 1
    SHOW_NE;

    CHECK_PATH("../Config/configL.yaml");
    auto configL = NEConfig("../Config/configL.yaml", false);
    CHECK_PATH("../Config/configR.yaml");
    auto configR = NEConfig("../Config/configR.yaml", false);

    CHECK_PATH("../Config/config.yaml");
    auto config = NEConfig("../Config/config.yaml");

    NEImgStream stream(config);

    // 验证以下，图像大小是否正确
    if ((stream.sourceInfoL.col != stream.sourceInfoR.col) || (stream.sourceInfoL.row != stream.sourceInfoR.row))
        spdlog::warn("Img Size Not = !, USE Left Camera");
    StereoSolvePosition stereoSolvePosition(config, configL, configR);

    // 初始化双目
    stereoSolvePosition.init(stream.sourceInfoL.col, stream.sourceInfoL.row);
    // 双目矫正开启
    stream.initStereo(stereoSolvePosition.stereoParam_);

    NEDemarcatePose demarcatePose(config);

    NENet neNetL(config);

    NESolvePosition solvePositionL(config, configL);
//    NERobotPosition robotPositionL(configL);

    NENet neNetR(config);
    NESolvePosition solvePositionR(config, configR);
    Tracker carTracker(config);


    IOManager *ioManagerPtr = IOManager::getIOManagerInstance(config.our());
    ioManagerPtr->init();
    ioManagerPtr->open();

//    while (true)
//    {
//        stream.getFrame();
//        cv::Mat frame = StereoMatchBM(stream.frameL, stream.frameR, stereoSolvePosition.stereoParam_.Q_m);
//        cv::imshow("1212", frame);
//        cv::waitKey(1);
//    }
    // 读取地图数据
    LOG_INFO("Read map data!");
    auto mapData = config.getMapData();
    stream.drawMapDataConfig(mapData); // 画出地图数据
    stream.show("检查地图数据, 回车进入", NE_STREAM_MAP, 0.5);
    stream.waitKey(13, 0); // ENTER

    // 场地标定 LEFT
    if (!demarcatePose.isDemarcate())
    {
        LOG_INFO("Start to demarcate LEFT!");
        demarcatePose.startDemarcate(stream, CAM_LEFT);

        LOG_INFO("Start to demarcate RIGHT!");
        demarcatePose.startDemarcate(stream, CAM_RIGHT);
    }

    stereoSolvePosition.initCamPose(demarcatePose.demarcatePointLeft_);

    LOG_INFO("Solve Position Data Init Left start!");

    // 初始化查找表和网络索引 LEFT
    LOG_INFO(demarcatePose.demarcatePointRight_.size());

    // 分别为左边的和右边的解算初始化
    solvePositionL.init(stream.sourceInfoL.row, stream.sourceInfoL.col, mapData, demarcatePose.demarcatePointLeft_, stereoSolvePosition.stereoParam_, -1);
    solvePositionR.init(stream.sourceInfoR.row, stream.sourceInfoR.col, mapData, demarcatePose.demarcatePointRight_, stereoSolvePosition.stereoParam_, 1);
    LOG_SUCCESS("Finish!")


    neNetL.netInit();
    neNetR.netInit();

    LOG_SUCCESS("Init All Finish!");

    LOG_INFO("Please Check All Data And Input 'Y/y' to Start Or 'N/n' to Stop!");
    char input;
    std::cin >> input;
    if (!(input == 'Y' || input == 'y'))
    {
        LOG_INFO("Exit!");
        abort();
    }

    SHOW_NE;
    LOG_INFO("START!");

#ifdef SAVE_VIDEO
    auto timeNow = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
    long long timestamp = timeNow.count();
    std::string videoFileLName = "../lib/video/Save/" + Stamp2Time(timestamp) + "-Left.avi";
    std::string videoFileRName = "../lib/video/Save/" + Stamp2Time(timestamp) + "-Right.avi";
    cv::VideoWriter outputVideoL;
    cv::VideoWriter outputVideoR;
    outputVideoL.open(videoFileLName, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 15.0, cv::Size(stream.sourceInfoL.col, stream.sourceInfoL.row));
    outputVideoR.open(videoFileRName, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 15.0, cv::Size(stream.sourceInfoR.col, stream.sourceInfoR.row));
#endif

    while (true)
    {
        stream.timerStart();

        if (!stream.getFrame())
            break;
#ifdef SAVE_VIDEO
        outputVideoL.write(stream.frameL);
        outputVideoR.write(stream.frameR);
#endif
        if (stream.waitKey(27, 1)) // ESC 退出
            break;

        neNetL.doInference(stream.frameL); // 进行推理
        neNetR.doInference(stream.frameR); // 进行推理
//
        stream.drawBoxL(neNetL); // 画框框
        stream.drawBoxR(neNetR); // 画框框

        carTracker.matchArmor(neNetL, neNetR);
        carTracker.track();

        carTracker.solvePosition(solvePositionL, solvePositionR, stereoSolvePosition, stream);
        stream.drawMap(carTracker.getRobotData());

        ioManagerPtr->robotPositionUpdate(carTracker.getRobotData());


        stream.show("Map", NE_STREAM_MAP, 0.5);

        stream.timerEnd();
        stream.drawFPS();
        stream.show("Source L", NE_STREAM_SOURCE_LEFT);
        stream.show("Source R", NE_STREAM_SOURCE_RIGHT);
    }
#endif
    return 0;
}
