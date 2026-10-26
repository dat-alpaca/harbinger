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

        // assets:
        asset_loaded,
        asset_uploaded,
    };
}