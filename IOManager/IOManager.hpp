//
// Created by ziyu on 24-5-7.
//

#ifndef IOManager_IOManager_HPP
#define IOManager_IOManager_HPP

#include "IODataDefine.hpp"
#include "RobotDefine.hpp"

#include "CSerialPort/SerialPort.h"
#include "CSerialPort/SerialPortInfo.h"
#include "CSerialPort/SerialPortListener.h"
#include "spdlog/sinks/basic_file_sink.h" // support for basic file logging

#include <memory>
#include <mutex>
#include <vector>
#include <thread>
#include "NEConfig.h"
#include "NEInit.h"

namespace ne {

    enum JudgeCmd_t
    {
        JUDGE_CMD_ROBOT_POSITION = 0x0305,
        JUDGE_CMD_ROBOT_INTERACTION = 0x0301
    };

    union Uint16T2Uint8T_u
    {
        uint16_t in = 0;
        uint8_t out[2];
    };

    union FloatT2Uint8T_u
    {
        float in = 0;
        uint8_t out[4];
    };

    class Listener : public itas109::CSerialPortListener
    {
    private:
        itas109::CSerialPort *p_sp;
    public:
        std::shared_ptr<void> readBufferPtr_;
        unsigned int readBufferSize_ = 0;
        radar_mark_data_t radarMarkData_;

        Listener(itas109::CSerialPort *sp)
                : p_sp(sp)
        {
            ;
        }

        void onReadEvent(const char *portName, unsigned int readBufferLen);
    };



    class IOManager
    {
    private:

        IOManager(Our_t our);
        IOManager& operator = (const IOManager &t) = delete;
        IOManager(const IOManager &t) = delete;


        Our_t our_;

        itas109::CSerialPort *sp_ = nullptr;
        //Listener *listener_ = nullptr;
        std::mutex writeMutex_;
        bool threadRun_ = true;
        std::thread *writeThreadPtr_ = nullptr;
        std::thread *readThreadPtr_ = nullptr;

        // 转换器
        Uint16T2Uint8T_u uint16T2Uint8T_;
        MapRobotData2Uint8T_u mapRobotData2Uint8T_;
        FloatT2Uint8T_u floatT2Uint8T_;

        // 预定义数据帧头
        std::vector<uint8_t> robotPositionHead_;
        std::vector<uint8_t> radarCmdHead_;


        // 数据保存与缓冲
        // 防止迭代器实效，这个Buffer的大小一定不要发生变化，只允许赋值操作
        RobotStereoData_t robotStereoDataBuffer_ = RobotStereoData_t(6); // TODO
        robot_interaction_radar_data_t robotInteractionRadarData_; // 雷达双倍易伤CMD 要求递增

        std::shared_ptr<spdlog::logger> IOFileLogger_;

        // TODO 这里手动写数据的发送
        static void writeThread(IOManager *thisPtr);

        static void readThread(IOManager *thisPtr);

    public:


        ~IOManager();

        static IOManager* getIOManagerInstance(Our_t our);

        // TODO 这里写帧头的生成
        void init(); // TODO

        bool open();

        void stopAll();

        void robotPositionUpdate(RobotStereoData_t &&stereoData);
    };

} // ne

#endif //IOManager_IOManager_HPP