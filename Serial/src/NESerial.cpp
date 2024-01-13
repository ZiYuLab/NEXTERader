//
// Created by ziyu on 11/16/23.
//

#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <cstdio>
#include <vector>
#include <iostream>
#include <chrono>

#include "NEInit.h"
#include "NESerial.h"
#include "RmCRC.h"

namespace ne
{

    std::mutex NESerial::mutSerialWriteBuffers_;
    NESerial::NESerial(char *DevicePlace)
    :DevicePath_(DevicePlace)
    {

        if (!init())
        {
            open_ = false;
            ASSERT(0); // Serial Port OpenError
        }
        else
        {
            open_ = true;
        }

    }

    int NESerial::init()
    {
        printf("\n  Loading Serial Port...");
        fd_ = open(DevicePath_, O_RDWR | O_NOCTTY);

        if (fd_ == -1) /* Error Checking */
        {
            printf("\n  Error! in Opening %s", DevicePath_);
            return 0;
        }

        else
            printf("\n  %s Opened Successfully ", DevicePath_);

        termios SerialPortSettings; //串口配置结构体
        tcgetattr(fd_, &SerialPortSettings);

        //设置波特率
        cfsetispeed(&SerialPortSettings, B115200);
        cfsetospeed(&SerialPortSettings, B115200);

        //设置没有校验
        SerialPortSettings.c_cflag &= ~PARENB;

        //停止位 = 1
        SerialPortSettings.c_cflag &= ~CSTOPB;
        SerialPortSettings.c_cflag &= ~CSIZE;

        //设置数据位 = 8
        SerialPortSettings.c_cflag |= CS8;

        SerialPortSettings.c_cflag &= ~CRTSCTS;
        SerialPortSettings.c_cflag |= CREAD | CLOCAL;

        //关闭软件流动控制
        SerialPortSettings.c_iflag &= ~(IXON | IXOFF | IXANY);

        //设置操作模式
        SerialPortSettings.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);

        SerialPortSettings.c_oflag &= ~OPOST;

        // 应用设置
        tcsetattr(fd_,TCSANOW,&SerialPortSettings);

        printf("\n +----------------------------------+");
        printf("\n |        Serial Port Open!         |");
        printf("\n +----------------------------------+");

        printf("\n");

