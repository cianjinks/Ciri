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

#ifdef CIRI_DEBUG
#define CIRI_LOG(...) ::Ciri::Log::GetLogger()->trace(__VA_ARGS__)
#define CIRI_WARN(...) ::Ciri::Log::GetLogger()->warn(__VA_ARGS__)
#define CIRI_ERROR(...) ::Ciri::Log::GetLogger()->error(__VA_ARGS__)
#else
#define CIRI_LOG(...)
#define CIRI_WARN(...)
#define CIRI_ERROR(...)
#endif

#endif