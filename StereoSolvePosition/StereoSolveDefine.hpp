//
// Created by ziyu on 24-5-16.
//

#ifndef NEXTERADAR_STEREOSOLVEDEFINE_HPP
#define NEXTERADAR_STEREOSOLVEDEFINE_HPP

#include <opencv2/opencv.hpp>
#include <Eigen/Dense>

namespace ne
{
    struct StereoParam_t
    {
        bool isInit = false;
        cv::Mat R1, R2, P1, P2, Q_m;
        cv::Mat mapLx_m, mapLy_m, mapRx_m, mapRy_m;
        Eigen::Vector2f mapLx, mapLy, mapRx, mapRy;
        Eigen::Matrix4f Q;
        cv::Mat cameraMatrixLeft_m, cameraMatrixRight_m;
        cv::Mat distCoeffsLeft_m, distCoeffsRight_m;
        cv::Mat translationVector_m, rotationMatrix_m;
        cv::Size frameSize;

        inline void frameStereoReMap(cv::Mat &frameL, cv::Mat &frameR)
        {
            cv::remap(frameL, frameL, mapLx_m, mapLy_m, cv::INTER_LINEAR);
            cv::remap(frameR, frameR, mapRx_m, mapRy_m, cv::INTER_LINEAR);
        }
    };
}

#endif //NEXTERADAR_STEREOSOLVEDEFINE_HPP
