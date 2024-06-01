//
// Created by ziyu on 24-1-25.
//

#ifndef MULTITHREADINGCVFRAMEWORK_FRAMEWORK_H
#define MULTITHREADINGCVFRAMEWORK_FRAMEWORK_H

#include <chrono>
#include <memory>
#include <opencv2/opencv.hpp>

/**
 * 注意
 * 使用框架需继承该类并重写以下三个函数
 * 继承派生类下列三个重写函数其所调用的函数，读取类变量时，父类提供互斥量，请手工加互斥锁 ！！！
 */

#define INPUT_MUTEX (x) { \
    inputMutex.lock()     \
    X;                    \
    inputMutex.unlock()\
}

#define OUTPUT_MUTEX (x) { \
    outputMutex.lock()     \
    X;                    \
    outputMutex.unlock()\
}

#define SHARE_MUTEX (x) { \
    shareMutex.lock()     \
    X;                    \
    shareMutex.unlock()\
}

struct MulFrameMessage_t
{
    cv::Mat frameL;
    cv::Mat frameR;
    std::chrono::high_resolution_clock::time_point capTime;
};

class Framework {


protected:
    std::mutex inputMutex_;
    std::mutex outputMutex_;
    std::mutex shareMutex_;

public:
    Framework() = default;

    /**
     * 重写该函数进行图像采集
     * @param img 输出的图像，采集的结果
     * @return
     */
    virtual bool input(MulFrameMessage_t & imgMessage) = 0;

    /**
     * 重写该函数进行图像的处理和其他算法
     * @param img 输入的图像，框架自动传参
     * @param output 输出，指针需要使用 static_pointer_cast 转化为自定义类型
     * @return
     */
    virtual bool operation(MulFrameMessage_t & imgMessage, std::shared_ptr<void> & output) = 0;

    /**
     * 重写该函数对输出进行处理
     * @param output 输出，是上面函数的结果
     * @param time 运行时间，用于计算帧率
     * @return
     */
    virtual bool output(std::shared_ptr<void> & output, double time) = 0;
};


#endif //MULTITHREADINGCVFRAMEWORK_FRAMEWORK_H

