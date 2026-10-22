#pragma once
#include "core/defines.hpp"
#include "core/event/event_queue.hpp"

#include "platform/window.hpp"
#include "platform/window_settings.hpp"

namespace har
{
    // TODO: rename
    class windowing_system
    {
    public:
        void initialize(NON_OWNING event_queue* eventQueue, const platform::window_api_settings& api, const platform::window_settings& settings);

    public:
        window_t window;
    };
}
