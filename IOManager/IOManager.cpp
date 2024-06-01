//
// Created by ziyu on 24-5-7.
//

#include "IOManager.hpp"
#include "RmCRC.h"
#include "NEInit.h"
#include "chrono"

namespace ne
{
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


    void Listener::onReadEvent(const char *portName, unsigned int readBufferLen)
    {
        spdlog::info(">>>>>{}", readBufferLen);
        if (readBufferLen >= (9 + sizeof(radar_mark_data_t)))
        {
            auto data = new uint8_t [readBufferLen];
            if (data)
            {
                p_sp->readData(data, readBufferLen);

                if (Verify_CRC8_Check_Sum(data, 5) && Verify_CRC16_Check_Sum(data, 15))
                {
                    Uint16T2Uint8T_u uint16T2Uint8Tu;
                    uint16T2Uint8Tu.out[0] = data[5];
                    uint16T2Uint8Tu.out[1] = data[6];
                    if (uint16T2Uint8Tu.in == 0x020D)
                    {
                        memcpy(&radarMarkData_, data + 7, sizeof(radar_mark_data_t));

                    }
                }

                readBufferSize_ = 0; // 数据复制前置0,防止异步状况下,出现size != 0 但是地址为空情况
                readBufferPtr_.reset(data);
                readBufferSize_ = readBufferLen;
            }
        }
        //
//        std::cout << readBufferLen << std::endl;
//        spdlog::info("Have Data {}", readBufferLen);
        return;
        // std::cout << readBufferSize_ << std::endl;
        if (readBufferLen)
        {
            auto data = new uint8_t [readBufferLen];
            if (data)
            {
                p_sp->readData(data, readBufferLen);


                readBufferSize_ = 0; // 数据复制前置0,防止异步状况下,出现size != 0 但是地址为空情况
                readBufferPtr_.reset(data);
                readBufferSize_ = readBufferLen;
            }
        }
    }








    IOManager* IOManager::getIOManagerInstance(Our_t our)
    {
        static IOManager instance_(our);
        return &instance_;
    }

    void IOManager::init()
    {
        auto timeNow = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
        long long timestamp = timeNow.count();
        std::string videoFileLName = "../lib/logs/IOLogger-" + Stamp2Time(timestamp) + ".txt";
        IOFileLogger_ = spdlog::basic_logger_mt("sbasic_logger", videoFileLName.c_str());


        sp_ = new itas109::CSerialPort();

        // TODO 修改为读取配置文件
        sp_->init("/dev/ttyUSB0", itas109::BaudRate115200); // 初始化

        //listener_ = new Listener(sp_);
        printf("\e[0;34m[SERIAL_INFO]\e[0m %s\n", sp_->getVersion());

        // --------------帧头的生成----------------


        // Robot Position
        robotPositionHead_.emplace_back(0xA5);
        uint16T2Uint8T_.in = 10; // 长度
        robotPositionHead_.emplace_back(uint16T2Uint8T_.out[0]);
        robotPositionHead_.emplace_back(uint16T2Uint8T_.out[1]);
        robotPositionHead_.emplace_back(0); // 包序号
        robotPositionHead_.emplace_back(Get_CRC8_Check_Sum(robotPositionHead_.data(), robotPositionHead_.size(), 0xff));

        //CMD ID
        uint16T2Uint8T_.in = JUDGE_CMD_ROBOT_POSITION;
        robotPositionHead_.emplace_back(uint16T2Uint8T_.out[0]);
        robotPositionHead_.emplace_back(uint16T2Uint8T_.out[1]);

        if (!Verify_CRC8_Check_Sum(robotPositionHead_.data(), 5))
            spdlog::error("Serial Head Position CRC8 ERROR!");

        // Radar

        radarCmdHead_.emplace_back(0xA5);
        uint16T2Uint8T_.in = 7; // 长度
        radarCmdHead_.emplace_back(uint16T2Uint8T_.out[0]);
        radarCmdHead_.emplace_back(uint16T2Uint8T_.out[1]);
        radarCmdHead_.emplace_back(0); // 包序号
        radarCmdHead_.emplace_back(Get_CRC8_Check_Sum(radarCmdHead_.data(), radarCmdHead_.size(), 0xff));

        //CMD ID
        uint16T2Uint8T_.in = JUDGE_CMD_ROBOT_INTERACTION;
        radarCmdHead_.emplace_back(uint16T2Uint8T_.out[0]);
        radarCmdHead_.emplace_back(uint16T2Uint8T_.out[1]);

        if (!Verify_CRC8_Check_Sum(radarCmdHead_.data(), 5))
            spdlog::error("Serial Head Radar CRC8 ERROR!");

    }

