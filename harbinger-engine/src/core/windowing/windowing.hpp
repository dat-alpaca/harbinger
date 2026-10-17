#pragma once
#include "core/defines.hpp"
#include "platform/window.hpp"
#include "platform/window_settings.hpp"

namespace har
{
    class harbinger_engine;

    class windowing_system
    {
    public:
        void initialize(NON_OWNING harbinger_engine* engine, const platform::window_api_settings& api, const platform::window_settings& settings);

    public:
        window_t window;
    };
}
