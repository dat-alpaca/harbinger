#pragma once
#include "rhi/common/objects.hpp"
#include "core/defines.hpp"
#include "core/containers/containers.hpp"

namespace har
{
    struct descriptor_item_gl
    {
        small_handle_t handle;
        small_handle_t extraHandle;
        u16 binding;
        descriptor_set_type type;
    };

    struct descriptor_gl
    {
        har_vector<memory_tag::rhi, descriptor_item_gl> descriptors;
    };
}