    IOManager::~IOManager()
    {
        stopAll();
    }

    void IOManager::stopAll()
    {

        if (writeThreadPtr_)
        {
            threadRun_ = false;
            writeThreadPtr_->join();
            readThreadPtr_->join();
        }
        sp_->close();
        //delete listener_;
        delete sp_;
        printf("\e[0;34m[SERIAL_INFO]\e[0m Serial Port Stopped!\n");
    }

    void IOManager::writeThread(IOManager *thisPtr)
    {
        auto robotPositionIter = thisPtr->robotStereoDataBuffer_.begin();
        ASSERT(robotPositionIter != thisPtr->robotStereoDataBuffer_.end());
        auto lastSentRobotPositionTimePoint = std::chrono::high_resolution_clock::now();

        while (thisPtr->threadRun_)
        {
            auto timeNow = std::chrono::high_resolution_clock::now();

            // TODO 10HZ 写死！！！！
            if (std::chrono::duration<double, std::milli>(timeNow - lastSentRobotPositionTimePoint).count() > (1000.0 / 10.0f))
            {
//
//                spdlog::info(">>>>{}", std::chrono::duration<double, std::milli>(timeNow - lastSentRobotPositionTimePoint).count());
                //char *test = "A";
                //thisPtr->sp_->writeData(test, 1);

                for (int i = 1; !robotPositionIter->have; i++)
                {
                    robotPositionIter++;
                    if (robotPositionIter == thisPtr->robotStereoDataBuffer_.end())
                        robotPositionIter = thisPtr->robotStereoDataBuffer_.begin();

                    // 如果遍历以后都没有找到数据，就推出
                    if (i >= thisPtr->robotStereoDataBuffer_.size())
                    {
                        break;
                    }
                }
                if (robotPositionIter->have)
                {
                    thisPtr->mapRobotData2Uint8T_.in.target_position_x = robotPositionIter->Pm[0];
                    thisPtr->mapRobotData2Uint8T_.in.target_position_y = robotPositionIter->Pm[1];
                    thisPtr->mapRobotData2Uint8T_.in.target_robot_id = robotPositionIter->classID;


                    auto robotOutMessage = thisPtr->robotPositionHead_;
                    robotOutMessage.emplace_back(thisPtr->mapRobotData2Uint8T_.out[0]);
                    robotOutMessage.emplace_back(thisPtr->mapRobotData2Uint8T_.out[1]);
                    robotOutMessage.emplace_back(thisPtr->mapRobotData2Uint8T_.out[2]);
                    robotOutMessage.emplace_back(thisPtr->mapRobotData2Uint8T_.out[3]);
                    robotOutMessage.emplace_back(thisPtr->mapRobotData2Uint8T_.out[4]);
                    robotOutMessage.emplace_back(thisPtr->mapRobotData2Uint8T_.out[5]);
                    robotOutMessage.emplace_back(thisPtr->mapRobotData2Uint8T_.out[6]);
                    robotOutMessage.emplace_back(thisPtr->mapRobotData2Uint8T_.out[7]);
                    robotOutMessage.emplace_back(thisPtr->mapRobotData2Uint8T_.out[8]);
                    robotOutMessage.emplace_back(thisPtr->mapRobotData2Uint8T_.out[9]);

                    thisPtr->uint16T2Uint8T_.in = Get_CRC16_Check_Sum(robotOutMessage.data(),
                                                                         robotOutMessage.size(), 0xffff);
                    robotOutMessage.emplace_back(thisPtr->uint16T2Uint8T_.out[0]);
                    robotOutMessage.emplace_back(thisPtr->uint16T2Uint8T_.out[1]);
//                    for (auto each : robotOutMessage)
//                    {
//                        printf("%X ", each);
//                    }
//                    printf("\n");

                    if (!Verify_CRC16_Check_Sum(robotOutMessage.data(), robotOutMessage.size()))
                        return;
                    const int num = thisPtr->sp_->writeData(robotOutMessage.data(), robotOutMessage.size());
//                    std::cout << num << std::endl;
                    if (num > 0)
                        lastSentRobotPositionTimePoint = timeNow;

                }
            }
            //            const int sentNum = serial->writeData(writeBuffer.get(), writeBufferSize);
        }
    }

