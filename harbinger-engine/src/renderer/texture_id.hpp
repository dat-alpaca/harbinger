#pragma once
#include "core/defines.hpp"

namespace har::renderer
{
    using bin_t = u32;
    using layer_t = u32;
}

namespace har
{
    struct texture_id
    {
        renderer::bin_t bin     = invalid_handle;
        renderer::layer_t layer = invalid_handle;
    };
}