//
// Created by ziyu on 24-1-5.
//

#ifndef NEXTERADAR_NEIMGSTREAM_H
#define NEXTERADAR_NEIMGSTREAM_H

#include "NEInit.h"

#include <opencv2/opencv.hpp>
#include <string>
#include <chrono>
#include <vector>

#include "NENet.h"
#include "NEConfig.h"
#include "NESolvePosition.h"
#include "NERobot.h"
#include "HikCam.hpp"

namespace ne
{

#define NE_STREAM_MAP 1
#define NE_STREAM_SOURCE 2

    class NEImgStream {

    private:
        enum TYPE_T {
            CAMERA,
            VIDEO,
            IMG,
            CAMERA_USB,
            IMG_DIR,
            NULL_TYPE
        } _type;
        std::string _path;
        //int _usb_camID = 0;
        cv::Mat _mapSource;
        cv::Mat _map;
        cv::VideoCapture _cap;
        ne::NEConfig _config;
        std::chrono::high_resolution_clock::time_point _begin;
        double _time = 0;
        HikCam Hik;


    public:

        struct info_t{
            int row = 0, col = 0;
        } sourceInfo, mapInfo;

        cv::Mat _frame;

        NEImgStream() = default;
        NEImgStream(NEConfig &config);

        ~NEImgStream();

        /**
         * 取一帧
         * @return 当流截止或取失败时返回false
         */
        bool getFrame();

        /**
         * 用于先命名再显示
         * @param windowName
         */
        void nameWindow(const std::string & windowName);

        /**
         * 显示一帧
         * @param windowName
         * @param type NE_STREAM_MAP NE_STREAM_SOURCE
         */
        void show(const std::string& windowName, int type);

        /**
         * 按照缩放比例显示一帧
         * @param windowName
         * @param type 同上
         * @param zoom
         */
        void show(const std::string& windowName, int type, double zoom);

        /**
         * 开启计时器
         */
        void timerStart();

        /**
         * 结束计时
         */
        void timerEnd();

        /**
         * 获取时间
         * @return 返回时间/单位毫秒
         */
        double getTime();

        /**
         * 获取帧率
         * @return 返回帧率/单位帧
         */
        double getFPS();

        /**
         * 画出帧数
         */
        void drawFPS();

        /**
         * 等待time毫秒检测是否有键盘输入=key 是则返回true 否则返回false
         * @param key
         * @param time 延时时间 = 0时为永远延时
         * @return
         */
        bool waitKey(char key, int time);

        /**
         * 画框框
         * @param boxSrc
         */
        void drawBox(NENet & boxSrc);

        /**
         * 将地图数据文件读取后的内容画在小地图上供检验
         * @param mapDataConfig
         */
        void drawMapDataConfig(std::vector<std::vector<cv::Point3f>> & mapDataConfig);

        /**
         * 画地图
         * @param mapSrc
         */
        void drawMap(std::vector<robot_t> & mapSrc, NENet & boxSrc);
    };
}

#endif //NEXTERADAR_NEIMGSTREAM_H
