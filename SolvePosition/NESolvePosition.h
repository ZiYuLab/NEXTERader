//
// Created by ziyu on 24-1-8.
//

#ifndef NEXTERADAR_NESOLVEPOSITION_H
#define NEXTERADAR_NESOLVEPOSITION_H

#include <eigen3/Eigen/Dense>
#include <sophus/so3.hpp>
#include <vector>
#include <opencv2/core/eigen.hpp>
#include <opencv2/opencv.hpp>

#include "NEInit.h"
#include "NEConfig.h"
#include "NENet.h"
#include "NEImgStream.h"
#include "StereoSolveDefine.hpp"
//#include "NERobot.h"
//#include "NERobotPosition.h"

namespace ne
{
    class NESolvePosition{
    private:
        NEConfig config_;
        NEConfig configCam_;
        std::vector<std::vector<Eigen::Vector2f>> _2DLUT; // 二维坐标查找表
//        int _carIndex = 0;
//        std::vector<int> _carArmourIndex;
        bool isInIt(const std::vector<cv::Point3f> & face, Eigen::Vector3f & point);

        /**
        * 按照框框取取位置点
        * @param x1 左上
        * @param y1
        * @param x2 右下
        * @param y2
        * @return
        */
        cv::Point2f box2Point(float x1, float y1, float x2, float y2);

    public:

//        std::vector<ne::robot_t> _result;
        NESolvePosition() = default;
        NESolvePosition(NEConfig & config, NEConfig &configCam);
        ~NESolvePosition();

        bool init(int frameRow, int frameCol, std::vector<std::vector<cv::Point3f>> & mapData, std::vector<cv::Point2f> & camData, StereoParam_t stereoParam, int type); // type 右1 左-1
        Eigen::Vector2f get2DPoints(Eigen::Vector2f Puv);
//        std::vector<ne::robot_t> getResult();
    };

}



#endif //NEXTERADAR_NESOLVEPOSITION_H
