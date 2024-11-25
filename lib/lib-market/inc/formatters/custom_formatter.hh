#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include <fmt/format.h>

namespace market
{
    inline std::string yellow(const std::string &text) { return fmt::format("\033[33m{}\033[0m", text); }
    inline std::string red(const std::string &text) { return fmt::format("\033[31m{}\033[0m", text); }
    inline std::string blue(const std::string &text) { return fmt::format("\033[34m{}\033[0m", text); }
    inline std::string green(const std::string &text) { return fmt::format("\033[32m{}\033[0m", text); }
} // namespace market