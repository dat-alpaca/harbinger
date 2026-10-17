#pragma once
#include "core/defines.hpp"

namespace har
{
    enum class memory_tag : u8
    {
        none = 0,
        string,

        windowing,
        script_system,
        worker_system,
        event_queue,
        logging,

        scene,
        
        rhi,
        renderer,
        assets,

        amount,
    };
}