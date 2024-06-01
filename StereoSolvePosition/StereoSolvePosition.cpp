//
// Created by ziyu on 24-5-16.
//

#include "StereoSolvePosition.hpp"
#include <opencv2/core/eigen.hpp>

namespace ne
{
    cv::Mat StereoMatchBM(cv::Mat left, cv::Mat right, cv::Mat Q, cv::Mat &out, cv::Mat &dst16)
    {
        cv::cvtColor(left, left, cv::COLOR_BGR2GRAY);
        cv::cvtColor(right, right, cv::COLOR_BGR2GRAY);
        const int numDisparities = 7;
        auto stereoBM = cv::StereoBM::create(numDisparities * 16 + 16/*视差窗口 16的整数倍*/, 19);   // 创建StereoBM
        // 设置StereoBM参数
        stereoBM->setMinDisparity(0);
        stereoBM->setNumDisparities(numDisparities * 16 + 16);
//    stereoBM->setBlockSize(2 * blockSize + 5);
        //stereoBM->setPreFilterType(cv::StereoBM::PREFILTER_XSOBEL);
        stereoBM->setPreFilterSize(5);
        stereoBM->setPreFilterCap(63);
        //stereoBM->setROI1(RoiL);
        //stereoBM->setROI2(RoiR);
        //stereoBM->setTextureThreshold(10);
        stereoBM->setUniquenessRatio(10);
        stereoBM->setSpeckleWindowSize(90);
        stereoBM->setSpeckleRange(32);
        stereoBM->setDisp12MaxDiff(-1);
        // 计算
        cv::Mat disp, disp8, _3dImage;
        stereoBM->compute(left, right, disp);   // disp: cv::Mat 结果输出变量
        // 转格式  // disp8: cv::Mat 结果输出变量
        disp.convertTo(disp8, CV_8U, 255 / ((numDisparities * 16 + 16) * 16.));
        // 重投影到三维空间     // _3dImage: cv::Mat 结果输出变量
        cv::reprojectImageTo3D(disp, _3dImage, Q, true);

        //cv::GaussianBlur(disp8, disp8, cv::Size(5, 5), 3, 3);
        //std::cout << "---" <<_3dImage.type()<< std::endl;
    out = _3dImage;
    dst16 = disp;
        return disp8;
    }



