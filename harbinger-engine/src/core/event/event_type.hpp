#pragma once
#include "core/defines.hpp"

namespace har
{
    enum class event_type : u8
    {
        invalid,
        window_close,
        window_resize,

        scene_created,
        scene_loaded,
        scene_switched,

        texture_loaded,
        texture_uploaded,
        request_texture_upload,

        pso_loaded,
        pso_uploaded,
        request_pso_upload,
    };
}