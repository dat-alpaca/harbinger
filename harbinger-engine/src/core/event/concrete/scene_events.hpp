#pragma once
#include "core/event/event.hpp"
#include "core/event/event_queue.hpp"
#include "core/scene/scene.hpp"

namespace har
{
    struct scene_create_data
    {
        NON_OWNING scene_t* createdScene; 
    };

    struct scene_switch_data
    {
        NON_OWNING scene_t* previous = nullptr; 
        NON_OWNING scene_t* current; 
    };
}

namespace har
{
    inline void send_scene_created_event(event_queue& queue, scene_create_data&& data)
    {
        queue.push(event { event_type::scene_created, std::move(data) });
    }

    inline void send_scene_switched_event(event_queue& queue, scene_switch_data&& data)
    {
        queue.push(event { event_type::scene_switched, std::move(data) });
    }
}
