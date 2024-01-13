//
// Created by ziyu on 24-1-5.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "NEInit.h"
#include "NEConfig.h"

ne::NEConfig::NEConfig(char *configPath)
    :_configPath(configPath)
{
    _config = YAML::LoadFile(configPath);
}

ne::NEConfig::NEConfig()
{

}

void ne::NEConfig::setConfigPath(char *configPath)
{
    _configPath = configPath;
}

void ne::NEConfig::updateConfig()
{
    _config = YAML::LoadFile(_configPath);
}

YAML::Node ne::NEConfig::getConfig()
{
    return _config;
}

ne::NEConfig::~NEConfig()
{

}

std::vector<std::vector<cv::Point3f>> ne::NEConfig::getMapData(bool showMapData)
{
    std::vector<std::vector<cv::Point3f>> mapData;

    const auto fileName = _config["data"]["map_data_path"].as<std::string>();

    std::ifstream csv_data(fileName, std::ios::in);

    CHECK_PATH(fileName.c_str());

    if (!csv_data.is_open())
    {
        LOG_ERROR("Open '" << fileName << "' Fail!");
    }
    else
    {
        std::string lineX, lineY, lineZ;

        std::istringstream sinX;
        std::istringstream sinY;
        std::istringstream sinZ;

        // 直接读取三行数据
        while (std::getline(csv_data, lineX) && std::getline(csv_data, lineY) && std::getline(csv_data, lineZ))
        {
            sinX.clear();
            sinY.clear();
            sinZ.clear();

            sinX.str(lineX);
            sinY.str(lineY);
            sinZ.str(lineZ);

            cv::Point3f point; // 记录一个点
            std::vector<cv::Point3f> face; // 记录一个面
            std::string wordX, wordY, wordZ;

            // 读取每一个坐标
            while (std::getline(sinX, wordX, ',') && std::getline(sinY, wordY, ',') && std::getline(sinZ, wordZ, ','))
            {
//                if (wordX.empty() || wordY.empty() || wordZ.empty())
//                {
//                    break;
//                }
//                else
                if (wordX == "\r" || wordY == "\r" || wordZ == "\r")
                {
                    LOG_ERROR("The MapData file may be explore from windows, place use: \n"
                              "\"  sed -i 's/.$//' filename  \" to transform!");
                }
                else if (!(wordX.empty() || wordY.empty() || wordZ.empty()))
                {
                    // 单位换算 mm -> m
                    const auto middleRobotHeight = _config["robot"]["middle_robot_height"].as<float>();
                    point.x = std::stof(wordX) / 1000.0f;
                    point.y = std::stof(wordY) / 1000.0f;
                    point.z = middleRobotHeight + std::stof(wordZ) / 1000.0f;

                    face.emplace_back(point);
                }
            }
            mapData.emplace_back(face);

            // 清除每个面数据间的空行
            std::getline(csv_data, lineX);
            lineX.clear();
        }
    }

    if (showMapData)
    {
        LOG_INFO("Map Data:");
        for (int i = 0; i < mapData.size(); i++)
        {
            LOG_INFO(i);
            for (auto j : mapData[i])
            {
                LOG_INFO(j);
            }
            std::cout << "\n\n";
        }
        LOG_INFO("Show End!");
    }

    return mapData;
}
