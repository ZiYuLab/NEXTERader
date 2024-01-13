/**
    Please use the UTF-8 to open this file for Chinese
    雷达串口类
    bits 版 1.0.0
    裁判串口协议版本：v1.5
    -20231124 by ZiYu
*/

#ifndef NESERIAL_NESERIAL_H
#define NESERIAL_NESERIAL_H

#include <thread>


namespace ne
{
#define NE_SERIAL_THREAT_RUN 1
// #define NE_SERIAL_THREAT_GENERATE_ONLY 0

#define NE_THREAT_TYPE_JUDGE 1


    class NESerial {

    private:

        bool open_ = false;
        bool debugWrite_ = false;
        bool showWrite_ = false;
        bool threadRun_ = true;
        char * DevicePath_;
        int fd_;
        int init();
        static std::mutex mutSerialWriteBuffers_;
        std::vector<std::vector<std::vector<NE_8U>>>buffersGroup_;
        int buffersId_ = 0;
        // 线程模板
        static void modelThreadWriteJudge(int frequency, NE_16U cmdID, std::vector<std::vector<NE_8U>> *buffers, NESerial *thisPtr, int countEach);


    public:
        NE16U2NE8U ne16U2Ne8U;
        NE32F2NE8U ne32F2Ne8U;

        void debugWrite(); // Use to make program output to shell

        void showWrite();

        bool isOpen() const;


        NESerial(char *DevicePlace);
        ~NESerial();

        // --------------------串口写----------------------
        // 串口发送，一般不直接使用
        int send(NE_8U *buffer, int count);

        // 以裁判系统串口格式发送一帧数据，注意，message 是待发送纯数据（先发送高八位）
        int send2Judge(NE_16U cmdID, std::vector<NE_8U> *message, int count);

        // 创建写串口线程 参数分别为发送频率，命令ID， 数据组（即多帧待发送数据），每一帧数据的大小，后两个参数默认即可，现没有开发
        // 返回值是bufferID
        int genWriteThreat(int frequency, NE_16U cmdId, std::vector<std::vector<NE_8U>> buffers, int countEach, int type = NE_THREAT_TYPE_JUDGE, int mode = NE_SERIAL_THREAT_RUN);

        // 更新数据
        void updateToWriteBuffer(int bufferID, std::vector<std::vector<NE_8U>> data);

        // --------------------串口读----------------------
        int readOne(NE_8U &message);

        int readFrame(std::vector<NE_8U> &buffer);

        void clearInputBuffer() const;
        void clearOutputBuffer() const;

        // 停止所有线程
        void stopAllThread();

    };
}

#endif //NESERIAL_NESERIAL_H