    StereoSolvePosition::StereoSolvePosition(NEConfig &config, NEConfig &configL, NEConfig &configR)
        :configPtr_(&config), configLPtr_(&configL), configRPtr_(&configR)
    {

        stereoParam_.cameraMatrixLeft_m = (cv::Mat_<double>(3, 3) <<
                configLPtr_->getConfig()["camera_data"]["camera_matrix"]["0"].as<double>(),
                configLPtr_->getConfig()["camera_data"]["camera_matrix"]["1"].as<double>(),
                configLPtr_->getConfig()["camera_data"]["camera_matrix"]["2"].as<double>(),
                configLPtr_->getConfig()["camera_data"]["camera_matrix"]["3"].as<double>(),
                configLPtr_->getConfig()["camera_data"]["camera_matrix"]["4"].as<double>(),
                configLPtr_->getConfig()["camera_data"]["camera_matrix"]["5"].as<double>(),
                configLPtr_->getConfig()["camera_data"]["camera_matrix"]["6"].as<double>(),
                configLPtr_->getConfig()["camera_data"]["camera_matrix"]["7"].as<double>(),
                configLPtr_->getConfig()["camera_data"]["camera_matrix"]["8"].as<double>());


        std::cout << stereoParam_.cameraMatrixLeft_m << std::endl;

        stereoParam_.cameraMatrixRight_m = (cv::Mat_<double>(3, 3) <<
                configRPtr_->getConfig()["camera_data"]["camera_matrix"]["0"].as<double>(),
                configRPtr_->getConfig()["camera_data"]["camera_matrix"]["1"].as<double>(),
                configRPtr_->getConfig()["camera_data"]["camera_matrix"]["2"].as<double>(),
                configRPtr_->getConfig()["camera_data"]["camera_matrix"]["3"].as<double>(),
                configRPtr_->getConfig()["camera_data"]["camera_matrix"]["4"].as<double>(),
                configRPtr_->getConfig()["camera_data"]["camera_matrix"]["5"].as<double>(),
                configRPtr_->getConfig()["camera_data"]["camera_matrix"]["6"].as<double>(),
                configRPtr_->getConfig()["camera_data"]["camera_matrix"]["7"].as<double>(),
                configRPtr_->getConfig()["camera_data"]["camera_matrix"]["8"].as<double>());


        stereoParam_.distCoeffsLeft_m = (cv::Mat_<double>(5, 1) <<
                configLPtr_->getConfig()["camera_data"]["dist_coeffs"]["0"].as<double>(),
                configLPtr_->getConfig()["camera_data"]["dist_coeffs"]["1"].as<double>(),
                configLPtr_->getConfig()["camera_data"]["dist_coeffs"]["2"].as<double>(),
                configLPtr_->getConfig()["camera_data"]["dist_coeffs"]["3"].as<double>(),
                configLPtr_->getConfig()["camera_data"]["dist_coeffs"]["4"].as<double>());

        stereoParam_.distCoeffsRight_m = (cv::Mat_<double>(5, 1) <<
                configRPtr_->getConfig()["camera_data"]["dist_coeffs"]["0"].as<double>(),
                configRPtr_->getConfig()["camera_data"]["dist_coeffs"]["1"].as<double>(),
                configRPtr_->getConfig()["camera_data"]["dist_coeffs"]["2"].as<double>(),
                configRPtr_->getConfig()["camera_data"]["dist_coeffs"]["3"].as<double>(),
                configRPtr_->getConfig()["camera_data"]["dist_coeffs"]["4"].as<double>());


        stereoParam_.rotationMatrix_m = (cv::Mat_<double>(3, 3) <<
                configPtr_->getConfig()["stereo_camera_data"]["R"]["0"].as<double>(),
                configPtr_->getConfig()["stereo_camera_data"]["R"]["1"].as<double>(),
                configPtr_->getConfig()["stereo_camera_data"]["R"]["2"].as<double>(),
                configPtr_->getConfig()["stereo_camera_data"]["R"]["3"].as<double>(),
                configPtr_->getConfig()["stereo_camera_data"]["R"]["4"].as<double>(),
                configPtr_->getConfig()["stereo_camera_data"]["R"]["5"].as<double>(),
                configPtr_->getConfig()["stereo_camera_data"]["R"]["6"].as<double>(),
                configPtr_->getConfig()["stereo_camera_data"]["R"]["7"].as<double>(),
                configPtr_->getConfig()["stereo_camera_data"]["R"]["8"].as<double>());

        stereoParam_.translationVector_m = (cv::Mat_<double>(3, 1) <<
                configPtr_->getConfig()["stereo_camera_data"]["T"]["0"].as<double>(),
                configPtr_->getConfig()["stereo_camera_data"]["T"]["1"].as<double>(),
                configPtr_->getConfig()["stereo_camera_data"]["T"]["2"].as<double>()
                );
    }

    StereoSolvePosition::~StereoSolvePosition()
    {

    }

    bool StereoSolvePosition::init(int sourceCol, int sourceRow)
    {
        ASSERT(sourceCol);
        ASSERT(sourceRow);

        stereoParam_.frameSize = cv::Size(sourceCol, sourceRow);

        cv::stereoRectify(stereoParam_.cameraMatrixLeft_m, stereoParam_.distCoeffsLeft_m,
                          stereoParam_.cameraMatrixRight_m, stereoParam_.distCoeffsRight_m,
                          stereoParam_.frameSize, stereoParam_.rotationMatrix_m, stereoParam_.translationVector_m,
                          stereoParam_.R1, stereoParam_.R2, stereoParam_.P1, stereoParam_.P2, stereoParam_.Q_m, true);


        cv::cv2eigen(stereoParam_.Q_m, stereoParam_.Q);
        Eigen::Matrix3f K1, K2;
        cv::cv2eigen(stereoParam_.cameraMatrixLeft_m, K1);
        cv::cv2eigen(stereoParam_.cameraMatrixRight_m, K2);
        stereoParam_.Q(3, 3) = ((K1(0, 2) - K2(0, 2)) / ( -(1 / stereoParam_.Q(3, 2)))); // 修正下Cx - Cy 错误


        cv::initUndistortRectifyMap(stereoParam_.cameraMatrixLeft_m, stereoParam_.distCoeffsLeft_m,
                                    stereoParam_.R1, stereoParam_.P1, stereoParam_.frameSize, CV_16SC2,
                                    stereoParam_.mapLx_m, stereoParam_.mapLy_m);

        cv::initUndistortRectifyMap(stereoParam_.cameraMatrixRight_m, stereoParam_.distCoeffsRight_m,
                                    stereoParam_.R2, stereoParam_.P2, stereoParam_.frameSize, CV_16SC2,
                                    stereoParam_.mapRx_m, stereoParam_.mapRy_m);

//        cv::cv2eigen(stereoParam_.mapLx_m, stereoParam_.mapLx)


        stereoParam_.isInit = true;
        return true;
    }

