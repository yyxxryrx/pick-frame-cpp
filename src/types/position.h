//
// Created by yyxxryrx on 2025/11/11.
//

#ifndef PICK_FRAME_POSITION_H
#define PICK_FRAME_POSITION_H
#include "time.h"
#include <cstdint>
#include <expected>
#include <format>
#include <optional>
#include <string>


struct Position {
    std::optional<uint32_t> frame;
    std::optional<Time> time;
    bool end = false;
    auto to_frame(this const Position &self, double rate)
        -> std::expected<uint32_t, std::string>;
    auto to_timestamp(this const Position &self, double rate) -> std::expected<int64_t, std::string>;
    auto is_start(this const Position &self) -> bool;
    auto is_end(this const Position &self) -> bool;
};

template <> struct std::formatter<Position> {
    static constexpr auto parse(const std::format_parse_context &ctx) {
        return ctx.begin();
    }

    static auto format(const Position &p, std::format_context &ctx) {
        return std::format_to(
            ctx.out(), "Position(frame: {}, time: {}, end: {})",
            p.frame ? std::to_string(p.frame.value()) : "None",
            p.time ? std::format("{}", p.time.value()) : "None", p.end);
    }
};

std::istream &operator>>(std::istream &is, Position &t);
#endif // PICK_FRAME_POSITION_H
