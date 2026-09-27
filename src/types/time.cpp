//
// Created by yyxxryrx on 2025/11/11.
//

#include "time.h"

auto Time::to_secs() const -> double {
    return this->hours * 3600 + this->minutes * 60 + this->seconds + static_cast<double>(this->milliseconds) / 1000.0;
}
