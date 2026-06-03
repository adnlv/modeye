#pragma once
#include <spdlog/spdlog.h>

namespace Modeye
{

class Log
{
public:
    static void init();

    template<typename... Args>
    static void trace(spdlog::format_string_t<Args...> fmt, Args&&... args)
    {
        Log::logger()->trace(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    static void debug(spdlog::format_string_t<Args...> fmt, Args&&... args)
    {
        Log::logger()->debug(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    static void info(spdlog::format_string_t<Args...> fmt, Args&&... args)
    {
        Log::logger()->info(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    static void warn(spdlog::format_string_t<Args...> fmt, Args&&... args)
    {
        Log::logger()->warn(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    static void error(spdlog::format_string_t<Args...> fmt, Args&&... args)
    {
        Log::logger()->error(fmt, std::forward<Args>(args)...);
    }

    static const std::shared_ptr<spdlog::logger>& logger();

private:
    static std::shared_ptr<spdlog::logger> s_logger;
};

}
