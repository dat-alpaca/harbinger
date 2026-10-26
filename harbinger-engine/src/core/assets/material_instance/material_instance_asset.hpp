#pragma once
#include "core/defines.hpp"
#include "core/assets/asset.hpp"
#include "core/memory/memory_tag.hpp"
#include "core/containers/containers.hpp"

namespace har
{
    enum class material_instance_type : u8
    {
        none,
        plain_colored,
        custom,
    };

    struct material_instance_asset : public asset
    {
        material_instance_type type;
        har_vector<memory_tag::assets, byte> data;
    };
}