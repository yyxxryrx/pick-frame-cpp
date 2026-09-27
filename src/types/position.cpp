//
// Created by yyxxryrx on 2025/11/11.
//

#include "position.h"
#include "../util.h"
#include "time.h"
#include <expected>
#include <iostream>
#include <vector>

// value from FFmpeg
#define TIME_BASE 1000000

auto Position::to_frame(this const Position &self, const double rate)
    -> std::expected<uint32_t, std::string> {
    if (self.end)
        return UINT32_MAX;
    if (self.frame)
        return *self.frame;
    if (self.time)
        return static_cast<uint32_t>(self.time->to_secs() * rate);
    return std::unexpected("No any value can use");
}

auto Position::to_timestamp(this const Position &self, const double rate) -> std::expected<int64_t, std::string> {
    if (self.end)
        return INT64_MAX;
    if (self.time) return self.time->to_secs() * TIME_BASE;
    if (self.frame) return *self.frame / rate * TIME_BASE;
    return std::unexpected("No any value can use");
}

auto Position::is_start(this const Position &self) -> bool {
    if (self.end)
        return false;
    if (self.time && self.time->to_secs() == 0)
        return true;
    if (self.frame && *self.frame == 0)
        return true;
    return false;
}

auto Position::is_end(this const Position &self) -> bool {
    return self.end;
}

std::istream &operator>>(std::istream &is, Position &t) {
    std::vector<uint32_t> times;
    bool is_milliseconds = false;
    while (true) {
        uint32_t value;
        // 是数字
        if (!(is >> value)) {
            // 连数字都不是，跳出，尝试匹配 end
            break;
        }
        // 是毫秒，不用管下一个了，直接结束
        if (is_milliseconds) {
            times.push_back(value);
            break;
        }
        // 尝试获取间隔符
        char c;
        if (!(is >> c)) {
            // 拿不到字符，说明到底了
            times.push_back(value);
            // 清除失败的状态
            is.clear();
            break;
        }
        // 判断间隔符是否合规
        if (c != ':' && c != '.') {
            is.setstate(std::ios::badbit);
            return is;
        }
        times.push_back(value);
        is_milliseconds = c == '.';
    }
    // 是帧数
    if (times.size() == 1) {
        t.frame = times[0];
        return is;
    }
    if (!times.empty() && times.size() < 5) {
        // 是时间
        Time time;
        // 如果 is_milliseconds 为真，说明输入的时间格式里带有毫秒
        if (is_milliseconds) {
            // 输入如果是 0.3 这样的毫秒要从 3 补位到 300
            time.milliseconds = fillInThePosition(times[times.size() - 1]);
            times.pop_back();
        }
        switch (times.size()) {
            // 只有秒
            case 1:
                time.seconds = times[0];
                break;
            // 只有分钟和秒
            case 2:
                time.minutes = times[0];
                time.seconds = times[1];
                break;
            // 三个都有
            default:
                time.hours = times[0];
                time.minutes = times[1];
                time.seconds = times[2];
                break;
        }
        t.time = time;
        return is;
    }
    if (times.size() >= 5) {
        // 不知道是啥
        is.setstate(std::ios_base::badbit);
        return is;
    }
    is.clear();
    // 如果是 end 那就是 end
    if (std::string word; is >> word && str_eq(word, "end")) {
        t.end = true;
        return is;
    }
    // 啥都不是
    is.setstate(std::ios_base::badbit);
    return is;
}
