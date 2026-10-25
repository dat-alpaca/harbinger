#pragma once
#include "core/event/event_queue.hpp"
#include "platform/window_settings.hpp"

#ifdef HAR_LINUX
#   include "platform/glfw/window_glfw.hpp"
    using window_t = har::platform::window_glfw;
#else
    #error Platform not supported
#endif

namespace har::platform
{
    void initialize_windowing();

    window_t window_initialize(const window_settings& settings, NON_OWNING event_queue* queue);

    void window_set_user_pointer(window_t* window);
    void window_set_vsync(const window_t& window, u32 frames);

    bool window_is_open(const window_t& window);

    void window_poll_events(const window_t& window);

    void window_hide(const window_t& window);
    void window_show(const window_t& window);

    void window_get_dimensions(const window_t& window, i32& width, i32& height);
}
