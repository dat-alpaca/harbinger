#pragma once
#include "core/containers/containers.hpp"
#include "core/assets/asset_bank_option.hpp"

namespace har
{
    using asset_bank = har_unordered_map<std::string, har_unique<asset_bank_option>>;
}