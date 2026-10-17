#pragma once
#include "platform/window.hpp"
#include "platform/window_settings.hpp"

namespace har::platform
{
    void pre_initialize(const window_api_settings& settings);
    void post_initialize(window_t& window);
    void present(window_t& window);

    u32 get_required_windowing_dynamic_memory();
}