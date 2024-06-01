//
// Created by ziyu on 24-1-8.
//

#include "NESolvePosition.h"

bool ne::NESolvePosition::isInIt(const std::vector<cv::Point3f> & face, Eigen::Vector3f & point)
{
    const float dalta = 0;
    // 利用叉积点积判断是否在平面内
    Eigen::Vector3f thisP3D(face[0].x, face[0].y, face[0].z); // 这个点
    Eigen::Vector3f nextP3D(face[1].x, face[1].y, face[1].z); // 下个点
    Eigen::Vector3f last3D; // 上次叉乘结果
    Eigen::Vector3f this3D; // 本次叉乘结果

    this3D = (nextP3D - thisP3D).cross(point - thisP3D);

    for (int i = 1; i < face.size() - 1; ++i)
    {
        thisP3D = nextP3D;
        last3D = this3D;
        nextP3D << face[i + 1].x, face[i + 1].y, face[i + 1].z; // 下个点
        this3D = (nextP3D - thisP3D).cross(point - thisP3D);

        //std::cout << last3D << std::endl << "---" << std::endl << this3D << std::endl << std::endl;

        //std::cout << "---|" << this3D.dot(last3D) << "|---" << std::endl;

        if (this3D.dot(last3D) < dalta)
            return false;

    }

    thisP3D = nextP3D;
    last3D = this3D;
    nextP3D << face[0].x, face[0].y, face[0].z; // 下个点
    this3D = (nextP3D - thisP3D).cross(point - thisP3D);

    //std::cout << last3D << std::endl << "---" << std::endl << this3D << std::endl << std::endl;

    //std::cout << "---|" << this3D.dot(last3D) << "|---" << std::endl;

    if (this3D.dot(last3D) < dalta)
        return false;

    return true;
}

cv::Point2f ne::NESolvePosition::box2Point(float x1, float y1, float x2, float y2)
{
    const float delta = 2;
    return cv::Point2f((x1 + x2) / 2.0, (y1 + y2) / delta);
}


ne::NESolvePosition::NESolvePosition(ne::NEConfig & config, NEConfig &configCam)
    :config_(config), configCam_(configCam)
{

}

ne::NESolvePosition::~NESolvePosition()
{

}

