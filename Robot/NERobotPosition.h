/*
    机器人位置设定，并自动生成数据串口函数
    20231124 by ZiYu
    裁判串口协议版本：v1.5
*/
#ifndef NESERIAL_NEROBOTPOSITION_H
#define NESERIAL_NEROBOTPOSITION_H

#include <vector>
#include "NEInit.h"
#include "NERobot.h"
#include "NEConfig.h"

namespace ne
{

#define ROBOT_GROUND_NUM_REGION 10 // 5 * 2
#define ROBOT_GROUND_NUM_NATIONWIDE 12 // 6 * 2

#define BUFFERS_MAX_LIMIT 20

    class NERobotPosition {
    private:
        NE32F2NE8U ne32F2Ne8U;
        NE16U2NE8U ne16U2Ne8U;
        std::vector<std::vector<NE_8U>> buffers_;
        int _clearDelayTime = 0; // 延时循环次数，达到循环次数，自动初始化positionBuffer
        int _timeCou = 0;

        int robotGroundNum_;
    public:
        std::vector<robot_t> _robotPositionBuffer; // 存储规则：按照ID存储 1 存储在索引为1位置上 101 存储在索引为 8位置上

        const NE_16U cmdID = 0x0305;
        const int sizeEach = 10;

        NERobotPosition(NEConfig & config);
        ~NERobotPosition();

        /**
         * 新循环开始标志，用于清除缓冲区
         */
        void oneLoop();

        void positionBufferInit();
        void updatePosition(int robotID, cv::Point2f position);
        void setPosition(int robotID, NE_32F x, NE_32F y);
        std::vector<std::vector<NE_8U>> exportBuffers() const;

        // 每次更新数据结束务必调用清楚缓冲区，不然会导致内存溢出
        void clearBuffers();
    };
}


#endif //NESERIAL_NEROBOTPOSITION_H
