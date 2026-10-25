#pragma once
#include <filesystem>
#include "core/engine/context.hpp"

namespace har
{
    class harbinger_engine
    {
    public:
        void initialize(const char* configFilepath = "config.ini");
        void shutdown();

    public:
        void start_frame();
        void on_update();

    public:
        void invoke_startup_script(const std::filesystem::path& filepath);

    public:
        bool is_running() const;

    public:
        context_t context;
    };
}
