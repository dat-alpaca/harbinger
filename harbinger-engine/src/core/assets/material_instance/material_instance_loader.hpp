#pragma once
#include <expected>
#include <filesystem>
#include <glm/glm.hpp>

#include "core/containers/containers.hpp"
#include "core/memory/memory_tag.hpp"
#include "core/utils/file.hpp"
#include "core/assets/asset_error.hpp"
#include "core/assets/material_instance/material_instance_asset.hpp"

#include "core/utils/json_serializable.hpp"
#include "core/serializers/material_instance_serializer.hpp"

namespace
{
    using namespace har;

    asset_load_error material_instance_load_plain_colored(const nlohmann::json& object, har_vector<memory_tag::assets, byte>& data)
    {
        if (!object.contains("data"))
            return asset_load_error::failed_to_load;

        auto objectData = object["data"];
        
        glm::vec4 color(0.0f, 0.0f, 0.0f, 1.0f);
        if (objectData.contains("color"))
            color = objectData.at("color").get<glm::vec4>();

        {
            const auto* dataPtr = reinterpret_cast<const byte*>(&color);
            data.insert(data.end(), dataPtr, dataPtr + sizeof(glm::vec4));
        }

        return asset_load_error::none;
    }
}

namespace har
{
    inline std::expected<material_instance_asset, asset_load_error> asset_material_instance_load(const std::filesystem::path& filepath)
    {
        using namespace har::internal;

        nlohmann::json object = read_json(filepath);
        if (!object.contains("type"))
            return std::unexpected { asset_load_error::failed_to_load };

        material_instance_asset asset;

        asset.type = object.at("type").get<material_instance_type>();
        switch(asset.type)
        {
            case material_instance_type::plain_colored:
            {
                if (material_instance_load_plain_colored(object, asset.data) != asset_load_error::none)
                    return std::unexpected { asset_load_error::failed_to_load };
            } break;

            default:
                return std::unexpected { asset_load_error::failed_to_load };
        }

        return asset;
    }
}