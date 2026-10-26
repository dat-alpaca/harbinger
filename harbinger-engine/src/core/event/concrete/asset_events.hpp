#pragma once
#include "core/event/event.hpp"
#include "core/assets/asset.hpp"
#include "core/event/event_queue.hpp"

namespace har
{
    struct asset_loaded_event_data
    {
        asset_handle handle;
        asset_type type;
    };
    inline void send_asset_loaded_event(event_queue& queue, asset_loaded_event_data&& data)
    {
        queue.push(event { event_type::asset_loaded, std::move(data) });
    }
}