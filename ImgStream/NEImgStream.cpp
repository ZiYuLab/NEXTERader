//
// Created by ziyu on 24-1-5.
//

#include "NEImgStream.h"

#include <opencv2/opencv.hpp>

ne::NEImgStream::NEImgStream(ne::NEConfig &config)
    :_config(config)
{
    for(YAML::const_iterator it= _config.getConfig()["class_name"].begin(); it != _config.getConfig()["class_name"].end(); ++it)
    {
        className_.emplace_back(it->second.as<std::string>());
    }


    const auto type = config.getConfig()["source"]["type"].as<std::string>();
    const auto mapPath = config.getConfig()["source"]["map_path"].as<std::string>();

    // 读取地图
    CHECK_PATH(mapPath.c_str());
    _mapSource = cv::imread(mapPath);
    if (_mapSource.empty())
    {
        LOG_ERROR("Map Img Read Fail!");
        ASSERT(0);
    }
    else
    {
        _map = _mapSource.clone();
        mapInfo.row = _map.rows;
        mapInfo.col = _map.cols;
        LOG_SUCCESS("Read Map Success!");
    }

    if (type == "camera")
    {
        _type = TYPE_T::CAMERA;

        int leftID = -1, rightID = -1;
        spdlog::warn("输入左侧相机ID: \n>>>");
        std::cin >> leftID;

        spdlog::warn("输入右侧相机ID: \n>>>");
        std::cin >> rightID;

        ASSERT( !(leftID == -1));
        ASSERT( !(rightID == -1));

        Hik.startDevice(leftID, rightID);
//        Hik.StartDevice(_config.getConfig()["camara"]["device"].as<int>()); //打开0号相机
//        Hik.SetResolution(_config.getConfig()["camara"]["resolution"]["width"].as<int>(), //设置分辨率
//                          _config.getConfig()["camara"]["resolution"]["height"].as<int>());
//        Hik.SetPixelFormat(_config.getConfig()["camara"]["pixel_format"].as<int>()); //设置像素格式
//        Hik.SetPixelFormat(PixelType_Gvsp_BayerBG8); //设置像素格式
//        Hik.SetExposureTime(_config.getConfig()["camara"]["exposure_time"].as<int>()); //设置曝光时间
//        Hik.SetGAIN(_config.getConfig()["camara"]["gain"].as<float>()); //设置增益
//        Hik.SetFrameRate(_config.getConfig()["camara"]["frame_rate"].as<int>()); //设置帧率上限
//        Hik.SetStreamOn();//开始取流


        //sleep(5);
        Hik.getFrame(frameL, frameR); //获得图像
        if (frameL.empty() || frameR.empty())
        {
            LOG_ERROR("Camara Stream Error!")
            ASSERT(0);
            return ;
        }

        sourceInfoL.row = frameL.rows;
        sourceInfoR.row = frameR.rows;
        sourceInfoL.col = frameL.cols;
        sourceInfoR.col = frameR.cols;

        spdlog::info("{} - {}", sourceInfoL.row, sourceInfoL.col);
    }
    else if (type == "video")
    {
        _type = TYPE_T::VIDEO;
        auto pathL = _config.getConfig()["source"]["video_L_path"].as<std::string>();
        auto pathR = _config.getConfig()["source"]["video_R_path"].as<std::string>();

        CHECK_PATH(pathL.c_str());
        CHECK_PATH(pathR.c_str());

        capL_.open(pathL);
        capR_.open(pathR);

        if (!capL_.isOpened())
            LOG_ERROR("Video File Open Fail!");

        if (!capR_.isOpened())
        LOG_ERROR("Video File Open Fail!");

        sourceInfoL.row = (int)capL_.get(cv::CAP_PROP_FRAME_HEIGHT);
        sourceInfoL.col = (int)capL_.get(cv::CAP_PROP_FRAME_WIDTH);

        sourceInfoR.row = (int)capR_.get(cv::CAP_PROP_FRAME_HEIGHT);
        sourceInfoR.col = (int)capR_.get(cv::CAP_PROP_FRAME_WIDTH);
    }
    else if (type == "img")
    {
        ASSERT(0);
    }
    else if (type == "img_dir")
    {
        _type = TYPE_T::IMG_DIR;
        ASSERT(0);
    }
    else if (type == "camera_usb")
    {
//        _type = TYPE_T::CAMERA_USB;
//        int usbCamID = _config.getConfig()["source"]["usb_camera_path"].as<int>();
//        //CHECK_PATH(_path.c_str());
//        _cap.open(usbCamID);
//        // 一些设置，还有一定问题
//        _cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
//        _cap.set(cv::CAP_PROP_BRIGHTNESS, 20);
//        _cap.set(cv::CAP_PROP_FPS, 30);
//        _cap.set(cv::CAP_PROP_FRAME_WIDTH, 1280);
//        _cap.set(cv::CAP_PROP_FRAME_HEIGHT, 960);
//
//        if (!_cap.isOpened())
//        LOG_ERROR("USB Camera Open Fail!");
//
//        sourceInfo.row = (int)_cap.get(cv::CAP_PROP_FRAME_HEIGHT);
//        sourceInfo.col = (int)_cap.get(cv::CAP_PROP_FRAME_WIDTH);
        ASSERT(0);
    }
    else
    {
        _type = TYPE_T::NULL_TYPE;
        LOG_ERROR("Source Type Error!");
    }


    mapXMax = _config.getConfig()["data"]["map_x_max"].as<int>();
    mapYMax = _config.getConfig()["data"]["map_y_max"].as<int>();
}

