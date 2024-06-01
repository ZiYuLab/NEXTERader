//
// Created by ziyu on 24-1-25.
//

#ifndef MULTITHREADINGCVFRAMEWORK_MULTHRCV_H
#define MULTITHREADINGCVFRAMEWORK_MULTHRCV_H

#include <queue>
#include <mutex>
#include <thread>
#include <memory>
#include <vector>
#include "Framework.h"
#include <opencv2/opencv.hpp>

#define DEBUG

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//#define DEBUG // <---------------------------
#ifdef DEBUG
#include <assert.h>
#define ASSERT(f) assert(f)
#else
#define ASSERT(f) ((void)0)
#endif
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#ifdef DEBUG
#define LOG_ERROR(X) { \
    std::cout  << "\033[31m" << "[ERROR]: " << X << "\033[0m" << std::endl; \
    ASSERT(0);                                                              \
}
#else
    #define LOG_ERROR(X) { \
    std::cout  << "\033[31m" << "[ERROR]: " << X << "\033[0m" << std::endl; \
    abort();                            \
}
#endif

#define LOG_INFO(X) { \
    std::cout  << "\033[34m" << "[INFO]: " << X << "\033[0m" << std::endl;                     \
}                     \

#define LOG_SUCCESS(X) { \
    std::cout  << "\033[32m" << "[SUCCESS]: " << X << "\033[0m" << std::endl;                     \
}

#define CHECK_NULL(X) { \
    if (X == nullptr)   \
    {                   \
        LOG_ERROR("X" << "is nullptr!")\
    }\
}

#define CHECK_TRUE(X) { \
    if (!(X))   \
    {                   \
        LOG_ERROR("Function:" << "X" << "return false!")\
    }\
}

class MulThrCV {
private:
    int _inputThreadNum = 0;
    int _operationThreadNum = 0;
    int _outputThreadNum = 0;
    int _maxInputQueue = 0;
    int _maxOutputQueue = 0;
    unsigned int _operationIDNow = 0;

    std::queue<unsigned int> _operationIDQueue;
    std::queue<MulFrameMessage_t> _inputQueue;
    std::queue<std::shared_ptr<void>> _outputQueue;

    std::mutex _operationIDMutex;
    std::mutex _inputMutex;
    std::mutex _sharedMutex;
    std::mutex _outputMutex;

    bool _threadStatus = true;
    bool _threadStop = false;

    std::vector<std::shared_ptr<std::thread>> _inputThread;
    std::vector<std::shared_ptr<std::thread>> _operationThread;
    std::vector<std::shared_ptr<std::thread>> _outputThread;

    static void modelInput(MulThrCV * thisPtr);
    static void modelOperation(MulThrCV * thisPtr, unsigned int ID);
    static void modelOutput(MulThrCV * thisPtr);
    Framework * _object = nullptr;

public:


    MulThrCV() = default;

    /**
     *
     * @param object 实际运行对象，供框架调用，需要继承并重写Framework类中的三个虚函数
     * @param maxInputQueue 最大的输入队列长度
     * @param maxOutputQueue 最大的输出队列长度
     */
    MulThrCV(Framework & object, int maxInputQueue, int maxOutputQueue);

    /**
     *
     * @param object 实际运行对象，供框架调用，需要继承并重写Framework类中的三个虚函数
     * @param maxInputQueue 最大的输入队列长度
     * @param maxOutputQueue 最大的输出队列长度
     * @param operationThreadNum 图像处理部分线程数量 注：不是越多越好
     */
    MulThrCV(Framework & object, int maxInputQueue, int maxOutputQueue, int operationThreadNum);

    /**
     * 创建输入线程 不能追加创建，程序中只能有一个！
     * @return
     */
    bool generateInputThread();

    /**
     * 创建输入线程，禁用！ 不能追加创建，程序中只能有一个！
     * @param inputThreadNum
     * @return
     */
    bool generateInputThread(int inputThreadNum);

    /**
     * 创建执行线程
     * @param earlyInEarlyOut 先入先出模式，防止出现后取图像的线程先输出，默认打开，建议打开
     * @return
     */
    bool generateOperationThread(bool earlyInEarlyOut = true);

    /**
     * 创建执行线程,可以追加创建
     * @param operationThreadNum 执行线程数量，可以修改
     * @param earlyInEarlyOut 先入先出模式，防止出现后取图像的线程先输出，默认打开，建议打开
     * @return
     */
    bool generateOperationThread(int operationThreadNum, bool earlyInEarlyOut = true);

    /**
     * 创建输出线程 不能追加创建，程序中只能有一个！
     * @return
     */
    bool generateOutputThread();

    /**
     * 创建输出线程 禁用！ 不能追加创建，程序中只能有一个！
     * @param outputThreadNum
     * @return
     */
    bool generateOutputThread(int outputThreadNum);

    /**
     * 停止所有线程
     */
    void stopAllThread();

    /**
     * 是否已经发出线程停止命令
     * @return
     */
    bool threadIsStop();

    /**
     * 未实装，没用
     */
    void releaseAllResource();
    ~MulThrCV();

};




#endif //MULTITHREADINGCVFRAMEWORK_MULTHRCV_H
