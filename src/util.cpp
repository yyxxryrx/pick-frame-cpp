//
// Created by yyxxryrx on 2025/11/11.
//

#include <algorithm>
#include <cmath>
#include <string>
#include <cstdint>
#include <fstream>

auto fillInThePosition(const std::uint32_t value) -> uint32_t {
    // 计算数值的位数，如果小于3，就往后补 0
    if (const auto bit = static_cast<int>(std::log10(value)) + 1; bit < 3)
        return static_cast<uint32_t>(value * std::pow(10, 3 - bit));
    // 位数不小于3，无需处理，直接返回
    return value;
}

auto checkFileExists(const std::string &path) -> bool {
    const std::ifstream f(path);
    return f.good();
}

auto str_eq(const std::string &a, const std::string &b) -> bool {
    if (a.length() != b.length())
        return false;
    return std::ranges::equal(a, b, [](const auto c1, const auto c2) constexpr {
        return std::tolower(c1) == std::tolower(c2);
    });
}

namespace ffmpeg {
    extern "C" {
#include <libavutil/error.h>
    }

    // 封装一个 C++ 友好的错误转换函数
    std::string av_err_to_string(int errnum) {
        char err_buf[AV_ERROR_MAX_STRING_SIZE];
        av_make_error_string(err_buf, AV_ERROR_MAX_STRING_SIZE, errnum);
        return {err_buf};
    }
}