ne::NEImgStream::~NEImgStream()
{

}

bool ne::NEImgStream::getFrame()
{
    _map = _mapSource.clone();

    switch (_type) {
        case CAMERA:
        {
            while (!Hik.getFrame(frameL, frameR))
            {
            }//获得图像
            if (frameL.empty() || frameR.empty())
            {
                LOG_ERROR("Camara Stream Error!")
                ASSERT(0);
                return false;
            }
            else
            {
//                // 双目矫正
//                if (stereoParamPtr_)
//                    stereoParamPtr_->frameStereoReMap(frameL, frameR);
                return true;
            }
//            ASSERT(0);
        }
        case VIDEO:
        {
            capL_ >> frameL;
            capR_ >> frameR;

            if (frameL.empty() || frameR.empty())
            {
                LOG_INFO("Video End!");
                return false;
            }
            // 双目矫正
//            if (stereoParamPtr_)
//                stereoParamPtr_->frameStereoReMap(frameL, frameR);
            return true;
        }
        case IMG:
        {
//            _frame = cv::imread(_path);
//            if (_frame.empty())
//            {
//                LOG_ERROR("Read Img Error!");
//                return false;
//            }
//            else
//            {
//                return true;
//            }
            ASSERT(0);
        }
        case IMG_DIR:
        {
            ASSERT(0);
            return false;
        }
        case CAMERA_USB:
        {
//            _cap >> _frame;
//            if (_frame.empty())
//            {
//                LOG_INFO("Video End!");
//                return false;
//            }
//            return true;
            ASSERT(0);
        }
        default:
        {
            LOG_ERROR("Source Type Error!");
            ASSERT(0);
            return false;
        }

    }


}

void ne::NEImgStream::nameWindow(const std::string & windowName)
{
    cv::namedWindow(windowName);
}

void ne::NEImgStream::show(const std::string& windowName, StreamType_e type)
{
    if (type == NE_STREAM_MAP)
    {
        cv::imshow(windowName, _map);
    }
    else if (type == NE_STREAM_SOURCE_LEFT)
    {
        cv::imshow(windowName, frameL);
    }
    else
        cv::imshow(windowName, frameR);
}

void ne::NEImgStream::show(const std::string& windowName, StreamType_e type, double zoom)
{
    cv::Mat dst, src;
    if (type == NE_STREAM_MAP)
        src = _map;
    else if (type == NE_STREAM_SOURCE_LEFT)
        src = frameL;
    else
        src = frameR;

    cv::resize(src, dst, cv::Size(0, 0), zoom, zoom);

    cv::imshow(windowName, dst);
}