        return 1;
    }

    int NESerial::send(NE_8U *buffer, int count)
    {
        if (isOpen())
        {
            if (debugWrite_)
            {
                printf("\n\n  debug:\n  ");
                for (int i = 0; i < count; i++)
                {
                    if (buffer[i] < 16)
                        printf("0x0%0X ", buffer[i]);
                    else
                        printf("0x%0X ", buffer[i]);
                }
                printf("\n  %d-bytes \n", count);
            }
            else
            {
                // printf("\n--------");
                if (showWrite_)
                    printf("\n  ->");
                int bytes_written;
                bytes_written = write(fd_, buffer, count);
                if (bytes_written != count)
                {
                    return 0;
                }
                else
                    return bytes_written;
            }
        }
        else
        {
            ASSERT(0); // Serial Port OpenError
            return 0;
        }

    }

    int NESerial::send2Judge(NE_16U cmdID, std::vector<NE_8U> *message, int count)
    {
        if (isOpen())
        {
            std::vector<NE_8U> buffer;

            // header -> SOF 1-byte
            buffer.push_back(0xA5);

            // header -> data_length 2-byte
            ne16U2Ne8U.ne16UIN = count;
            buffer.push_back(ne16U2Ne8U.ne8uOUT[1]);
            buffer.push_back(ne16U2Ne8U.ne8uOUT[0]); // 发送顺序待处理

            // header -> seq
            buffer.push_back(0); // 包序号待处理

            // header CRC-8 1-byte
            buffer.push_back(Get_CRC8_Check_Sum(&buffer[0], buffer.size()));

            // cmdID
            ne16U2Ne8U.ne16UIN = cmdID;
            buffer.push_back(ne16U2Ne8U.ne8uOUT[1]);
            buffer.push_back(ne16U2Ne8U.ne8uOUT[0]);

            // data
            for (int i = 0; i < count; i++)
            {
                buffer.push_back((*message)[i]);
            }

            const NE_16U crc16Result = Get_CRC16_Check_Sum(&buffer[0], buffer.size());
            ne16U2Ne8U.ne16UIN = crc16Result;

            // frame_tail CRC-16
            buffer.push_back(ne16U2Ne8U.ne8uOUT[1]);
            buffer.push_back(ne16U2Ne8U.ne8uOUT[0]);

            send(&buffer[0], buffer.size());
        }
        else
        {
            ASSERT(0); // Serial Port OpenError
            return 0;
        }

    }

    NESerial::~NESerial()
    {
        stopAllThread();
        close(fd_);
    }

    bool NESerial::isOpen() const
    {
        return open_;
    }

    void NESerial::debugWrite()
    {
        open_ = true;
        debugWrite_ = true;
    }

    void NESerial::showWrite()
    {
        showWrite_ = true;
    }

    int NESerial::genWriteThreat(int frequency, NE_16U cmdId, std::vector<std::vector<NE_8U>> buffers, int countEach, int type, int mode)
    {
        if (isOpen())
        {
            if (type == NE_THREAT_TYPE_JUDGE)
            {
                if (mode == NE_SERIAL_THREAT_RUN)
                {
                    mutSerialWriteBuffers_.lock();
                    buffersGroup_.push_back(buffers);
                    mutSerialWriteBuffers_.unlock();
                    buffersId_++;

                    std::thread writeThread(modelThreadWriteJudge, frequency, cmdId, &buffersGroup_[buffersId_ - 1], this, countEach);
                    writeThread.detach();
                    return buffersId_;

                }
            }
            printf("\n Write Thread Generate OK in buffer ID:%d", buffersId_ - 1);
            putchar('\n');
        }
        else
        {
            ASSERT(0); // Serial Port OpenError
            printf("\n  Can not run thread because the serial is not running!");
        }
        return 0;
    }

    // thread model -- private

    void NESerial::modelThreadWriteJudge(int frequency, NE_16U cmdID, std::vector<std::vector<NE_8U>> *buffers, NESerial *thisPtr, int countEach)
    {
        //printf("\nin");
        //printf("\nin");
        unsigned long count = 0;
        int i = 0;

        // count = buffers->size();
        auto start = std::chrono::high_resolution_clock::now();

        while (true)
        {
            mutSerialWriteBuffers_.lock();
            if (!thisPtr->threadRun_)
            {
                break;
            }
            mutSerialWriteBuffers_.unlock();
            // thisPtr->mut_.unlock();

            if (buffers->size() == 0) {
                continue;
            }
            else
            {
                if (i < buffers->size()) {

                    if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() > 1000 / frequency)
                    {
                        NESerial::mutSerialWriteBuffers_.lock();

                        thisPtr->send2Judge(cmdID, &(*buffers)[i], countEach);
                        start = std::chrono::high_resolution_clock::now();

                        NESerial::mutSerialWriteBuffers_.unlock();
                        ++i;
                    }
                }
                else
                {
                    i = 0;
                }
            }
        }
    }

    void NESerial::updateToWriteBuffer(int bufferID, std::vector<std::vector<NE_8U>> data)
    {
        mutSerialWriteBuffers_.lock();

        if (buffersGroup_.size() < bufferID - 1)
        {
            printf("\n This Buffer ID is not found!");
            ASSERT(0); // This buffer ID is not found!
        }
        //while(1);
        buffersGroup_[bufferID - 1] = data;
        mutSerialWriteBuffers_.unlock();

    }

    void NESerial::stopAllThread()
    {
        threadRun_ = false;
    }

    // ----串口读----

    int NESerial::readOne(NE_8U &message)
    {
        return read(fd_, &message, 1);
    }

    void NESerial::clearInputBuffer() const
    {
        tcflush(fd_, TCIFLUSH);
    }

    void NESerial::clearOutputBuffer() const
    {
        tcflush(fd_, TCOFLUSH);
    }


}