//
// Created by ziyu on 11/16/23.
//

#ifndef NESERIAL_NEINIT_H
#define NESERIAL_NEINIT_H


#include <bits/types.h>
#include <mutex>
#include <iostream>
#include <unistd.h>
#include <string>
#include <spdlog/spdlog.h>

#define DEBUG

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//#define DEBUG // <---------------------------
#ifdef DEBUG
#include <assert.h>
#define ASSERT(f) assert(f)
#else
#define ASSERT(f) ((void)0)
#endif
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

// 多线程互斥设置
// std::mutex mutSerialWriteBuffers;

namespace ne {

#ifdef DEBUG
#define LOG_ERROR(X) { \
    std::cout  << "\033[31m" << "NE_ERROR: " << X << "\033[0m" << std::endl; \
    abort();                                                               \
}
#else
#define LOG_ERROR(X) { \
    std::cout  << "\033[31m" << "NE_ERROR: " << X << "\033[0m" << std::endl; \
}
#endif

#define LOG_INFO(X) { \
    std::cout  << "\033[34m" << "NE_INFO: " << X << "\033[0m" << std::endl;                     \
}                     \

#define LOG_SUCCESS(X) { \
    std::cout  << "\033[32m" << "NE_SUCCESS: " << X << "\033[0m" << std::endl;                     \
}

#define SHOW_NE { \
    std::cout <<  "\033[35m";                                              \
    std::cout << "   _   _ ________   _________      ______     \n"        \
                 "  | \\ | |  ____\\ \\ / /__   __|    |  ____| \n"        \
                 "  |  \\| | |__   \\ V /   | |       | |__     \n"        \
                 "  | . ` |  __|   > <    | |       |  __|      \n"        \
                 "  | |\\  | |____ / . \\   | |       | |____   \n"        \
                 "  |_| \\_|______/_/ \\_\\  |_|       |______| \n"  ;     \
    std::cout <<  "\033[0m" << std::endl;                                  \
}

#ifdef DEBUG
#define CHECK_PATH(X) { \
    if (access(X, F_OK)) \
    {                   \
        std::cout  << "\033[31m" << "NE_ERROR: \"" << X << "\" Open Fail! \033[0m" << std::endl; \
        abort();                     \
    }                   \
}
#else
#define CHECK_PATH(X) { \
    if (access(X, F_OK)) \
    {                   \
        std::cout  << "\033[31m" << "NE_ERROR: \"" << X << "\" Open Fail! \033[0m" << std::endl; \
        abort();                     \
    }                   \
}
#endif
// 比赛基本设置


// Type re def
    typedef __uint8_t NE_8U;
    typedef __uint16_t NE_16U;
    typedef __uint32_t NE_32U;
    typedef float NE_32F;
    typedef double NE_64F;

// Change types to 1 byte
    union NE32F2NE8U {
        NE_32F ne32FIN;
        NE_8U ne8uOUT[4];
    };

    union NE16U2NE8U {
        NE_16U ne16UIN;
        NE_8U ne8uOUT[2];
    };

    enum CamType_t
    {
        CAM_LEFT = 1,
        CAM_RIGHT = 2,
    };

    enum Our_t
    {
        OUR_RED = 1,
        OUR_BLUE = 2,
        OUR_NULL = 0
    };

}

#endif //NESERIAL_NEINIT_H
