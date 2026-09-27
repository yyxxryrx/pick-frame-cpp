//
// Created by yyxxryrx on 2025/11/11.
//

#ifndef PICK_FRAME_CLI_H
#define PICK_FRAME_CLI_H
#include <string>
#include "types/position.h"
struct Cli {
    std::string input;
    std::string output;
    Position from;
    Position to;

    static auto parse(int argc, char *argv[]) -> Cli;
};

#endif //PICK_FRAME_CLI_H