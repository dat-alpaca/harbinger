#pragma once
#include "core/event/event_queue.hpp"
#include "core/input/keys.hpp"
#include "platform/window_callback.hpp"
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
    void window_get_framebuffer_dimensions(const window_t& window, i32& width, i32& height);

    const char* window_get_clipboard_string(const window_t& window);
    void window_set_clipboard_string(const window_t& window, const char* string);

    input_action window_get_key(const window_t& window, key_t key);
    input_action window_get_mouse_button(const window_t& window, mouse_button button);
    void window_get_cursor_position(const window_t& window, f64* x, f64* y);
    void window_set_cursor_position(const window_t& window, f64 x, f64 y);

    f64 window_get_time_seconds(const window_t& window);

    void window_show_cursor(const window_t& window);
    void window_hide_cursor(const window_t& window);
}

namespace har::platform
{
    void window_set_mouse_button_callback(window_t& window, mouse_button_clbk&& callback);

    void window_set_scroll_callback(window_t& window, scroll_clbk&& callback);

    void window_set_character_callback(window_t& window, character_clbk&& callback);

    void window_set_key_callback(window_t& window, key_clbk&& callback);
}