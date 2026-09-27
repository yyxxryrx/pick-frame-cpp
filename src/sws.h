//
// Created by yyxxryrx on 2026/9/26.
//
#pragma once

#include <expected>
#include <fstream>
#include "util.h"

#include "types/ffmpeg.h"

extern "C" {
#include  <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
}

namespace sws {
    using FramePtr = FFPtr1<AVFrame, av_frame_free>;
    using PacketPtr = FFPtr1<AVPacket, av_packet_free>;
    using SwsContextPtr = FFPtr2<SwsContext, sws_freeContext>;
    using CodecContextPtr = FFPtr1<AVCodecContext, avcodec_free_context>;

    struct Converter {
        AVPixelFormat format{};

        Converter(SwsContextPtr sws_context, const AVCodec *codec, CodecContextPtr codec_context, AVPixelFormat format);

        auto save(this const Converter &self, const AVFrame &frame,
                  std::string_view path) noexcept -> std::expected<void, std::string>;

    private:
        SwsContextPtr swc_context{};
        AVCodec const *codec{};
        CodecContextPtr codec_ctx{};
    };

    inline Converter::Converter(SwsContextPtr sws_context, const AVCodec *codec, CodecContextPtr codec_context,
                                const AVPixelFormat format) : format(format), swc_context(
                                                                  std::move(sws_context)),
                                                              codec(codec), codec_ctx(std::move(codec_context)) {
    }

    inline auto Converter::save(this const Converter &self, const AVFrame &frame,
                                const std::string_view path) noexcept -> std::expected<void, std::string> {
        const auto width = frame.width;
        const auto height = frame.height;

        const auto rgb_frame = FramePtr(av_frame_alloc());
        if (rgb_frame == nullptr) return std::unexpected("Frame allocate failed");

        rgb_frame->width = width;
        rgb_frame->height = height;
        rgb_frame->format = self.format;
        av_frame_get_buffer(rgb_frame.get(), 0);

        sws_scale(self.swc_context.get(), frame.data, frame.linesize, 0, height, rgb_frame->data, rgb_frame->linesize);

        const auto pkt = PacketPtr(av_packet_alloc());

        if (const auto ret = avcodec_send_frame(self.codec_ctx.get(), rgb_frame.get()); ret >= 0) {
            if (const auto ret2 = avcodec_receive_packet(self.codec_ctx.get(), pkt.get()); ret2 >= 0) {
                std::ofstream out(path.data(), std::ios::binary);
                if (!out) return std::unexpected("Cannot open file");

                out.write(reinterpret_cast<const char *>(pkt->data), pkt->size);

                if (!out) return std::unexpected("Write file failed");

                av_packet_unref(pkt.get());
            }
        }
        return {};
    }

    inline auto new_converter(const int width, const int height, const AVPixelFormat src_pix_format,
                              const AVCodecID encoder,
                              const AVPixelFormat dst_pix_format) noexcept -> std::expected<Converter, std::string> {
        const auto codec = avcodec_find_encoder(encoder);
        if (codec == nullptr) return std::unexpected("Cannot found encoder");

        auto codec_ctx = CodecContextPtr(avcodec_alloc_context3(codec));
        if (codec_ctx == nullptr) return std::unexpected("Cannot allocate the context");

        codec_ctx->width = width;
        codec_ctx->height = height;
        codec_ctx->pix_fmt = dst_pix_format;
        codec_ctx->time_base = {.num = 1, .den = 25};

        if (const auto ret = avcodec_open2(codec_ctx.get(), codec, nullptr); ret < 0)
            return std::unexpected(ffmpeg::av_err_to_string(ret));

        auto sws = SwsContextPtr(sws_getContext(width, height, src_pix_format, width, height, dst_pix_format,
                                                SWS_BILINEAR,
                                                nullptr, nullptr, nullptr));

        if (sws == nullptr) return std::unexpected("Get sws context failed");

        return Converter(std::move(sws), codec, std::move(codec_ctx), dst_pix_format);
    }
}
