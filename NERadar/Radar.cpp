////
//// Created by ziyu on 24-5-20.
////
//
//#include "Radar.hpp"
//
//
//namespace ne
//{
//    Radar::Radar()
//    {
//        SHOW_NE;
//
//        CHECK_PATH("../Config/configL.yaml");
//        auto configL = NEConfig("../Config/configL.yaml", false);
//        CHECK_PATH("../Config/configR.yaml");
//        auto configR = NEConfig("../Config/configR.yaml", false);
//
//        CHECK_PATH("../Config/config.yaml");
//        auto config = NEConfig("../Config/config.yaml");
//
//        stream_ = NEImgStream(config);
//
//        // 验证以下，图像大小是否正确
//        if ((stream_.sourceInfoL.col != stream_.sourceInfoR.col) || (stream_.sourceInfoL.row != stream_.sourceInfoR.row))
//            spdlog::warn("Img Size Not = !, USE Left Camera");
//        StereoSolvePosition stereoSolvePosition(config, configL, configR);
//
//        // 初始化双目
//        stereoSolvePosition.init(stream_.sourceInfoL.col, stream_.sourceInfoL.row);
//        // 双目矫正开启
//        stream_.initStereo(stereoSolvePosition.stereoParam_);
//
//        NEDemarcatePose demarcatePose(config);
//
//        NENet neNetL(config);
//
//        NESolvePosition solvePositionL(config, configL);
////    NERobotPosition robotPositionL(configL);
//
//        NENet neNetR(config);
//        NESolvePosition solvePositionR(config, configR);
//        Tracker carTracker(config);
//
//
//        IOManager *ioManagerPtr = IOManager::getIOManagerInstance();
//        ioManagerPtr->init();
//        ioManagerPtr->open();
//
////    while (true)
////    {
////        stream.getFrame();
////        cv::Mat frame = StereoMatchBM(stream.frameL, stream.frameR, stereoSolvePosition.stereoParam_.Q_m);
////        cv::imshow("1212", frame);
////        cv::waitKey(1);
////    }
//        // 读取地图数据
//        LOG_INFO("Read map data!");
//        auto mapData = config.getMapData();
//        stream_.drawMapDataConfig(mapData); // 画出地图数据
//        stream_.show("检查地图数据, 回车进入", NE_STREAM_MAP, 0.5);
//        stream_.waitKey(13, 0); // ENTER
//
//        // 场地标定 LEFT
//        if (!demarcatePose.isDemarcate())
//        {
//            LOG_INFO("Start to demarcate LEFT!");
//            demarcatePose.startDemarcate(stream_, CAM_LEFT);
//
//            LOG_INFO("Start to demarcate RIGHT!");
//            demarcatePose.startDemarcate(stream_, CAM_RIGHT);
//        }
//
//        stereoSolvePosition.initCamPose(demarcatePose.demarcatePointLeft_);
//
//        LOG_INFO("Solve Position Data Init Left start!");
//
//        // 初始化查找表和网络索引 LEFT
//        LOG_INFO(demarcatePose.demarcatePointRight_.size());
//
//        // 分别为左边的和右边的解算初始化
//        solvePositionL.init(stream_.sourceInfoL.row, stream_.sourceInfoL.col, mapData, demarcatePose.demarcatePointLeft_, stereoSolvePosition.stereoParam_, -1);
//        solvePositionR.init(stream_.sourceInfoR.row, stream_.sourceInfoR.col, mapData, demarcatePose.demarcatePointRight_, stereoSolvePosition.stereoParam_, 1);
//        LOG_SUCCESS("Finish!")
//
//
//        neNetL.netInit();
//        neNetR.netInit();
//
//        LOG_SUCCESS("Init All Finish!");
//
//        LOG_INFO("Please Check All Data And Input 'Y/y' to Start Or 'N/n' to Stop!");
//        char input;
//        std::cin >> input;
//        if (!(input == 'Y' || input == 'y'))
//        {
//            LOG_INFO("Exit!");
//            abort();
//        }
//
//        SHOW_NE;
//        LOG_INFO("START!");
//    }
//
//    bool Radar::input(MulFrameMessage_t &imgMessage)
//    {
//        if (!stream_.getFrame())
//            break;
//        return true;
//    }
//} // ne