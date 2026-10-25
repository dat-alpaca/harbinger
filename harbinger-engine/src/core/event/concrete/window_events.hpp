#pragma once
#include "core/event/event.hpp"
#include "core/event/event_queue.hpp"
#include "core/event/event_type.hpp"

namespace har
{
    struct window_resize_event
    {
        i32 width;
        i32 height;
    };

    inline void send_window_close_event(event_queue& queue)
    {
        queue.push(event { event_type::window_close });
    }

    inline void send_window_resize_event(event_queue& queue, window_resize_event&& data) 
    {
        queue.push(event { event_type::window_resize, std::move(data) });
    };
}