#pragma once
#include "core/defines.hpp"

namespace har
{
    enum class event_type : u8
    {
        invalid,
        window_close,
        window_resize,

        // scene:
        scene_created,
        scene_switched,
        scene_assets_loaded,

        // assets:
        asset_loaded,
        asset_uploaded,
        asset_linked_after_load,
        asset_linked_after_upload,

        // renderer:
        renderer_texture_pushed
    };
}