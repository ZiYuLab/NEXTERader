//
// Created by ziyu on 24-5-7.
//

#ifndef RADARTRACKER_TRACKER_HPP
#define RADARTRACKER_TRACKER_HPP

#include "RobotDefine.hpp"
#include "NEConfig.h"
#include "NENet.h"
#include "NESolvePosition.h"
#include "StereoSolvePosition.hpp"
#include "NEImgStream.h"

namespace ne
{

    class Tracker {
    private:
        NEConfig *config_;
        Our_t our_ = OUR_NULL; // TODO
        RobotStereoData_t robotData_ = RobotStereoData_t(6); // TODO
        RobotData_t robotDataBufferL_, robotDataBufferR_;
        std::vector<int> carArmourIndex_;
        int carIndex_ = 0;

    public:
        Tracker(NEConfig &config);

        void matchArmor(NENet netL, NENet netR);
        void track();
        void solvePosition(NESolvePosition &solvePositionL, NESolvePosition &solvePositionR, StereoSolvePosition &stereoSolvePosition, NEImgStream &imgStream);

        RobotStereoData_t getRobotData();

    };

} // ne

#endif //RADARTRACKER_TRACKER_HPP