//
// Created by ziyu on 24-5-7.
//

#include "Tracker.hpp"
#include "NEInit.h"


namespace ne {
    Tracker::Tracker(NEConfig &config)
        :config_(&config)
    {

        our_ = config.our();
        if (our_ == OUR_RED)
        {
            robotData_.at(0).classID = 101;
            robotData_.at(1).classID = 102;
            robotData_.at(2).classID = 103;
            robotData_.at(3).classID = 104;
            robotData_.at(4).classID = 105;
            robotData_.at(5).classID = 107;
        }
        else if (our_ == OUR_BLUE)
        {
            robotData_.at(0).classID = 1;
            robotData_.at(1).classID = 2;
            robotData_.at(2).classID = 3;
            robotData_.at(3).classID = 4;
            robotData_.at(4).classID = 5;
            robotData_.at(5).classID = 7;
        }
        else
        {
            spdlog::error("Our Not Config!");
            ASSERT(0);
        }

        LOG_INFO("Get car armour index!")
        for(YAML::const_iterator it= config_->getConfig()["set_car_armour"].begin(); it != config_->getConfig()["set_car_armour"].end(); ++it)
        {
            carArmourIndex_.emplace_back(it->second.as<int>());
        }

        for (int i = 0; i < carArmourIndex_.size(); ++i)
        {
            if (carArmourIndex_[i] == 0)
            {
                carIndex_ = i;
                goto finish;
            }
        }

        LOG_ERROR("CarArmourIndex set ERROR!, Place Check!");
        ASSERT(0);
        finish:
        LOG_SUCCESS("Finish!");
        return ;
    }

    void Tracker::track()
    {
        for (auto iter = robotData_.begin(); iter != robotData_.end(); iter++)
        {
            // 左右相机都没有数据

            float distanceMin = 1000000;
            bool foundL = false;
            for (auto iterL = robotDataBufferL_.begin(); iterL != robotDataBufferL_.end(); iterL++)
            {
                if (iter->classID != iterL->classID)
                    continue;

                else
                {
                    // 没有就添加
                    if (!iter->L.have)
                    {
                        iter->have = true;
                        iter->L = *iterL;
                        foundL = true;
                        break;
                    }
                    else
                    {
                        // 找出距离最短的一个跟踪
                        const float tmpDis = (iter->L.Puv - iterL->Puv).norm();
                        if (tmpDis < distanceMin)
                        {
                            distanceMin = tmpDis;
                            iter->L = *iterL;
                            iter->have = true;

                            foundL = true;
                        }
                    }
                }
            }

            // 左侧没找到就输出没找到
            if (!foundL)
            {
                iter->L.have = false;
            }

            distanceMin = 1000000;
            bool foundR = false;
            for (auto iterR = robotDataBufferR_.begin(); iterR != robotDataBufferR_.end(); iterR++)
            {
                if (iter->classID != iterR->classID)
                    continue;

                else
                {
                    // 没有就添加
                    if (!iter->R.have)
                    {
                        iter->have = true;
                        iter->R = *iterR;
                        foundR = true;
                        break;
                    }
                    else
                    {
                        // 找出距离最短的一个跟踪
                        const float tmpDis = (iter->R.Puv - iterR->Puv).norm();
                        if (tmpDis < distanceMin)
                        {
                            distanceMin = tmpDis;
                            iter->R = *iterR;
                            iter->have = true;

                            foundR = true;
                        }
                    }
                }
            }

            // 右侧没找到就输出没找到
            if (!foundR)
            {
                iter->R.have = false;
            }

            // 如果两侧都没找到就输出都没找到
            if (!foundR && !foundL)
            {
                iter->have = false;
            }
        }
    }

    void Tracker::matchArmor(NENet netL, NENet netR)
    {

        ASSERT(netL._result.size()); // net输入错误，可能是没有进行推理
        robotDataBufferL_.clear();

        for (auto each : netL._result[0])
        {
            if (each.classId == carIndex_)
            {
                for (auto eachArmour : netL._result[0])
                {
                    if (eachArmour.classId != carIndex_ && carArmourIndex_.at(eachArmour.classId) != -1) // 筛选出装甲板
                    {
                        // 匹配装甲板
                        if (eachArmour.x1 >= each.x1 && eachArmour.x2 <= each.x2
                            && eachArmour.y1 >= each.y1 && eachArmour.y2 <= each.y2)
                        {
//                            cv::Point2f aimPoint = box2Point(each.x1, each.y1, each.x2, each.y2);

                            Robot_t tmp;
                            tmp.classID = carArmourIndex_.at(eachArmour.classId);
                            tmp.Puv[0] = (each.x1 + each.x2) / 2;
                            tmp.Puv[1] = (each.y1 + each.y2) / 2;
                            tmp.have = true;

                            robotDataBufferL_.emplace_back(tmp);
                        }
                    }
                }
            }
        }

        ASSERT(netR._result.size()); // net输入错误，可能是没有进行推理
        robotDataBufferR_.clear();

        for (auto each : netR._result[0])
        {
            if (each.classId == carIndex_)
            {
                for (auto eachArmour : netR._result[0])
                {
                    if (eachArmour.classId != carIndex_ && carArmourIndex_.at(eachArmour.classId) != -1) // 筛选出装甲板
                    {
                        // 匹配装甲板
                        if (eachArmour.x1 >= each.x1 && eachArmour.x2 <= each.x2
                            && eachArmour.y1 >= each.y1 && eachArmour.y2 <= each.y2)
                        {
//                            cv::Point2f aimPoint = box2Point(each.x1, each.y1, each.x2, each.y2);

                            Robot_t tmp;
                            tmp.classID = carArmourIndex_.at(eachArmour.classId);
                            tmp.Puv[0] = (each.x1 + each.x2) / 2;
                            tmp.Puv[1] = (each.y1 + each.y2) / 2;
                            tmp.have = true;

                            robotDataBufferR_.emplace_back(tmp);
                        }
                    }
                }
            }
        }
    }

    void Tracker::solvePosition(NESolvePosition &solvePositionL, NESolvePosition &solvePositionR, StereoSolvePosition &stereoSolvePosition, NEImgStream &imgStream)
    {
        for (auto iter = robotData_.begin(); iter != robotData_.end(); iter++)
        {
            if (iter->have)
            {
                if (iter->L.have && iter->R.have)
                {
//                    iter->Pm =
                    iter->Pm = (solvePositionL.get2DPoints(iter->L.Puv) + solvePositionR.get2DPoints(iter->R.Puv)) / 2;
                    iter->Pm[0];
//                    std::cout << iter->Pm << std::endl << std::endl;
                }
                else if (iter->L.have)
                {
                    iter->Pm = solvePositionL.get2DPoints((iter->L.Puv));
                }
                else if (iter->R.have)
                {
                    iter->Pm = solvePositionR.get2DPoints((iter->R.Puv));
                }

            }
        }
    }

    RobotStereoData_t Tracker::getRobotData()
    {
        return robotData_;
    }


} // ne