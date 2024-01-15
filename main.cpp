#include <cstdio>
#include <opencv2/opencv.hpp>
#include <string>
#include <string>

#include "NEInit.h"
#include "NEImgStream.h"
#include "NEDemarcatePose.h"
#include "NESerial.h"
#include "NERobotPosition.h"
#include "NESolvePosition.h"
#include "NEConfig.h"
#include "NENet.h"

using namespace ne;

int main()
{
    SHOW_NE;

    CHECK_PATH("../Config/config.yaml");
    auto config = NEConfig("../Config/config.yaml");

    NENet neNet(config);
    NEImgStream stream(config);
    NEDemarcatePose demarcatePose(config);
    NESolvePosition solvePosition(config);
    NERobotPosition robotPosition(config);
    NESerial serial(config.getConfig()["serial_path"].as<std::string>());
    //serial.debugWrite();
    int bufferID = serial.genWriteThreat(10, robotPosition.cmdID, robotPosition.exportBuffers(), robotPosition.sizeEach);

    // 读取地图数据
    LOG_INFO("Read map data!");
    auto mapData = config.getMapData();
    stream.drawMapDataConfig(mapData); // 画出地图数据
    stream.show("检查地图数据, 回车进入", NE_STREAM_MAP, 0.5);
    stream.waitKey(13, 0); // ENTER

    // 场地标定
    if (!demarcatePose.isDemarcate())
    {
        LOG_INFO("Start to demarcate!");
        demarcatePose.startDemarcate(stream);
    }

    LOG_INFO("Solve Position Data Init start!");
    // 初始化查找表和网络索引
    solvePosition.init(stream.sourceInfo.row, stream.sourceInfo.col, mapData, demarcatePose._demarcatePoint);
    LOG_SUCCESS("Finish!")

    neNet.netInit();
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

    while (true)
    {
        stream.timerStart();
        robotPosition.oneLoop();
        if (!stream.getFrame())
            break;

        if (stream.waitKey(27, 1)) // ESC 退出
            break;

        neNet.doInference(stream._frame); // 进行推理

        stream.drawBox(neNet); // 画框框

        solvePosition.get2DPoints(neNet, robotPosition); // 计算位置

        stream.drawMap(solvePosition._result, neNet); // 画地图

        stream.show("Map", NE_STREAM_MAP, 0.3);

        serial.updateToWriteBuffer(bufferID, robotPosition.exportBuffers());

        stream.timerEnd();
        stream.drawFPS();
        stream.show("Source", NE_STREAM_SOURCE);

        //stream.timerEnd();
        //LOG_INFO("FPS:" << stream.getFPS());
    }

    return 0;
}
