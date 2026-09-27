#include <iostream>
#include <print>

#include "cli.h"
#include "sws.h"
#include "util.h"
#include "video/video_frame_reader.h"

int main(const int argc, char *argv[]) {
    const auto cli = Cli::parse(argc, argv);
    if (!checkFileExists(cli.input)) {
        std::println(std::cerr, "{} does not exist", cli.input);
        return EXIT_FAILURE;
    }
    std::println("input: {}", cli.input);
    std::println("output: {}", cli.output);
    std::println("from: {}", cli.from);
    std::println("to: {}", cli.to);
    std::println("--------------------");
    const auto video_info = VideoInfo::open(cli.input);
    if (!video_info) {
        std::println(std::cerr, "get info error: {}", video_info.error());
        return EXIT_FAILURE;
    }
    const auto from_frame = cli.from.to_frame(video_info->fps);
    if (!from_frame) {
        std::println(std::cerr, "{}", from_frame.error());
        return EXIT_FAILURE;
    }
    const auto to_frame = cli.to.to_frame(video_info->fps);
    if (!to_frame) {
        std::println(std::cerr, "{}", to_frame.error());
        return EXIT_FAILURE;
    }
    if (*from_frame > *to_frame) {
        std::println(std::cerr, "Start frame cannot be greater than end frame");
        return EXIT_FAILURE;
    }
    auto reader = VideoReader::with_info(cli.from, cli.to, *video_info);
    auto range = reader.open(cli.input);
    if (!range) {
        std::println(std::cerr, "Init reader error: {}", range.error());
        return EXIT_FAILURE;
    }
    std::optional<sws::Converter> converter = std::nullopt;
    for (const auto &frame: *range) {
        if (!frame) {
            if (frame.error() == "EOF") break;
            std::println(std::cerr, "{}", frame.error());
            continue;
        }
        std::println("Index: {}", frame->index);
        if (!converter) {
            if (auto ret = sws::new_converter(video_info->width, video_info->height,
                                              static_cast<AVPixelFormat>(frame->frame->format), AV_CODEC_ID_MJPEG,
                                              AV_PIX_FMT_YUVJ420P); ret)
                converter = std::move(ret.value());
            else {
                std::println(std::cerr, "Error: {}", ret.error());
                return EXIT_FAILURE;
            }
        }
        auto _ = converter->save(*frame->frame, std::format("{}.png", frame->index));
    }
    return 0;
}
