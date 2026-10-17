#pragma once
#include "core/defines.hpp"
#include "renderer/material.hpp"
#include "renderer/mesh.hpp"
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
                (static_cast<u64>(meshHandle)   << 32) |
                (static_cast<u64>(meshHandle)   << 16);
        }

    public:
        graphics_pipeline_handle pipeline;  // u16
        mesh_handle meshHandle;
        material_instance_handle materialInstanceHandle; 
    };
}