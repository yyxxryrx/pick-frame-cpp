//
// Crated by yyxxryrx on 2025/11/19
//

#ifndef PICK_FRAME_VIDEO_INFO_READER_H
#define PICK_FRAME_VIDEO_INFO_READER_H
#include <expected>
#include <string>

extern "C" {
#include <libavformat/avformat.h>
}

struct VideoInfo {
    uint64_t frame_count, duration;
    uint32_t width, height;
    int frame_index;
    double fps;

    static auto open(const std::string &path)
        -> std::expected<VideoInfo, std::string>;
};

struct VideoInfoReader {
    VideoInfoReader();
    ~VideoInfoReader();

    auto open(const std::string &path) noexcept -> std::expected<VideoInfo, std::string>;

  private:
    AVFormatContext *context = nullptr;
};
#endif // PICK_FRAME_VIDEO_INFO_READER_H