void ne::NEImgStream::timerStart()
{
    _begin = std::chrono::high_resolution_clock::now();
}

void ne::NEImgStream::timerEnd()
{
    auto end = std::chrono::high_resolution_clock::now();
    _time = (std::chrono::duration<double, std::milli>(end - _begin).count());
}

double ne::NEImgStream::getTime()
{
    return _time;
}

double ne::NEImgStream::getFPS()
{
    return 1000.0 / _time;
}

void ne::NEImgStream::drawFPS()
{
    const std::string str = "FPS: " + std::to_string((int)std::round(getFPS()));
    cv::putText(frameL, str, cv::Point(20, 50), cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar(0 ,255, 0), 1);
    cv::putText(frameR, str, cv::Point(20, 50), cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar(0 ,255, 0), 1);
}

bool ne::NEImgStream::waitKey(char key, int time)
{
    if (time > 0)
    {
        if (key == cv::waitKey(time))
            return true;
        else
            return false;
    }
    else if(time == 0)
    {
        while (true)
        {
            if (key == cv::waitKey(time))
                return true;
        }
    }
    else
    {
        LOG_ERROR("'waitKey method' argument time Invalid!");
        ASSERT(0);
    }
}

void ne::NEImgStream::drawBoxL(ne::NENet &boxSrc)
{
    const auto RED = cv::Scalar(0, 0, 255);
    const auto BLUE = cv::Scalar(255, 0, 0);
    const auto GREEN = cv::Scalar(0, 255, 0);
    const int LINE_THICKNESS = 1;
    const double FONT_SCALE = 3;



    auto color = RED;
    for (int i = 0; i < boxSrc._result[0].size(); ++i)
    {
        if (boxSrc._result[0][i].classId == 0 || boxSrc._result[0][i].classId == 11 || boxSrc._result[0][i].classId == 12)
            color = BLUE;
        else
            color = RED;

        cv::line(frameL, cv::Point(boxSrc._result[0][i].x1, boxSrc._result[0][i].y1),
                 cv::Point(boxSrc._result[0][i].x2, boxSrc._result[0][i].y1), color, LINE_THICKNESS);
        cv::line(frameL, cv::Point(boxSrc._result[0][i].x2, boxSrc._result[0][i].y2),
                 cv::Point(boxSrc._result[0][i].x2, boxSrc._result[0][i].y1), color, LINE_THICKNESS);
        cv::line(frameL, cv::Point(boxSrc._result[0][i].x2, boxSrc._result[0][i].y2),
                 cv::Point(boxSrc._result[0][i].x1, boxSrc._result[0][i].y2), color, LINE_THICKNESS);
        cv::line(frameL, cv::Point(boxSrc._result[0][i].x1, boxSrc._result[0][i].y1),
                 cv::Point(boxSrc._result[0][i].x1, boxSrc._result[0][i].y2), color, LINE_THICKNESS);

        cv::putText(frameL, className_[boxSrc._result[0][i].classId], cv::Point(boxSrc._result[0][i].x2, boxSrc._result[0][i].y1),
                    cv::FONT_HERSHEY_PLAIN, 1, GREEN);

    }

}

