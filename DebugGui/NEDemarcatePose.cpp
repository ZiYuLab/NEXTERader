//
// Created by ziyu on 24-1-10.
//

#include "NEDemarcatePose.h"
#include <string>

ne::NEDemarcatePose::NEDemarcatePose(ne::NEConfig & config)
{
    _demarcatePointNum = config.getConfig()["gui"]["demarcate_point_num"].as<int>();
}


// 回调函数
void ne::NEDemarcatePose::mouse_event(int event, int x, int y, int flags, void * param)
{
    if(event == cv::MouseEventTypes::EVENT_LBUTTONUP)
    {
        const int RADIUS = 2;
        const int THICKNESS = 1;
        const auto COLOR = cv::Scalar(0, 255, 0);

        auto * demarcatePose = static_cast<NEDemarcatePose *>(param);

        if (demarcatePose->_demarcatePoint.size() < demarcatePose->_demarcatePointNum) // 判断是否超选，超选提示
        {
            //int rx = x * demarcatePose->_zoom;
            //int ry = y * demarcatePose->_zoom;
            demarcatePose->_demarcatePoint.emplace_back(x, y);

            cv::circle(demarcatePose->_stream->_frame, cv::Point(x, y), RADIUS, COLOR, THICKNESS);

            std::string str = std::to_string(demarcatePose->_demarcatePoint.size())  + "/";
            str += std::to_string(demarcatePose->_demarcatePointNum);
            str += " (";
            str += std::to_string(x);
            str += ",";
            str += std::to_string(y);
            str += ")";

            cv::putText(demarcatePose->_stream->_frame, str, cv::Point(x, y), cv::FONT_HERSHEY_PLAIN, 1, COLOR);

            LOG_INFO("Point: (" << x << "," << y << ")");
        }
        else
            LOG_INFO("Point MAX!!");
    }
}

ne::NEDemarcatePose::~NEDemarcatePose()
{

}

bool ne::NEDemarcatePose::startDemarcate(ne::NEImgStream &stream)
{
    _stream = &stream;
    while (true)
    {
        stream.getFrame();

        char key = 0;

        // 选择标定帧
        LOG_INFO("#TODO# : Use 'n' & 'ENTER' to select the frame\n");
        while (true)
        {
            key = cv::waitKey(1);
            stream.show("Select The Frame", NE_STREAM_SOURCE, _zoom);
            if (key == 'n') // n
            {
                stream.getFrame();
            }
            else if (key == 13) // 13 ENTER
            {
                LOG_SUCCESS("select OK!");
                break;
            }
            else if (key == 27) // ESC
            {
                LOG_INFO("exit!");
                abort();
                return false;
            }
        }
        cv::destroyAllWindows();

        const std::string WIN_NAME = "Demarcate";
        stream.nameWindow(WIN_NAME);

        cv::setMouseCallback(WIN_NAME, mouse_event, this);

        LOG_INFO("Start to demarcate");

        // 标定结束选项
        while (true)
        {
            stream.show(WIN_NAME, NE_STREAM_SOURCE, _zoom);

            key = cv::waitKey(1);
            if (key == 13) // ENTER
            {
                if (_demarcatePointNum > _demarcatePoint.size())
                {
                    LOG_INFO("#TODO# : still need" << _demarcatePointNum - _demarcatePoint.size() << "points!\n");
                }
                else if(_demarcatePointNum < _demarcatePoint.size())
                {
                    LOG_ERROR("Too many point");
                }
                else
                {
                    cv::destroyAllWindows();
                    return true;
                }
            }
            else if (key == 27) // ESC
            {
                LOG_INFO("exit!");
                cv::destroyAllWindows();
                abort();
                return false;
            }
            else if (key == 48) // DEL
            {
                cv::destroyAllWindows();
                _demarcatePoint.resize(0); // 清空标定点存储
                LOG_INFO("Delete all point and demarcate again!")
                break;
            }
        }
    }
}

//bool ne::NEDemarcatePose::startDemarcate(ne::NEImgStream &stream, double zoom)
//{
//    _zoom = zoom;
//    return startDemarcate(stream);;
//}

bool ne::NEDemarcatePose::isDemarcate() const
{
    if (_demarcatePointNum != _demarcatePoint.size())
    {
        return false;
    }
    else
        return true;
}