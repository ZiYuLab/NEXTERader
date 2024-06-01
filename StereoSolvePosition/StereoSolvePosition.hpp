//
// Created by ziyu on 24-5-16.
//

#ifndef NEXTERADAR_STEREOSOLVEPOSITION_HPP
#define NEXTERADAR_STEREOSOLVEPOSITION_HPP

#include "NEInit.h"
#include "NEConfig.h"
#include <Eigen/Dense>
#include <opencv2/opencv.hpp>
#include "RobotDefine.hpp"
#include "StereoSolveDefine.hpp"
#include "NEImgStream.h"

namespace ne
{

    class StereoSolvePosition
    {
    private:
        NEConfig *configPtr_ = nullptr;
        NEConfig *configLPtr_ = nullptr;
        NEConfig *configRPtr_ = nullptr;

        Eigen::Matrix3f R_;
        Eigen::Vector3f T_;


    public:
        StereoParam_t stereoParam_;

        StereoSolvePosition(NEConfig &config, NEConfig &configL, NEConfig &configR);
        ~StereoSolvePosition();

        bool init(int sourceCol, int sourceRow);
        bool initCamPose(std::vector<cv::Point2f> & camData);
        Eigen::Vector2f solvePosition(Eigen::Vector2f LData, Eigen::Vector2f RData, NEImgStream &imgStream);

    };

} // ne

#endif //NEXTERADAR_STEREOSOLVEPOSITION_HPP