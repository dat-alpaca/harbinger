#pragma once
#include "core/assets/texture/texture_bank_options.hpp"
#include "nlohmann/json.hpp"

namespace har
{
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(texture_asset_bank_options, compression, dataFormat, dataType, gpuOptions, samplerOptions);
}