//
// Created by ziyu on 24-1-12.
//

#ifndef NEXTERADAR_NEROBOT_H
#define NEXTERADAR_NEROBOT_H

#include <opencv2/opencv.hpp>

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

    struct robot_t
    {
        int ID = 0;
        cv::Point2f position;
    };
}




#endif //NEXTERADAR_NEROBOT_H
