//
// Created by ziyu on 24-1-25.
//

#include "MulThrCV.h"


void MulThrCV::modelInput(MulThrCV *thisPtr)
{
    while (thisPtr->_threadStatus)
    {
        MulFrameMessage_t imgInfo;
        CHECK_NULL(thisPtr->_object);

        // 返回 false 做停止处理
        if(!thisPtr->_object->input(imgInfo))
        {
            LOG_INFO("Stop All Thread Because Function \"input\" return false!");
            thisPtr->_sharedMutex.lock();
            thisPtr->_threadStatus = false;
            thisPtr->_sharedMutex.unlock();
            break;
        }


        thisPtr->_inputMutex.lock();

        const int tmp = thisPtr->_inputQueue.size() - thisPtr->_maxInputQueue;
        for (int i = 0; tmp >= 0 && i < tmp; ++i)
        {
            thisPtr->_inputQueue.pop();
        }
        //if (!img.empty()) printf("OK\n");
        thisPtr->_inputQueue.push(imgInfo);
        thisPtr->_inputMutex.unlock();
    }

    thisPtr->_sharedMutex.lock();
    LOG_INFO("Input Thread Stopped!");
    thisPtr->_sharedMutex.unlock();
}

void MulThrCV::modelOperation(MulThrCV *thisPtr, unsigned int ID)
{
    while (thisPtr->_threadStatus)
    {
        MulFrameMessage_t imgMessage;

        thisPtr->_inputMutex.lock();
        if (!thisPtr->_inputQueue.empty())
        {
            if (ID)
            {
                thisPtr->_operationIDMutex.lock();
                thisPtr->_operationIDQueue.push(ID);
                thisPtr->_operationIDMutex.unlock();
            }

            imgMessage = thisPtr->_inputQueue.front();
            thisPtr->_inputQueue.pop();
            thisPtr->_inputMutex.unlock();
            //printf("1\n");
        }
        else
        {
            thisPtr->_inputMutex.unlock();
            continue;
        }


        std::shared_ptr<void> output;

        CHECK_NULL(thisPtr->_object);
        if(!thisPtr->_object->operation(imgMessage, output))
        {
            LOG_INFO("Stop All Thread Because Function \"operation\" return false!");
            thisPtr->_sharedMutex.lock();
            thisPtr->_threadStatus = false;
            thisPtr->_sharedMutex.unlock();
            break;
        }

        if (ID)
        {
            while (true)
            {
                thisPtr->_operationIDMutex.lock();
                if (thisPtr->_operationIDQueue.front() == ID)
                {
                    thisPtr->_operationIDQueue.pop();
                    thisPtr->_outputMutex.lock();
                    const int tmp = thisPtr->_outputQueue.size() - thisPtr->_maxOutputQueue;
                    for (int i = 0; tmp >= 0 && i < tmp; ++i)
                    {
                        thisPtr->_outputQueue.front().reset();
                        thisPtr->_outputQueue.pop();
                    }
                    thisPtr->_outputQueue.push(output);
                    thisPtr->_outputMutex.unlock();
                    thisPtr->_operationIDMutex.unlock();
                    break;
                }
                thisPtr->_operationIDMutex.unlock();
            }

        }

    }

    thisPtr->_sharedMutex.lock();
    if (ID)
    {
        LOG_INFO("Operation Thread:" << ID << " Stopped!");
    }
    else
    {
        LOG_INFO("Operation Thread Stopped!");
    }

    thisPtr->_sharedMutex.unlock();
}

void MulThrCV::modelOutput(MulThrCV *thisPtr)
{
    auto begin = std::chrono::high_resolution_clock::now();
    auto end = std::chrono::high_resolution_clock::now();
    while (thisPtr->_threadStatus)
    {
        thisPtr->_outputMutex.lock();
        std::shared_ptr<void> output;
        if (!thisPtr->_outputQueue.empty())
        {
            output = thisPtr->_outputQueue.front();
            thisPtr->_outputQueue.pop();
            thisPtr->_outputMutex.unlock();
        }
        else
        {
            thisPtr->_outputMutex.unlock();
            //printf("1\n");
            continue;
        }

        CHECK_NULL(thisPtr->_object);
        begin = end;
        end = std::chrono::high_resolution_clock::now();
        if(!thisPtr->_object->output(output, (std::chrono::duration<double, std::milli>(end - begin).count())))
        {
            LOG_INFO("Stop All Thread Because Function \"output\" return false!");
            thisPtr->_sharedMutex.lock();
            thisPtr->_threadStatus = false;
            thisPtr->_sharedMutex.unlock();
            break;
        }
    }

    thisPtr->_sharedMutex.lock();
    LOG_INFO("Output Thread Stopped!");
    thisPtr->_sharedMutex.unlock();
}


