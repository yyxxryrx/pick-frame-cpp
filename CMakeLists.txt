cmake_minimum_required(VERSION 4.0)
project(pick_frame)

set(VCPKG_TARGET_TRIPLET "x64-windows")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)
#set(CMAKE_CXX_MODULE_STD ON)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

# 使用 FetchContent 自动下载 cxxopts
include(FetchContent)
FetchContent_Declare(
        cxxopts
        GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git
        GIT_TAG v3.3.1  # 使用最新稳定版本
)
FetchContent_MakeAvailable(cxxopts)

# 查找并链接 FFmpeg 库
find_package(PkgConfig REQUIRED)
pkg_check_modules(AVFORMAT REQUIRED IMPORTED_TARGET libavformat)
pkg_check_modules(AVCODEC REQUIRED IMPORTED_TARGET libavcodec)
pkg_check_modules(AVUTIL REQUIRED IMPORTED_TARGET libavutil)
pkg_check_modules(AVDEVICE REQUIRED IMPORTED_TARGET libavdevice)
pkg_check_modules(AVFILTER REQUIRED IMPORTED_TARGET libavfilter)
pkg_check_modules(SWSCALE REQUIRED IMPORTED_TARGET libswscale)
pkg_check_modules(SWRESAMPLE REQUIRED IMPORTED_TARGET libswresample)
find_package(Boost CONFIG REQUIRED)

add_executable(pick_frame src/main.cpp
        src/util.cpp
        src/util.h
        src/types/time.h
        src/types/position.h
        src/types/position.cpp
        src/cli.cpp
        src/cli.h
        src/types/time.cpp
        src/video/video_frame_reader.cpp
        src/video/video_info_reader.cpp
        src/types/ffmpeg.h
        src/sws.h)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        cxxopts::cxxopts
        Boost::headers
        PkgConfig::AVFORMAT
        PkgConfig::AVCODEC
        PkgConfig::AVUTIL
        PkgConfig::AVDEVICE
        PkgConfig::AVFILTER
        PkgConfig::SWSCALE
        PkgConfig::SWRESAMPLE
        #        ${AVFORMAT_LIBRARIES}
        #        ${AVCODEC_LIBRARIES}
        #        ${AVUTIL_LIBRARIES}
        #        ${AVDEVICE_LIBRARIES}
        #        ${AVFILTER_LIBRARIES}
        #        ${SWSCALE_LIBRARIES}
        #        ${SWRESAMPLE_LIBRARIES}
)

#target_include_directories(${PROJECT_NAME}
#        PRIVATE
#        ${AVFORMAT_INCLUDE_DIRS}
#        ${AVCODEC_INCLUDE_DIRS}
#        ${AVUTIL_INCLUDE_DIRS}
#        ${AVDEVICE_INCLUDE_DIRS}
#        ${AVFILTER_INCLUDE_DIRS}
#        ${SWSCALE_INCLUDE_DIRS}
#        ${SWRESAMPLE_INCLUDE_DIRS}
#)
#
#target_compile_options(${PROJECT_NAME}
#        PRIVATE
#        ${AVFORMAT_CFLAGS_OTHER}
#        ${AVCODEC_CFLAGS_OTHER}
#        ${AVUTIL_CFLAGS_OTHER}
#        ${AVDEVICE_CFLAGS_OTHER}
#        ${AVFILTER_CFLAGS_OTHER}
#        ${SWSCALE_CFLAGS_OTHER}
#        ${SWRESAMPLE_CFLAGS_OTHER}
#)