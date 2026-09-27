#include "video_info_reader.h"
#include "../util.h"
#include <expected>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
}

VideoInfoReader::VideoInfoReader() { avformat_network_init(); }

VideoInfoReader::~VideoInfoReader() {
    if (context)
        avformat_close_input(&context);
}

auto VideoInfoReader::open(const std::string &path)
    noexcept -> std::expected<VideoInfo, std::string> {
    if (const auto code =
                avformat_open_input(&context, path.c_str(), nullptr, nullptr);
        code != 0)
        return std::unexpected(ffmpeg::av_err_to_string(code));

    if (const auto code = avformat_find_stream_info(context, nullptr); code < 0)
        return std::unexpected(ffmpeg::av_err_to_string(code));

    const auto index =
            av_find_best_stream(context, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);

    if (index < 0)
        return std::unexpected("Cannot find the best video stream");

    const auto stream = context->streams[index];
    const auto codec_params = stream->codecpar;
    const auto codec = avcodec_find_decoder(codec_params->codec_id);
    if (!codec)
        return std::unexpected("Cannot find the video codec");
    const auto codec_context = avcodec_alloc_context3(codec);
    if (!codec_context)
        return std::unexpected("Cannot allocate the video codec context");
    if (const auto code =
                avcodec_parameters_to_context(codec_context, codec_params);
        code < 0)
        return std::unexpected(ffmpeg::av_err_to_string(code));

    return VideoInfo{
        .frame_count = static_cast<uint64_t>(stream->nb_frames),
        .duration = static_cast<uint64_t>(stream->duration),
        .width = static_cast<uint32_t>(codec_params->width),
        .height = static_cast<uint32_t>(codec_params->height),
        .frame_index = index,
        .fps = static_cast<double>(stream->avg_frame_rate.num) /
               stream->avg_frame_rate.den,
    };
}

auto VideoInfo::open(const std::string &path)
    -> std::expected<VideoInfo, std::string> {
    VideoInfoReader reader;
    return reader.open(path);
}
