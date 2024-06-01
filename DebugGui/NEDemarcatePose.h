//
// Created by ziyu on 24-1-10.
//

#ifndef NEXTERADAR_NEDEMARCATEPOSE_H
#define NEXTERADAR_NEDEMARCATEPOSE_H

#include <vector>
#include <opencv2/opencv.hpp>

#include "NEInit.h"
#include "NEImgStream.h"
#include "NEConfig.h"

namespace ne
{


    class NEDemarcatePose {
    private:
        NEImgStream * _stream = nullptr;
        int _demarcatePointNum = 0;
        double _zoom = 1;
        static void mouse_event(int event, int x, int y, int flags, void* param);
        CamType_t camType_ = CAM_LEFT;

    public:
        std::vector<cv::Point2f> _demarcatePoint;

        std::vector<cv::Point2f> demarcatePointLeft_;
        std::vector<cv::Point2f> demarcatePointRight_;

        NEDemarcatePose() = default;
        NEDemarcatePose(NEConfig & config);
        ~NEDemarcatePose();

        /**
         * 开始标定赛场，使用n选择下一帧图片，ENTER 确定选择图片，鼠标选点开始标定
         * @param stream
         * @result 成功或失败
         */
        bool startDemarcate(NEImgStream & stream, CamType_t camType);

        /**
         * 标定时缩放图片
         * @param stream
         * @param zoom
         * @return
         */
        //bool startDemarcate(NEImgStream & stream, double zoom);

        /**
         * 显示是否已经完成标定
         * @return
         */
        bool isDemarcate() const;
    };
}


#endif //NEXTERADAR_NEDEMARCATEPOSE_H
