#pragma once
#include "core/assets/asset_bank_option.hpp"
#include "core/assets/texture/texture_asset.hpp"
#include "rhi/common/objects.hpp"

namespace har
{
    struct texture_asset_bank_options : public asset_bank_option
    {
        // Upload options:
        texture_descriptor gpuOptions;
        sampler_descriptor samplerOptions;
        format_t dataFormat;
        data_type dataType;

        // Load options:
        texture_compression compression;
    };
}