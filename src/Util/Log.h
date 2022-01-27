#ifndef CIRI_LOG_H
#define CIRI_LOG_H

#include <memory>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace Ciri
{
    class Log
    {
    public:
        static void Init();
        inline static std::shared_ptr<spdlog::logger> &GetLogger() { return s_Logger; }

    private:
        static std::shared_ptr<spdlog::logger> s_Logger;
    };
}

#define CIRI_LOG(...) ::Ciri::Log::GetLogger()->trace(__VA_ARGS__)

#endif