bool ne::NESolvePosition::init(int frameRow, int frameCol, std::vector<std::vector<cv::Point3f>> & mapData, std::vector<cv::Point2f> & camData, StereoParam_t stereoParam, int type)
{

    // 准备pnp数据
    std::vector<cv::Point3f> world;

    auto our = config_.our();

    if (our == OUR_RED)
        for(YAML::const_iterator it= config_.getConfig()["data"]["demarcate_red_points"].begin(); it != config_.getConfig()["data"]["demarcate_red_points"].end(); ++it)
        {
            world.emplace_back(it->second["x"].as<float>(), it->second["y"].as<float>(), it->second["z"].as<float>());
        }
    else if (our == OUR_BLUE)
        for(YAML::const_iterator it= config_.getConfig()["data"]["demarcate_blue_points"].begin(); it != config_.getConfig()["data"]["demarcate_blue_points"].end(); ++it)
        {
            world.emplace_back(it->second["x"].as<float>(), it->second["y"].as<float>(), it->second["z"].as<float>());
        }
    else
    {
        spdlog::error("Our Not Config!");
        ASSERT(0);
    }

    ASSERT(world.size() == config_.getConfig()["gui"]["demarcate_point_num"].as<int>()); // 标定数据数量不匹配

    std::vector<double> cameraMatrixVec;
    std::vector<double> distCoeffsVec;



    cameraMatrixVec.emplace_back(configCam_.getConfig()["camera_data"]["camera_matrix"]["0"].as<double>());
    cameraMatrixVec.emplace_back(configCam_.getConfig()["camera_data"]["camera_matrix"]["1"].as<double>());
    cameraMatrixVec.emplace_back(configCam_.getConfig()["camera_data"]["camera_matrix"]["2"].as<double>());
    cameraMatrixVec.emplace_back(configCam_.getConfig()["camera_data"]["camera_matrix"]["3"].as<double>());
    cameraMatrixVec.emplace_back(configCam_.getConfig()["camera_data"]["camera_matrix"]["4"].as<double>());
    cameraMatrixVec.emplace_back(configCam_.getConfig()["camera_data"]["camera_matrix"]["5"].as<double>());
    cameraMatrixVec.emplace_back(configCam_.getConfig()["camera_data"]["camera_matrix"]["6"].as<double>());
    cameraMatrixVec.emplace_back(configCam_.getConfig()["camera_data"]["camera_matrix"]["7"].as<double>());
    cameraMatrixVec.emplace_back(configCam_.getConfig()["camera_data"]["camera_matrix"]["8"].as<double>());

    distCoeffsVec.emplace_back(configCam_.getConfig()["camera_data"]["dist_coeffs"]["0"].as<double>());
    distCoeffsVec.emplace_back(configCam_.getConfig()["camera_data"]["dist_coeffs"]["1"].as<double>());
    distCoeffsVec.emplace_back(configCam_.getConfig()["camera_data"]["dist_coeffs"]["2"].as<double>());
    distCoeffsVec.emplace_back(configCam_.getConfig()["camera_data"]["dist_coeffs"]["3"].as<double>());
    distCoeffsVec.emplace_back(configCam_.getConfig()["camera_data"]["dist_coeffs"]["4"].as<double>());

    cv::Mat cameraMatrix =(cv::Mat_<double>(3,3)<<cameraMatrixVec[0],cameraMatrixVec[1],cameraMatrixVec[2],
            cameraMatrixVec[3],cameraMatrixVec[4],cameraMatrixVec[5],
            cameraMatrixVec[6],cameraMatrixVec[7],cameraMatrixVec[8]);

//    ASSERT(stereoParam.isInit);
//    if (type == 1)
//    {
//        cameraMatrix = stereoParam.cameraMatrixRight_m;
//    }
//    else
//        cameraMatrix = stereoParam.cameraMatrixLeft_m;

    //cv::Mat distCoeffs = (cv::Mat_<double>(1, 5)<<distCoeffsVec[0],distCoeffsVec[1],distCoeffsVec[2],distCoeffsVec[3],distCoeffsVec[4]);

    cv::Mat rvec;
    cv::Mat tvec;

    // pnp确定场地位姿
    // std::cout << camData.size() << std::endl;
//    solvePnPRansac(world, camData, cameraMatrix, distCoeffs, rvec, tvec, false, 100, 2);
    solvePnPRansac(world, camData, cameraMatrix, cv::Mat(), rvec, tvec, false, 100, 2);
    // solvePnP(world, camData, cameraMatrix, distCoeffs, rvec, tvec, false);
    Rodrigues(rvec, rvec);

    // std::cout << rvec << std::endl;
    // std::cout << tvec << std::endl;

    /**
     * 各种变量的解释
     * K R T ： 内参矩阵，旋转矩阵，平移向量
     * Puv ： UV坐标系下的目标点坐标
     * Pc ： 相机在世界坐标系下的坐标
     * Pd ： 目标点在世界坐标系下 Z = 1 时的粗略坐标
     * vec1 : 用于计算叉积的第一个向量
     * vec2 : 用于计算叉积的第二个向量
     * n : 解矩阵方程后的平面方程 X Y Z 的三个系数 / 法向量
     * P0 ： 点法式所需要的P0点
     * D : 平面一般式方程中的D参数
     * result3D result2D ： 目标点在地图的3D 和 2D 坐标
     */

    // 各种矩阵向量初始化
    Eigen::Matrix<float, 3, 3> R;
    Eigen::Matrix<float, 3, 3> K;
    Eigen::Vector3f T;
    Eigen::Vector3f Puv;
    Eigen::Vector3f Pd;
    Eigen::Vector3f vec1;
    Eigen::Vector3f vec2;
    Eigen::Vector3f n;
    Eigen::Vector3f P0;
    Eigen::Vector3f result3D;
    Eigen::Vector2f result2D;
    float D = 0;

    cv::cv2eigen(cameraMatrix, K);
    cv::cv2eigen(rvec, R);
    cv::cv2eigen(tvec, T);

    Eigen::Vector3f Pc = -1 * R.transpose() * T; // 计算相机坐标

    LOG_SUCCESS("PnP Finish!");
    LOG_INFO("The Camara Position Is:\n" << Pc);

    for (int u = 0; u < frameCol; ++u)
    {
        std::vector<Eigen::Vector2f> LUTEachV; // 查找表的按列索引
        for (int v = 0; v < frameRow; ++v)
        {
            Puv << (float)u, (float)v, 1;
            Pd = R.transpose() * (K.inverse() * Puv - T); // 计算像素坐标位置相机坐标的方向并转换为世界坐标

            //std::cout << "(" << u << "," << v << ")" << std::endl;

            std::vector<Eigen::Vector2f> positionInfer;

            // 逐个面遍历
            for (std::vector<cv::Point3f> faceData : mapData)
            {

                P0 << faceData[0].x, faceData[0].y, faceData[0].z;
                vec1 << (faceData[1].x - faceData[0].x), (faceData[1].y - faceData[0].y), (faceData[1].z - faceData[0].z);
                vec2 << (faceData[2].x - faceData[0].x), (faceData[2].y - faceData[0].y), (faceData[2].z - faceData[0].z);
                n = vec1.cross(vec2);
                D = n.dot(P0);

                ASSERT(!n.isZero()); // 法向量不存在，点共线

                result3D = Pc + (Pd - Pc) * ((D - n.dot(Pc)) / (n.dot(Pd - Pc)));
                result2D = result3D.block<2, 1>(0, 0);

                if (isInIt(faceData, result3D)) // 判断是否在面内
                {
                    //std::cout << result3D << std::endl << std::endl;
                    positionInfer.emplace_back(result2D);
                }
            }

            Eigen::Vector2f eachAverage(-1, -1);
            for (Eigen::Vector2f each : positionInfer)
            {
                eachAverage += each;
            }
            if (positionInfer.size() != 0)
                eachAverage *= (1.0f / (float)positionInfer.size());
            //std::cout << eachAverage(0) << "--" << eachAverage(1) << std::endl;
            LUTEachV.emplace_back(eachAverage(0), eachAverage(1));
        }
        _2DLUT.emplace_back(LUTEachV);
    }

    ASSERT(_2DLUT.size() == frameCol && _2DLUT[0].size() == frameRow); // LUT生成错误

    return true;
}

Eigen::Vector2f ne::NESolvePosition::get2DPoints(Eigen::Vector2f Puv)
{
    int PuvX = (int)(Puv[0]);
    int PuvY = (int)(Puv[1]);

    ASSERT(_2DLUT.size());

    if (PuvX >= _2DLUT.size())
    {
        spdlog::warn("The PuvX Out of The LUT!");
        PuvX = _2DLUT.size() - 1;
    }

    if (PuvY >= _2DLUT.at(0).size())
    {
        spdlog::warn("The PuvY Out of The LUT!");
        PuvY = _2DLUT.size() - 1;
    }


    return _2DLUT.at(PuvX).at(PuvY);
}
