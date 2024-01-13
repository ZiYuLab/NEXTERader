//
// Created by ziyu on 11/19/23.
//

#include "NERobotPosition.h"

namespace ne
{
    NERobotPosition::NERobotPosition(NEConfig & config)
        :_clearDelayTime(config.getConfig()["robot"]["clearDelayTime"].as<int>())
    {
        positionBufferInit();
    }

    void NERobotPosition::setPosition(int robotID, NE_32F x, NE_32F y)
    {
        if (buffers_.size() >= BUFFERS_MAX_LIMIT)
            ASSERT(0); // 数组超限，你是否调用了clearBuffer();
        else if (buffers_.size() >= robotGroundNum_)
            buffers_.resize(0);

        std::vector<NE_8U> buffer;

        ne16U2Ne8U.ne16UIN = robotID;
        buffer.push_back(ne16U2Ne8U.ne8uOUT[1]);
        buffer.push_back(ne16U2Ne8U.ne8uOUT[0]);

        ne32F2Ne8U.ne32FIN = x;
        buffer.push_back(ne32F2Ne8U.ne8uOUT[3]);
        buffer.push_back(ne32F2Ne8U.ne8uOUT[2]);
        buffer.push_back(ne32F2Ne8U.ne8uOUT[1]);
        buffer.push_back(ne32F2Ne8U.ne8uOUT[0]);

        ne32F2Ne8U.ne32FIN = y;
        buffer.push_back(ne32F2Ne8U.ne8uOUT[3]);
        buffer.push_back(ne32F2Ne8U.ne8uOUT[2]);
        buffer.push_back(ne32F2Ne8U.ne8uOUT[1]);
        buffer.push_back(ne32F2Ne8U.ne8uOUT[0]);

        buffers_.push_back(buffer);
        // mutSerialWriteBuffers.unlock();
    }

    std::vector<std::vector<NE_8U >> NERobotPosition::exportBuffers() const
    {
        return buffers_;
    }

    void NERobotPosition::clearBuffers()
    {
        buffers_.resize(0);
    }

    NERobotPosition::~NERobotPosition()
    {

    }

    void NERobotPosition::updatePosition(int robotID, cv::Point2f position)
    {
        if (_timeCou >= _clearDelayTime)
        {
            _timeCou = 0;
            positionBufferInit();
        }
        _robotPositionBuffer[(robotID / 100) * 7 + robotID % 100].ID = robotID;
        _robotPositionBuffer[(robotID / 100) * 7 + robotID % 100].position = position;
    }

    void NERobotPosition::positionBufferInit()
    {
        _robotPositionBuffer.resize(15);
        for (auto each : _robotPositionBuffer)
        {
            each.ID = -1;
            each.position = cv::Point2f(-1, -1);
        }
    }

    void NERobotPosition::oneLoop()
    {
        _timeCou++;
        // std::cout << _timeCou << std::endl;
    }


}

