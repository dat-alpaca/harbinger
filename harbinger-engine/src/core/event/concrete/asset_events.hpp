#pragma once
#include "core/assets/asset.hpp"
#include "core/assets/pso/pso_asset.hpp"
#include "core/defines.hpp"
#include "core/event/event.hpp"
#include "core/event/event_queue.hpp"
#include "core/scene/scene.hpp"
#include "rhi/common/objects.hpp"

namespace har
{
    enum class asset_upload_options : u8
    {
        none                = 0,
        upload_immediate    = 1 << 0,
    };
    HAR_DEFINE_ENUM(asset_upload_options);
}

namespace har
{
    struct texture_loaded_data
    {
        asset_handle handle;
        asset_upload_options options;
        bool freeHostMemory;
    };
    
    struct texture_upload_request_data
    {
        asset_handle handle;
        bool freeHostMemory;
    };

    struct texture_uploaded_data
    {
        asset_handle handle;
        combined_texture_handle combinedTextureHandle;
        bool freeHostMemory;
    };

    inline void send_texture_loaded_event(event_queue& queue, texture_loaded_data&& data)
    {
        queue.push(event { event_type::texture_loaded, std::move(data) });
    }

    inline void send_texture_upload_request_event(event_queue& queue, texture_upload_request_data&& data)
    {
        queue.push(event { event_type::request_texture_upload, std::move(data) });
    }

    inline void send_texture_uploaded_event(event_queue& queue, texture_uploaded_data&& data)
    {
        queue.push(event { event_type::texture_uploaded, std::move(data) });
    }
}

namespace har
{
    struct pso_loaded_data
    {
        asset_handle handle;
    };
    struct pso_uploaded_data
    {
        asset_handle handle;
        graphics_pipeline_handle pipeline;
    };

    inline void send_pipeline_state_loaded_event(event_queue& queue, pso_loaded_data&& data)
    {
        queue.push(event { event_type::pso_loaded, std::move(data) });
    }

    inline void send_pipeline_state_upload_request_event(event_queue& queue, pso_loaded_data&& data)
    {
        queue.push(event { event_type::request_pso_upload, std::move(data) });
    }

    inline void send_pipeline_uploaded_event(event_queue& queue, pso_uploaded_data&& data)
    {
        queue.push(event { event_type::pso_uploaded, std::move(data) });
    }
}

namespace har
{
    struct scene_loaded_data
    {
        scene_handle handle;
    };

    inline void send_scene_loaded_event(event_queue& queue, scene_loaded_data&& data)
    {
        queue.push(event { event_type::scene_loaded, std::move(data) });
    }
}