//
// Created by yyxxryrx on 2025/11/25.
//

#ifndef PICK_FRAME_VIDEO_FRAME_READER_H
#define PICK_FRAME_VIDEO_FRAME_READER_H
#include "video_info_reader.h"
#include "../types/position.h"
#include <expected>
#include "../types/av.h"

extern "C" {
#include <libavcodec/avcodec.h>
}

struct VideoFrame {
    uint32_t index;

    FramePtr frame = nullptr;

    explicit VideoFrame(FramePtr frame);

    explicit VideoFrame(FramePtr frame, uint32_t index);
};

struct VideoFrameReader {
    std::optional<VideoInfo> info = std::nullopt;

    explicit VideoFrameReader(const VideoInfo &info);

    ~VideoFrameReader() = default;

    VideoFrameReader(const VideoFrameReader &) = delete;

    VideoFrameReader &operator=(const VideoFrameReader &) = delete;

    VideoFrameReader(VideoFrameReader &&) = default;

    VideoFrameReader &operator=(VideoFrameReader &&) = default;

    auto goto_position(this VideoFrameReader &self, Position position) -> std::expected<void, std::string>;

    auto read_frame(this VideoFrameReader &self) -> std::expected<FramePtr, std::string>;

    auto open(this VideoFrameReader &self, const std::string &path) -> std::expected<void, std::string>;

private:
    FormatContextPtr fmtCtx = nullptr;
    CodecContextPtr codecCtx = nullptr;
    FramePtr frame = nullptr;
};

struct VideoReaderSentinel {
    explicit VideoReaderSentinel(const uint32_t frame) : frame(frame) {
    }

    constexpr auto get_limit(this const VideoReaderSentinel &self) -> uint32_t;

private:
    uint32_t frame;
};

struct VideoReaderIterator {
    using value_type = std::expected<VideoFrame, std::string>;
    using difference_type = int;
    using iterator_concept = std::input_iterator_tag;

    VideoReaderIterator(VideoFrameReader& reader, const Position &from);

    auto operator*() -> value_type;

    auto operator++() -> VideoReaderIterator &;

    auto operator++(int) -> VideoReaderIterator;

    friend bool operator==(const VideoReaderIterator &it, const VideoReaderSentinel &sent);

private:
    std::expected<FramePtr, std::string> frame;
    uint32_t frames = 0;
    bool eof = false;
    VideoFrameReader* reader;
};

struct VideoReaderRange {
    Position from;
    Position to;
    VideoFrameReader *reader;

    VideoReaderRange(const Position &from, const Position &to, VideoFrameReader *reader)
        : from(from), to(to), reader(reader) {
    }

    auto begin(this const VideoReaderRange &self) -> VideoReaderIterator;

    [[nodiscard]] auto end() const -> VideoReaderSentinel;
};

struct VideoReader {
    VideoReader(const Position &from, const Position &to);

    static auto with_info(const Position &from, const Position &to, const VideoInfo &info) -> VideoReader;

    auto open(this VideoReader &self, const std::string &path) -> std::expected<VideoReaderRange, std::string>;

private:
    Position from;
    Position to;
    std::optional<VideoFrameReader> reader = std::nullopt;
    std::optional<VideoInfo> info = std::nullopt;
};

#endif //PICK_FRAME_VIDEO_FRAME_READER_H
