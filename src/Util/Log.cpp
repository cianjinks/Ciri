#include "Log.h"

namespace Ciri
{
    std::shared_ptr<spdlog::logger> Log::s_Logger;

    void Log::Init()
    {
        s_Logger = spdlog::stdout_color_mt("Ciri");
        s_Logger->set_level(spdlog::level::trace);
        spdlog::set_pattern("[%n] %v");
    }
}