MulThrCV::MulThrCV(Framework & object, int maxInputQueue, int maxOutputQueue)
    :_object(&object), _maxInputQueue(maxInputQueue), _maxOutputQueue(maxOutputQueue)
{
    CHECK_NULL(_object);
}

MulThrCV::MulThrCV(Framework & object,int maxInputQueue, int maxOutputQueue, int operationThreadNum)
    :_object(&object), _maxInputQueue(maxInputQueue), _maxOutputQueue(maxOutputQueue), _inputThreadNum(1),
    _operationThreadNum(operationThreadNum), _outputThreadNum(1)
{
    CHECK_NULL(_object);
}

bool MulThrCV::generateInputThread()
{
    return generateInputThread(_inputThreadNum);
}

bool MulThrCV::generateInputThread(int inputThreadNum)
{
    _inputThreadNum = inputThreadNum = 1; // 后续开发
    if (inputThreadNum <= 0)
    {
        LOG_ERROR("Input Thread <= 0!");
    }
    else if (_maxInputQueue <= 0)
    {
        LOG_ERROR("Max Input Queue <= 0!");
    }
    else
    {
        // 初始化线程状态
        _threadStatus = true;
        _threadStop = false;
        _inputThread.clear();
        for (int i = 0; i < inputThreadNum; ++i)
        {
            std::shared_ptr<std::thread> thread(new std::thread(modelInput, this));
            _inputThread.emplace_back(thread);
        }
    }
    return true;
}

bool MulThrCV::generateOperationThread(bool earlyInEarlyOut)
{
    return generateOperationThread(_operationThreadNum, earlyInEarlyOut);
}

bool MulThrCV::generateOperationThread(int operationThreadNum, bool earlyInEarlyOut)
{
    unsigned int ID = 0;
    _operationThreadNum = operationThreadNum;
    if (_operationThreadNum <= 0)
    {
        LOG_ERROR("Input Thread <= 0!");
    }
    else if (_maxInputQueue <= 0)
    {
        LOG_ERROR("Max Input Queue <= 0!");
    }
    else if (_maxOutputQueue <= 0)
    {
        LOG_ERROR("Max Output Queue <= 0!");
    }
    else
    {
        // 初始化线程状态
        _threadStatus = true;
        _threadStop = false;
        _operationThread.clear();
        for (int i = 0; i < operationThreadNum; ++i)
        {
            if (earlyInEarlyOut)
            {
                ID = ++_operationIDNow;
            }
            std::shared_ptr<std::thread> thread(new std::thread(modelOperation, this, ID));
            _operationThread.emplace_back(thread);
        }
    }
    return true;
}

bool MulThrCV::generateOutputThread()
{
    return generateOutputThread(_outputThreadNum);
}

bool MulThrCV::generateOutputThread(int outputThreadNum)
{
    _outputThreadNum = outputThreadNum = 1; // 后续开发
    if (_outputThreadNum <= 0)
    {
        LOG_ERROR("Input Thread <= 0!");
    }
    else if (_maxOutputQueue <= 0)
    {
        LOG_ERROR("Max Output Queue <= 0!");
    }
    else
    {
        // 初始化线程状态
        _threadStatus = true;
        _threadStop = false;
        _outputThread.clear();
        for (int i = 0; i < outputThreadNum; ++i)
        {
            std::shared_ptr<std::thread> thread(new std::thread(modelOutput, this));
            _outputThread.emplace_back(thread);
        }
    }
    return true;
}

void MulThrCV::stopAllThread()
{
    _threadStatus = false;
    for (const auto& each : _inputThread)
    {
        if (each->joinable())
        {
            each->join();
            //LOG_INFO("Input Thread Stopped!");
        }

    }
    for (const auto& each : _operationThread)
    {
        if (each->joinable())
        {
            each->join();
            //LOG_INFO("Operation Thread Stopped!");
        }
    }
    for (const auto& each : _outputThread)
    {
        if (each->joinable())
        {
            each->join();
            //LOG_INFO("Output Thread Stopped!");
        }

    }
    _operationIDNow = 0;
    _threadStop = true;
}

void MulThrCV::releaseAllResource()
{

}

MulThrCV::~MulThrCV()
{
    stopAllThread();
}

bool MulThrCV::threadIsStop()
{
    return !_threadStatus;
}

