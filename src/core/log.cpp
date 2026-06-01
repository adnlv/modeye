#include "Log.hpp"
#include <spdlog/sinks/stdout_color_sinks.h>

std::shared_ptr<spdlog::logger> Modeye::Log::s_logger;

void Modeye::Log::init()
{
    // %^   -> Start color region (for level)
    // [%T] -> Time (HH:MM:SS)
    // [%n] -> Logger name ("MODEYE")
    // [%l] -> Log level (info, warn, error, etc.)
    // %v   -> The actual log message
    // %$   -> End color region
    spdlog::set_pattern("%^[%T] [%n] [%l] %v%$");

    s_logger = spdlog::stdout_color_mt("MODEYE");
    s_logger->set_level(spdlog::level::trace);
}

const std::shared_ptr<spdlog::logger>& Modeye::Log::logger()
{
    return Modeye::Log::s_logger;
}
