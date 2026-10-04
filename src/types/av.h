//
// Created by yuxir on 2026/10/3.
//

#ifndef PICK_FRAME_AV_H
#define PICK_FRAME_AV_H
#include "ffmpeg.h"

extern "C" {
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
}

using FramePtr = FFPtr1<AVFrame, av_frame_free>;
using FormatContextPtr = FFPtr2<AVFormatContext, avformat_free_context>;
using CodecContextPtr = FFPtr1<AVCodecContext, avcodec_free_context>;
using PacketPtr = FFPtr1<AVPacket, av_packet_free>;

#endif //PICK_FRAME_AV_H
