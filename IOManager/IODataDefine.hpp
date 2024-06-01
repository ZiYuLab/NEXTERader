//
// Created by ziyu on 24-5-7.
//

#ifndef IOMANAGER_IODATADEFINE_HPP
#define IOMANAGER_IODATADEFINE_HPP

#include <cstdint>
#include <vector>

namespace ne
{
    struct __attribute__ ((__packed__)) map_robot_data_t
    {
        uint16_t target_robot_id;
        float target_position_x;
        float target_position_y;
    };

    struct __attribute__ ((__packed__)) radar_mark_data_t
    {
        uint8_t mark_hero_progress;
        uint8_t mark_engineer_progress;
        uint8_t mark_standard_3_progress;
        uint8_t mark_standard_4_progress;
        uint8_t mark_standard_5_progress;
        uint8_t mark_sentry_progress;
    };

    struct __attribute__ ((__packed__)) robot_interaction_radar_data_t
    {
        uint16_t data_cmd_id;
        uint16_t sender_id;
        uint16_t receiver_id;
        uint8_t radar_cmd;
    };

    struct __attribute__ ((__packed__)) radar_info_t
    {
        uint8_t radar_info;
    };

    union MapRobotData2Uint8T_u
    {
        map_robot_data_t in;
        uint8_t out[sizeof(map_robot_data_t)]{};
    };

    union robotInteractionRadarData2Uint8_u
    {
        robot_interaction_radar_data_t in;
        uint8_t out[sizeof(robot_interaction_radar_data_t)]{};
    };
}

#endif //IOMANAGER_IODATADEFINE_HPP
