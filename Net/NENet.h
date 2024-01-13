//
// Created by ziyu on 24-1-5.
//

#include <opencv2/opencv.hpp>
#include "NEInit.h"
#include "NEConfig.h"
#include "TRTInfer/include/Inference.h""

#ifndef NEXTERADAR_NENET_H
#define NEXTERADAR_NENET_H


namespace ne
{
    class NENet {
    private:
        TRTInferV1::TRTInfer _infer;
        NEConfig _config;
        bool _isInit = false;

    public:
        std::vector<std::vector<TRTInferV1::DetectionObj>> _result;

        NENet() = default;
        NENet(NEConfig & config);
        ~NENet();
        bool netInit();
        bool doInference(cv::Mat & src);
    };
}


#endif //NEXTERADAR_NENET_H
