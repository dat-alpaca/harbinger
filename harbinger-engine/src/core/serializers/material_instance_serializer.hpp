#pragma once
#include <nlohmann/json.hpp>
#include "core/assets/material_instance/material_instance_asset.hpp"

namespace har
{
    NLOHMANN_JSON_SERIALIZE_ENUM(material_instance_type, 
    {
        { material_instance_type::none, "none" },
        { material_instance_type::plain_colored, "plain_colored" },
        { material_instance_type::custom, "custom" },
    });
}