#pragma once
#include "core/defines.hpp"
#include "core/assets/asset.hpp"
#include "core/memory/memory_tag.hpp"
#include "core/assets/custom_format.hpp"
#include "core/containers/containers.hpp"
#include <variant>

namespace har
{
    struct material_instance_entry
    {
        custom_format format = custom_format::none;
        std::variant<u32, i32, f32, uuid_t, glm::vec2, glm::vec3, glm::vec4> value;
    };

    struct material_instance_asset : public asset
    {
        har_vector<memory_tag::assets, material_instance_entry> entries;
    };
}