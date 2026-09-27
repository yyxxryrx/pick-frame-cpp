//
// Created by yyxxryrx on 2025/11/11.
//

#ifndef PICK_FRAME_UTIL_H
#define PICK_FRAME_UTIL_H
#include <cstdint>
auto fillInThePosition(std::uint32_t value) -> std::uint32_t;
auto checkFileExists(const std::string& path) -> bool;
auto str_eq(const std::string &a, const std::string &b) -> bool;

namespace ffmpeg {
    std::string av_err_to_string(int errnum);
}
#endif //PICK_FRAME_UTIL_H