    Eigen::Vector2f StereoSolvePosition::solvePosition(Eigen::Vector2f LData, Eigen::Vector2f RData, NEImgStream &imgStream)
    {
        /*cv::Mat out;
        cv::Mat dst16;
        StereoMatchBM(imgStream.frameL, imgStream.frameR, stereoParam_.Q_m, out, dst16);

        auto vc3 = out.at<cv::Vec3f>(cv::Point(LData[0], LData[1]));
        auto dataP = dst16.at<float>(cv::Point(LData[0], LData[1]));
        float x = vc3.val[0];
        float y = vc3.val[1];
        float z = vc3.val[2];

        */

//        int xd = round(vc3d.val[0]);
//
//        if (x == 10000 || y == 10000 || z == 10000 || dataP == 0 || dataP == 255)
//        {
//            return Eigen::Vector2f(-1, -1);
//        }

// TODO
//        stereoParam_.Q(3, 3) = -0.02;

        const float d = (LData[0] - RData[0]);
        Eigen::Vector4f Pwi(LData[0], LData[1], d, 1);
        Pwi = stereoParam_.Q * Pwi;
        Pwi /= Pwi[3];
        Eigen::Vector3f Pw = Pwi.block<3, 1>(0, 0);
//        Eigen::Vector3f Pw(x, y, z);

        std::cout << Pw << std::endl << std::endl;

        // 坐标换算
        Pw /= 1000; //毫米转米
        Pw = R_.transpose() * (Pw - T_);
//        std::cout << Pw << std::endl << std::endl;
//        std::cout << testRe / testRe[3] << std::endl << std::endl;
        return Pw.block<2, 1>(0, 0);
    }

    bool StereoSolvePosition::initCamPose(std::vector<cv::Point2f> & camData)
    {
        spdlog::info("Stereo PnP Start!");
        // 准备pnp数据
        std::vector<cv::Point3f> world;

        auto our = configPtr_->our();

        if (our == OUR_RED)
            for(YAML::const_iterator it= configPtr_->getConfig()["data"]["demarcate_red_points"].begin(); it != configPtr_->getConfig()["data"]["demarcate_red_points"].end(); ++it)
            {
                world.emplace_back(it->second["x"].as<float>(), it->second["y"].as<float>(), it->second["z"].as<float>());
            }
        else if (our == OUR_BLUE)
            for(YAML::const_iterator it= configPtr_->getConfig()["data"]["demarcate_blue_points"].begin(); it != configPtr_->getConfig()["data"]["demarcate_blue_points"].end(); ++it)
            {
                world.emplace_back(it->second["x"].as<float>(), it->second["y"].as<float>(), it->second["z"].as<float>());
            }
        else
        {
            spdlog::error("Our Not Config!");
            ASSERT(0);
        }

        cv::Mat rvec;
        cv::Mat tvec;

        solvePnPRansac(world, camData, stereoParam_.P1(cv::Rect(0, 0, 3, 3)), cv::Mat(), rvec, tvec, false, 100, 2);
        Rodrigues(rvec, rvec);
        cv::cv2eigen(rvec, R_);
        cv::cv2eigen(tvec, T_);

        Eigen::Vector3f Pc = -1 * R_.transpose() * T_; // 计算相机坐标
        spdlog::info("Camera Position Stereo Left>");
        std::cout << Pc << std::endl;

        return true;
    }

} // ne