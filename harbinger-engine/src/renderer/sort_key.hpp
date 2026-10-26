#pragma once
#include "core/defines.hpp"
#include "rhi/common/objects.hpp"

namespace har
{
    struct sort_key
    {
    public:
        inline u64 pack() const
        {
            return 
                (static_cast<u64>(pipeline)     << 48) |
                (static_cast<u64>(0)            << 32) |
                (static_cast<u64>(0)            << 16);
        }

    public:
        graphics_pipeline_handle pipeline;  // u16
    };
}