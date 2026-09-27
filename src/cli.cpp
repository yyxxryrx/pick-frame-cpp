//
// Created by yyxxryrx on 2025/11/11.
//
#include "cli.h"

#include <cxxopts.hpp>
#include <print>

auto Cli::parse(const int argc, char *argv[]) -> Cli {
    Cli cli;
    try {
        cxxopts::Options options("pick frame");
        options.add_options()
                ("i,input", "the path of video", cxxopts::value<std::string>())
                ("f,from", "the start you want cut", cxxopts::value<Position>(cli.from)->default_value("0"))
                ("t,to", "the end you want cut", cxxopts::value<Position>(cli.to)->default_value("end"))
                ("h,help", "print help message")
                ("V,version", "print version")
                ("output", "The output folder", cxxopts::value<std::string>(cli.output)->default_value("."));

        options.parse_positional({"output"});
        options.positional_help("[output]");

        const auto result = options.parse(argc, argv);
        if (result.count("help")) {
            std::println("{}", options.help());
            std::exit(EXIT_SUCCESS);
        }
        if (result.count("version")) {
            std::println("pick frame v1.0.0");
            std::exit(EXIT_SUCCESS);
        }
        if (!result.count("input")) {
            std::println("You miss the input");
            std::exit(EXIT_FAILURE);
        }
        cli.input = result["input"].as<std::string>();
    } catch (cxxopts::exceptions::exception &e) {
        std::println("{}", e.what());
        std::exit(EXIT_FAILURE);
    }
    return cli;
}
