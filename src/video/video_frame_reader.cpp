//
// Created by yyxxryrx on 2025/11/25.
//

#include "video_frame_reader.h"
#include "../util.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

VideoFrame::VideoFrame(FramePtr frame) : index(0), frame(std::move(frame)) {
}

VideoFrame::VideoFrame(FramePtr frame, const uint32_t index) : index(index), frame(std::move(frame)) {
}


VideoFrameReader::VideoFrameReader(const VideoInfo &info) {
    this->info = info;
    frame.reset(av_frame_alloc());
    avformat_network_init();
}

auto VideoFrameReader::goto_position(
    this VideoFrameReader &self,
    Position position
) -> std::expected<void, std::string> {
    if (!self.fmtCtx || !self.codecCtx)
        return std::unexpected("VideoFrameReader not opened");
    if (!self.info)
        return std::unexpected("VideoInfo not set");
    const auto index = self.info->frame_index;
    const auto timestamp = position.to_timestamp(self.info->fps);
    if (!timestamp)
        return std::unexpected(timestamp.error());
    if (const auto ret = av_seek_frame(self.fmtCtx.get(), index, *timestamp, AVSEEK_FLAG_BACKWARD);
        ret < 0) {
        return std::unexpected(std::format("av_seek_frame failed: {}", ffmpeg::av_err_to_string(ret)));
    }
    avcodec_flush_buffers(self.codecCtx.get());
    auto pkt = av_packet_alloc();
    const auto frame = av_frame_alloc();
    while (av_read_frame(self.fmtCtx.get(), pkt) >= 0) {
        if (pkt->stream_index == index) {
            if (avcodec_send_packet(self.codecCtx.get(), pkt) < 0) {
                av_packet_unref(pkt);
                continue;
            }
            while (avcodec_receive_frame(self.codecCtx.get(), frame) == 0)
                if (frame->pts >= *timestamp)
                    goto end_seek;
        }
        av_packet_unref(pkt);
    }
end_seek:
    av_packet_free(&pkt);
    self.frame.reset(frame);
    return {};
}

auto VideoFrameReader::read_frame(this VideoFrameReader &self) -> std::expected<FramePtr, std::string> {
    const auto index = self.info->frame_index;
    const auto pkt = av_packet_alloc();

    while (av_read_frame(self.fmtCtx.get(), pkt) >= 0) {
        if (pkt->stream_index == index) {
            if (avcodec_send_packet(self.codecCtx.get(), pkt) < 0) continue;
            while (avcodec_receive_frame(self.codecCtx.get(), self.frame.get()) == 0) {
                auto ret = std::move(self.frame);
                self.frame.reset(av_frame_alloc());
                av_packet_unref(pkt);
                return ret;
            }
        }
        av_packet_unref(pkt);
    }
    return std::unexpected("EOF");
}


auto VideoFrameReader::open(this VideoFrameReader &self, const std::string &path) -> std::expected<void, std::string> {
    if (!self.info)
        return std::unexpected("VideoInfo not set");
    AVFormatContext *fmtCtx = nullptr;
    if (const auto ret = avformat_open_input(&fmtCtx, path.c_str(), nullptr, nullptr); ret < 0) {
        return std::unexpected(std::format("avformat_open_input failed: {}", ffmpeg::av_err_to_string(ret)));
    }
    self.fmtCtx.reset(fmtCtx);
    if (const auto ret = avformat_find_stream_info(self.fmtCtx.get(), nullptr); ret < 0)
        return std::unexpected(ffmpeg::av_err_to_string(ret));
    const auto index = self.info->frame_index;
    const auto codecPar = self.fmtCtx->streams[index]->codecpar;
    const auto codec = avcodec_find_decoder(codecPar->codec_id);
    if (!codec)
        return std::unexpected("avcodec_find_decoder failed");
    self.codecCtx.reset(avcodec_alloc_context3(codec));
    avcodec_parameters_to_context(self.codecCtx.get(), codecPar);
    if (const auto ret = avcodec_open2(self.codecCtx.get(), codec, nullptr); ret < 0) {
        return std::unexpected(std::format("avcodec_open2 failed: {}", ffmpeg::av_err_to_string(ret)));
    }
    return {};
}

VideoReaderIterator::VideoReaderIterator(VideoFrameReader &reader, const Position &from) : reader(
    &reader) {
    if (!this->reader->goto_position(from)) {
        this->eof = true;
        return;
    }

    if (!this->reader->info) {
        this->eof = true;
        return;
    }

    const auto frame_index = from.to_frame(this->reader->info->fps);
    if (!frame_index) {
        this->eof = true;
        return;
    }
    this->frames = *frame_index;
    this->frame = std::move(this->reader->read_frame());
}

VideoReaderIterator::value_type VideoReaderIterator::operator*() {
    return std::move(this->frame).transform([this](FramePtr f) {
        return VideoFrame{std::move(f), this->frames};
    });
}

auto VideoReaderIterator::operator++() -> VideoReaderIterator & {
    this->frames++;
    this->frame = std::move(this->reader->read_frame());
    return *this;
}

auto VideoReaderIterator::operator++(int) -> VideoReaderIterator {
    auto tmp = std::move(*this);
    this->frames++;
    this->frame = std::move(this->reader->read_frame());
    return tmp;
}

constexpr auto VideoReaderSentinel::get_limit(this const VideoReaderSentinel &self) -> uint32_t {
    return self.frame;
}

auto VideoReaderRange::begin(this const VideoReaderRange &self) -> VideoReaderIterator {
    return {*self.reader, self.from};
}

auto VideoReaderRange::end() const -> VideoReaderSentinel {
    return VideoReaderSentinel(this->to.to_frame(this->reader->info->fps).value_or(0));
}

VideoReader::VideoReader(const Position &from, const Position &to) : from(from), to(to) {
}

auto VideoReader::with_info(const Position &from, const Position &to, const VideoInfo &info) -> VideoReader {
    auto reader = VideoReader(from, to);
    reader.info = info;
    return reader;
}

auto VideoReader::open(
    this VideoReader &self,
    const std::string &path
) -> std::expected<VideoReaderRange, std::string> {
    if (self.info) {
        self.reader = VideoFrameReader(*self.info);
    } else {
        const auto info = VideoInfo::open(path);
        if (!info) {
            self.reader.reset();
            return std::unexpected(info.error());
        }
        self.reader = VideoFrameReader(*info);
    }
    if (const auto ret = self.reader->open(path); !ret)
        return std::unexpected(ret.error());
    auto range = VideoReaderRange(self.from, self.to, &*self.reader);
    // self.reader.reset();
    return range;
}

bool operator==(const VideoReaderIterator &it, const VideoReaderSentinel &sent) {
    return it.eof || it.frames >= sent.get_limit();
}