void ne::NEImgStream::drawBoxR(ne::NENet &boxSrc)
{
    const auto RED = cv::Scalar(0, 0, 255);
    const auto BLUE = cv::Scalar(255, 0, 0);
    const auto GREEN = cv::Scalar(0, 255, 0);
    const int LINE_THICKNESS = 1;
    const double FONT_SCALE = 3;

    auto color = RED;
    for (int i = 0; i < boxSrc._result[0].size(); ++i)
    {
        if (boxSrc._result[0][i].classId == 0 || boxSrc._result[0][i].classId == 11 || boxSrc._result[0][i].classId == 12)
            color = BLUE;
        else
            color = RED;

        cv::line(frameR, cv::Point(boxSrc._result[0][i].x1, boxSrc._result[0][i].y1),
                 cv::Point(boxSrc._result[0][i].x2, boxSrc._result[0][i].y1), color, LINE_THICKNESS);
        cv::line(frameR, cv::Point(boxSrc._result[0][i].x2, boxSrc._result[0][i].y2),
                 cv::Point(boxSrc._result[0][i].x2, boxSrc._result[0][i].y1), color, LINE_THICKNESS);
        cv::line(frameR, cv::Point(boxSrc._result[0][i].x2, boxSrc._result[0][i].y2),
                 cv::Point(boxSrc._result[0][i].x1, boxSrc._result[0][i].y2), color, LINE_THICKNESS);
        cv::line(frameR, cv::Point(boxSrc._result[0][i].x1, boxSrc._result[0][i].y1),
                 cv::Point(boxSrc._result[0][i].x1, boxSrc._result[0][i].y2), color, LINE_THICKNESS);

        cv::putText(frameR, className_[boxSrc._result[0][i].classId], cv::Point(boxSrc._result[0][i].x2, boxSrc._result[0][i].y1),
                    cv::FONT_HERSHEY_PLAIN, 1, GREEN);

    }

}

void ne::NEImgStream::drawMapDataConfig(std::vector<std::vector<cv::Point3f>> & mapDataConfig)
{
    const cv::Scalar COLOR(0, 255, 0);
    const int mapXMax = _config.getConfig()["data"]["map_x_max"].as<int>();
    const int mapYMax = _config.getConfig()["data"]["map_y_max"].as<int>();

    for (const auto& face : mapDataConfig)
    {
        for (int i = 0; i < face.size(); ++i)
        {
            int a, b;
            if (i == face.size() - 1)
            {
                a = 0;
                b = face.size() - 1;
            }
            else
            {
                a = i;
                b = i + 1;
            }
            // 坐标变换 实际坐标转为照片
            cv::Point first;
            first.x = face[a].x * (float)((float)mapInfo.col / (float)mapXMax);
            //printf("-------%f\n", first.x);
            first.y = mapInfo.row - face[a].y * ((float)mapInfo.row / (float)mapYMax);
            cv::Point second;
            second.x = face[b].x * ((float)mapInfo.col / (float)mapXMax);
            second.y = mapInfo.row - face[b].y * ((float)mapInfo.row / (float)mapYMax);

            cv::circle(_map, first, 5, COLOR, 5);
            cv::circle(_map, second, 5, COLOR, 5);
            cv::line(_map, first, second, COLOR, 4, 4);
        }
    }

}

void ne::NEImgStream::drawMap(RobotStereoData_t &&mapSrc)
{
    const auto RED = cv::Scalar(0, 0, 255);
    const auto BLUE = cv::Scalar(255, 0, 0);
    const auto GREEN = cv::Scalar(255, 0, 0);
    cv::Scalar color;
    int ID = 0;
    for (auto each : mapSrc)
    {
        if (each.have)
        {
            if (each.classID < 0)
                continue;
            else if (each.classID > 100)
            {
                color = BLUE;
                ID = each.classID - 100;
            }
            else
            {
                color = RED;
                ID = each.classID;
            }

            cv::Point Puv;
            Puv.x = each.Pm[0] * (float)((float)mapInfo.col / (float)mapXMax) ;
            Puv.y = mapInfo.row - each.Pm[1] * ((float)mapInfo.row / (float)mapYMax);
            cv::circle(_map, Puv, 20, color, 40);
            cv::putText(_map, std::to_string(ID), cv::Point(Puv.x - 10, Puv.y + 10), 4, cv::FONT_HERSHEY_PLAIN, cv::Scalar(0, 255, 0), 3);
        }
    }
}

void ne::NEImgStream::initStereo(ne::StereoParam_t &stereoParam)
{
    if (stereoParam.isInit)
        stereoParamPtr_ = &stereoParam;
    else
        stereoParamPtr_ = nullptr;
    haveInitStereo_ = true;
}


