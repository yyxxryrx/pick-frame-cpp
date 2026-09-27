//
// Created by yyxxryrx on 2025/11/11.
//

#ifndef PICK_FRAME_TIME_H
#define PICK_FRAME_TIME_H
#include <cstdint>
#include <format>

struct Time {
    uint32_t hours = 0;
    uint32_t minutes = 0;
    uint32_t seconds = 0;
    uint32_t milliseconds = 0;
    auto to_secs() const -> double;
};

template<>
struct std::formatter<Time> {
    static constexpr auto parse(const std::format_parse_context &ctx) {
        return ctx.begin();
    }

    static auto format(const Time &t, std::format_context &ctx) {
        return std::format_to(ctx.out(), "{}:{}:{}.{}", t.hours, t.minutes, t.seconds, t.milliseconds);
    }
};

#endif //PICK_FRAME_TIME_H
