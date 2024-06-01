//
// Created by ziyu on 24-1-5.
//

#ifndef NEXTERADAR_NECONFIG_H
#define NEXTERADAR_NECONFIG_H

#include <opencv2/opencv.hpp>
#include <vector>
#include "yaml-cpp/yaml.h"
#include "NEInit.h"

namespace ne
{
    class NEConfig {
    private:
        char * _configPath = nullptr;
        YAML::Node _config;
        Our_t our_ = OUR_NULL;

    public:
        NEConfig(char * configPath, bool haveOur = true);
        NEConfig();
        ~NEConfig();
        void setConfigPath(char * configPath);
        void updateConfig();
        YAML::Node getConfig();

        Our_t our();

        /**
         * 获取地图数据
         * @param showMapData = true 显示地图数据，默认false
         * @return
         */
        std::vector<std::vector<cv::Point3f>> getMapData(bool showMapData = false); // 读取地图数据CSV文件
    };
}



#endif //NEXTERADAR_NECONFIG_H
