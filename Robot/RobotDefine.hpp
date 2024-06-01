//
// Created by ziyu on 24-5-7.
//

#ifndef RADARTRACKER_ROBOTDEFINE_HPP
#define RADARTRACKER_ROBOTDEFINE_HPP

#include <Eigen/Dense>
#include <vector>
#include "Inference.h"

namespace ne
{

#define NE_ROBOT_RED_HERO 1
#define NE_ROBOT_RED_ENGINEER 2
#define NE_ROBOT_RED_INFANTRY_3 3
#define NE_ROBOT_RED_INFANTRY_4 4
#define NE_ROBOT_RED_INFANTRY_5 5
#define NE_ROBOT_RED_SENTRY 7

#define NE_ROBOT_BLUE_HERO 101
#define NE_ROBOT_BLUE_ENGINEER 102
#define NE_ROBOT_BLUE_INFANTRY_3 103
#define NE_ROBOT_BLUE_INFANTRY_4 104
#define NE_ROBOT_BLUE_INFANTRY_5 105
#define NE_ROBOT_BLUE_SENTRY 107

    struct Robot_t
    {
        bool have = false;
        int classID = 0;
        Eigen::Vector2f Puv;
    };

    struct RobotStereo_t
    {
        bool have = false;
        int classID = 0;
        Eigen::Vector2f Pm;
        Robot_t L;
        Robot_t R;
    };

    struct DetectRobotResult
    {
        float conf;
    };

    typedef std::vector<Robot_t> RobotData_t;
    typedef std::vector<RobotStereo_t> RobotStereoData_t;
    typedef std::vector<TRTInferV1::DetectionObj> RobotDetectData_t;


}



#endif //RADARTRACKER_ROBOTDEFINE_HPP
