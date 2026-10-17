#pragma once
#include "core/defines.hpp"
#include "rhi/common/objects.hpp"

namespace har
{
    struct material_instance
    {
        graphics_pipeline_handle pipeline   = invalid_small_handle;
        descriptor_set_handle descriptorSet = invalid_small_handle;
    };

    using material_instance_handle = small_handle_t;
}