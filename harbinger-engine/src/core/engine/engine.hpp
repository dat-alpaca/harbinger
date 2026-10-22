#pragma once
#include "core/engine/context.hpp"

namespace har
{
    class harbinger_engine
    {
    public:
        harbinger_engine(const char* configFilepath = "config.ini");
        ~harbinger_engine();

    public:
        void start_frame();
        void on_update();

    public:
        bool is_running() const;

    public:
        context_t context;
    };
}
