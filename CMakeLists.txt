cmake_minimum_required(VERSION 4.0)
project(pick_frame)

set(VCPKG_TARGET_TRIPLET "x64-windows")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 使用 FetchContent 自动下载 cxxopts
include(FetchContent)
FetchContent_Declare(
        cxxopts
        GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git
        GIT_TAG v3.3.1
)
FetchContent_MakeAvailable(cxxopts)

# 查找并链接 FFmpeg 库
find_package(PkgConfig REQUIRED)
pkg_check_modules(AVFORMAT REQUIRED IMPORTED_TARGET libavformat)
pkg_check_modules(AVCODEC REQUIRED IMPORTED_TARGET libavcodec)
pkg_check_modules(AVUTIL REQUIRED IMPORTED_TARGET libavutil)
pkg_check_modules(SWSCALE REQUIRED IMPORTED_TARGET libswscale)

add_executable(pick_frame
        src/main.cpp
        src/util.cpp
        src/util.h
        src/cli.cpp
        src/cli.h
        src/sws.h
        src/types/time.h
        src/types/time.cpp
        src/types/position.h
        src/types/position.cpp
        src/types/ffmpeg.h
        src/video/video_frame_reader.cpp
        src/video/video_frame_reader.h
        src/video/video_info_reader.cpp
        src/video/video_info_reader.h
        src/types/av.h
        src/types/av.h
)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        cxxopts::cxxopts
        PkgConfig::AVFORMAT
        PkgConfig::AVCODEC
        PkgConfig::AVUTIL
        PkgConfig::SWSCALE
)

if(MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE /W4 /permissive-)
else()
        target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
endif()
