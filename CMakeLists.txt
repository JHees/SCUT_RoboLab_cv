CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
set(CMAKE_BUILD_TYPE Release)
project(SCUT_RoboLab_cv)

add_compile_options(-std=c++11)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
link_directories(${OpenCV_LIBRARY_DIRS})
add_definitions(${OpenCV_DEFINITIONS})


add_executable(SCUT_RoboLab_cv SCUT_RoboLab_cv.cpp)
target_link_libraries(SCUT_RoboLab_cv ${OpenCV_LIBRARIES})