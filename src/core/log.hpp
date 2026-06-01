#pragma once
#include <spdlog/spdlog.h>

namespace modeye
{
    class log
    {
    public:
        static void init();

        template<typename... Args>
        static void trace(spdlog::format_string_t<Args...> fmt, Args&&... args)
        {
            log::logger()->trace(fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        static void debug(spdlog::format_string_t<Args...> fmt, Args&&... args)
        {
            log::logger()->debug(fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        static void info(spdlog::format_string_t<Args...> fmt, Args&&... args)
        {
            log::logger()->info(fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        static void warn(spdlog::format_string_t<Args...> fmt, Args&&... args)
        {
            log::logger()->warn(fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        static void error(spdlog::format_string_t<Args...> fmt, Args&&... args)
        {
            log::logger()->error(fmt, std::forward<Args>(args)...);
        }

        static const std::shared_ptr<spdlog::logger>& logger();

    private:
        static std::shared_ptr<spdlog::logger> s_logger;
    };
}
