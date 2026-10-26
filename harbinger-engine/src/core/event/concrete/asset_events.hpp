#pragma once
#include "core/defines.hpp"
#include "core/event/event.hpp"
#include "core/assets/asset.hpp"
#include "core/event/event_queue.hpp"

namespace har
{
    struct asset_loaded_event_data
    {
        asset_handle handle = invalid_small_handle;
        asset_type type = asset_type::unknown;
    };
    inline void send_asset_loaded_event(event_queue& queue, asset_loaded_event_data&& data)
    {
        queue.push(event { event_type::asset_loaded, std::move(data) });
    }

    struct asset_uploaded_event_data
    {
        asset_handle assetHandle = invalid_small_handle;
        handle_t gpuHandle = invalid_handle;
        asset_type type = asset_type::unknown;
    };
    inline void send_asset_uploaded_event(event_queue& queue, asset_uploaded_event_data&& data)
    {
        queue.push(event { event_type::asset_uploaded, std::move(data) });
    }

    inline void send_scene_assets_loaded_event(event_queue& queue)
    {
        queue.push(event { event_type::scene_assets_loaded });
    }
}