    void IOManager::readThread(IOManager *thisPtr)
    {
        int lastRadarInfo = 0;
        thisPtr->robotInteractionRadarData_.radar_cmd = 0;

        while (thisPtr->threadRun_)
        {
            uint8_t dataOne;
            if (thisPtr->sp_->readData(&dataOne, 1) > 0)
            {
                if (dataOne == 0XA5)
                {
                    uint8_t dataMessageRadarInfo[10];
                    dataMessageRadarInfo[0] = 0XA5;
                    if (thisPtr->sp_->readData(dataMessageRadarInfo + 1, 4) == 4)
                    {
                        if (Verify_CRC8_Check_Sum(dataMessageRadarInfo, 5))
                        {
                            Uint16T2Uint8T_u uint16T2Uint8Tu;
                            uint16T2Uint8Tu.out[0] = dataMessageRadarInfo[1];
                            uint16T2Uint8Tu.out[1] = dataMessageRadarInfo[2];
                            //spdlog::info("--{}", uint16T2Uint8Tu.in);
                            if (uint16T2Uint8Tu.in == 1)
                            {
                                if (thisPtr->sp_->readData(dataMessageRadarInfo + 5, 5) == 5)
                                {
                                    if (Verify_CRC16_Check_Sum(dataMessageRadarInfo, 10))
                                    {
                                        uint8_t radarInfo = dataMessageRadarInfo[7];

                                        thisPtr->IOFileLogger_->info("[IO-READ][JUDGE-RADAR-INFO]:{}->{}", radarInfo, (radarInfo >> 6));

                                        if ((radarInfo >> 6) == 1)
                                        {
                                            if (lastRadarInfo == 0)
                                            {
                                                thisPtr->robotInteractionRadarData_.radar_cmd++;
                                            }
                                            else if (lastRadarInfo == 1)
                                            {

                                            }
                                            lastRadarInfo = 1;
                                        }
                                        else if ((radarInfo >> 6) == 2)
                                        {
                                            if (lastRadarInfo == 0 || lastRadarInfo == 1)
                                            {
                                                thisPtr->robotInteractionRadarData_.radar_cmd++;
                                            }
                                            else if (lastRadarInfo == 2)
                                            {

                                            }
                                            lastRadarInfo = 2;


                                        }
                                        else
                                            lastRadarInfo = 0;

                                        // 激活双倍易伤
                                        if ((radarInfo >> 6) > 0)
                                        {
                                            auto radarCmdMessage = thisPtr->radarCmdHead_;
                                            thisPtr->robotInteractionRadarData_.data_cmd_id = 0x0121;
//                                            if (thisPtr->)
                                            if (thisPtr->our_ == OUR_RED)
                                                thisPtr->robotInteractionRadarData_.sender_id = 9;
                                            if (thisPtr->our_ == OUR_BLUE)
                                                thisPtr->robotInteractionRadarData_.sender_id = 109;

                                            thisPtr->robotInteractionRadarData_.receiver_id = 0x8080;

                                            robotInteractionRadarData2Uint8_u robotInteractionRadarData2Uint8U;
                                            robotInteractionRadarData2Uint8U.in = thisPtr->robotInteractionRadarData_;
                                            radarCmdMessage.emplace_back(robotInteractionRadarData2Uint8U.out[0]);
                                            radarCmdMessage.emplace_back(robotInteractionRadarData2Uint8U.out[1]);
                                            radarCmdMessage.emplace_back(robotInteractionRadarData2Uint8U.out[2]);
                                            radarCmdMessage.emplace_back(robotInteractionRadarData2Uint8U.out[3]);
                                            radarCmdMessage.emplace_back(robotInteractionRadarData2Uint8U.out[4]);
                                            radarCmdMessage.emplace_back(robotInteractionRadarData2Uint8U.out[5]);
                                            radarCmdMessage.emplace_back(robotInteractionRadarData2Uint8U.out[6]);


                                            thisPtr->uint16T2Uint8T_.in = Get_CRC16_Check_Sum(radarCmdMessage.data(),
                                                                                              radarCmdMessage.size(), 0xffff);
                                            radarCmdMessage.emplace_back(thisPtr->uint16T2Uint8T_.out[0]);
                                            radarCmdMessage.emplace_back(thisPtr->uint16T2Uint8T_.out[1]);

                                            thisPtr->IOFileLogger_->info("[IO-SENT][JUDGE-RADAR-CMD]:{}", thisPtr->robotInteractionRadarData_.radar_cmd);

                                            if (!Verify_CRC16_Check_Sum(radarCmdMessage.data(), radarCmdMessage.size()))
                                                return;
                                            const int num = thisPtr->sp_->writeData(radarCmdMessage.data(), radarCmdMessage.size());

//                                            if (Verify_CRC16_Check_Sum(radarCmdMessage.data(), 16))
//                                                spdlog::info(">>>>>");

//                                            for (auto each : radarCmdMessage)
//                                            {
//                                                printf("%X ", each);
//                                            }
//                                            std::cout << std::endl;
//
//                                            if (num == 16)
//                                                spdlog::info("--->>>>");
                                        }

//                                        spdlog::info("cmd {:0x}", thisPtr->robotInteractionRadarData_.radar_cmd);
//                                        spdlog::info("re ID {:0x}", thisPtr->robotInteractionRadarData_.receiver_id);
//                                        spdlog::info("se ID {:0x}", thisPtr->robotInteractionRadarData_.sender_id);
//                                        spdlog::info("da CMD {:0x}", thisPtr->robotInteractionRadarData_.data_cmd_id);

                                    }
                                }
                            }
                            //if (Verify_CRC16_Check_Sum(dataMessagePtr, 15))

                        }
                    }
                }
            }
        }

    }

    void IOManager::robotPositionUpdate(RobotStereoData_t &&stereoData)
    {
//        writeMutex_.lock();
        robotStereoDataBuffer_ = stereoData;
//        writeMutex_.unlock();
    }

    bool IOManager::open()
    {

        //const bool status = true;
        //sp_->connectReadEvent(listener_);

        if (!sp_->open())
        {

            printf("\e[1;31m[SERIAL_ERROR]\e[0m Serial Open Fail With ERROR_CODE: %d\n", sp_->getLastError());
            //printf("\e[1;31m[SERIAL_ERROR]\e[0m Serial Open Fail!\n");
            return false;
        }
        else
        {
            writeThreadPtr_ = new std::thread(writeThread, this);
            readThreadPtr_ = new std::thread(readThread, this);

            printf("\e[0;34m[SERIAL_INFO]\e[0m Serial Port Opened! \n");
            return true;
        }
//    return status;
    }

    IOManager::IOManager(Our_t our)
    {
        our_ = our;
    }


} // ne