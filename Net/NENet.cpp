//
// Created by ziyu on 24-1-5.
//

#include "NENet.h"

#include <string>
#include <vector>

#include "NEInit.h"


ne::NENet::NENet(ne::NEConfig & config)
    : _infer(config.getConfig()["net"]["device"].as<int>()), _config(config)
{

}

ne::NENet::~NENet() {

}

bool ne::NENet::netInit()
{
    // 从配置中取出模型参数
    const std::string onnx_path = _config.getConfig()["net"]["onnx_path"].as<std::string>();
    const std::string trt_path = _config.getConfig()["net"]["trt_path"].as<std::string>();
    const int batch_size = _config.getConfig()["net"]["batch_size"].as<int>();
    const int num_class = _config.getConfig()["net"]["num_class"].as<int>();
    const int input_h = _config.getConfig()["net"]["input_h"].as<int>();
    const int input_w = _config.getConfig()["net"]["input_w"].as<int>();



    // 初始化模型 没有trt则创建trt

    LOG_INFO("Init Module...")

    CHECK_PATH(onnx_path.c_str());
    // CHECK_PATH(trt_path.c_str());
    if (access(trt_path.c_str(), F_OK))
    {
        LOG_INFO("Create trt Engine, Please wait...");
        nvinfer1::IHostMemory *data = _infer.createEngine(onnx_path, batch_size, input_h, input_w);
        if (data == nullptr)
        {
            LOG_ERROR("Create Engine Fail!");
            return false;
        }
        _infer.saveEngineFile(data, trt_path);
    }

    bool success = _infer.initModule(trt_path, batch_size, num_class);
    if (!success)
    {
        LOG_ERROR("Init Module Fail!");
        return false;
    }
    else
    {
        _isInit = true;
        LOG_SUCCESS("Module Init Success!");
        return true;
    }
}

bool ne::NENet::doInference(cv::Mat &src)
{
    std::vector<cv::Mat> inputBuffer;
    inputBuffer.emplace_back(src);
    const auto conf_threshold = _config.getConfig()["net"]["conf_threshold"].as<float>();
    const auto nms_threshold = _config.getConfig()["net"]["nms_threshold"].as<float>();
    _result = _infer.doInference(inputBuffer, conf_threshold, conf_threshold, nms_threshold);
    if (_result.empty())
    {
        LOG_ERROR("Do Inference Fail!");
    }
    else
        return true;
}
