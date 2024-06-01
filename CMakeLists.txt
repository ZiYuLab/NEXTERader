cmake_minimum_required(VERSION 3.11 )
project(NEXTERadar)

# 检测政策更新
if(POLICY CMP0146)
    cmake_policy(SET CMP0146 OLD)
endif()

find_package(OpenCV REQUIRED)
find_package(CUDA REQUIRED)
#find_package(TensorRT REQUIRED)
#find_package(fmt REQUIRED)

option(CUDA_USE_STATIC_CUDA_RUNTIME OFF)

# set(CMAKE_BUILD_TYPE Debug)
set(set(CMAKE_CXX_STANDARD 17)
        set(CMAKE_CXX_FLAGS "-std=c++11"))
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -O0  -Wall -fopenmp")

set(CUDA_GEN_CODE "-gencode=arch=compute_86,code=sm_86")
set(CUDA_NVCC_FLAGS "${CUDA_NVCC_FLAGS}  -std=c++17 -O0 -Xcompiler -fPIC -g -w ${CUDA_GEN_CODE}")
# set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

#CSerialPort
set(CSerialPortRootPath "/usr/local/")
include_directories(${CSerialPortRootPath}/include)
link_directories(${CSerialPortRootPath}/lib)

# fmt---------------------------------------------
add_subdirectory(3rd/fmt EXCLUDE_FROM_ALL)
# ------------------------------------------------

# spdlog------------------------------------------
include_directories(3rd/spdlog/include/)
# ------------------------------------------------

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -Ofast -Wfatal-errors -D_MWAITXINTRIN_H_INCLUDED")

include_directories(
        Net/TRTInfer/include/
        Net/
        ImgStream/
        Serial/include/
        Robot/
        StereoPosition/
        ReadConfig/
        DebugGui/
        SolvePosition/
        ImgStream/hik_camera
        Tracker/
        IOManager/
        StereoSolvePosition/
        Framework/
        NERadar/
        ./
        ${EIGEN3_INCLUDE_DIR}
        ${OpenCV_INCLUDE_DIRS}
        ${TensorRT_INCLUDE_DIRS}
)


cuda_add_executable(
        ${PROJECT_NAME}
        main.cpp
        Net/TRTInfer/src/Inference.cpp
        Net/TRTInfer/CUDA/preprocess.cu
        ImgStream/NEImgStream.cpp
        ReadConfig/NEConfig.cpp
        Net/NENet.cpp
        SolvePosition/NESolvePosition.cpp
        DebugGui/NEDemarcatePose.cpp
        ImgStream/hik_camera/HikCam.cpp
        Tracker/Tracker.cpp
        IOManager/RmCRC.cpp
        IOManager/IOManager.cpp
        StereoSolvePosition/StereoSolvePosition.cpp
        Framework/MulThrCV.cpp
        NERadar/Radar.cpp
        #Net/NENet.h
        #Camera/HikCamera/HikCamera.cpp
)


#target_include_directories(${PROJECT_NAME} PUBLIC ./ ./net_onnx/include)

target_include_directories(${PROJECT_NAME} PUBLIC ${OpenCV_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME}  ${OpenCV_LIBS})

## GalaxyCamera
#add_subdirectory(GalaxyCameraReader)
#target_include_directories(${PROJECT_NAME} PUBLIC  GalaxyCameraReader)
#target_link_libraries(${PROJECT_NAME} PUBLIC "GalaxyCameraReader")

# HikCamera

target_include_directories(${PROJECT_NAME} PUBLIC ${MVCAM_SDK_PATH}/include )
target_include_directories(${PROJECT_NAME} PUBLIC /opt/MVS/include/)
target_link_libraries(${PROJECT_NAME}  /opt/MVS/lib/64/libMvCameraControl.so )
target_link_libraries(${PROJECT_NAME}  ${LD_LIBRARY_PATH} )
#
set_target_properties(${PROJECT_NAME} PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
target_link_libraries(${PROJECT_NAME}  nvinfer)
target_link_libraries(${PROJECT_NAME}  /usr/local/cuda-12.1/targets/x86_64-linux/lib/libcudart.so)
target_link_libraries(${PROJECT_NAME}  nvonnxparser)
target_link_libraries(${PROJECT_NAME}  cuda)
target_link_libraries(${PROJECT_NAME}  ${TensorRT_LIBS})
target_link_libraries( ${PROJECT_NAME} cserialport)

#eigen3
include_directories(/usr/include/eigen3)

#fmt
#target_link_libraries(${PROJECT_NAME}  fmt)
include_directories(${CMAKE_SOURCE_DIR}/3rd/fmt/include)
target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/3rd/fmt/libfmt.a)

#yaml-cpp
#find_package(yaml-cpp REQUIRED)
#include_directories(/usr/local/include/yaml-cpp)
#target_link_libraries(${PROJECT_NAME} /usr/local/lib/libyaml-cpp.so)
#find_package(yaml-cpp REQUIRED)
include_directories(${CMAKE_SOURCE_DIR}/3rd/yaml-cpp/include)
target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/3rd/yaml-cpp/libyaml-cpp.a)


# 在Linux系统下，多线程模块并非自动链接的，需要额外链接。
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    find_package(Threads)
    target_link_libraries(${PROJECT_NAME}  ${CMAKE_THREAD_LIBS_INIT})
endif()

#其他
target_link_libraries(${PROJECT_NAME}  dl pthread m)

