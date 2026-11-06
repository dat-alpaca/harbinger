#pragma once
#include <expected>
#include <filesystem>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>

#include "core/containers/containers.hpp"
#include "core/logging/logger.hpp"
#include "core/utils/file.hpp"

#include "core/assets/asset_error.hpp"
#include "core/assets/custom_format.hpp"
#include "core/assets/material_instance/material_instance_asset.hpp"

#include "core/utils/json_serializable.hpp"

namespace
{
    using namespace har;

    static inline custom_format get_format_from_json_string(const std::string& string)
    {
        if (string == "int32")
            return custom_format::int32;

        if (string == "float32")
            return custom_format::float32;
        
        if (string == "vec2")
            return custom_format::vec2;
        
        if (string == "vec3")
            return custom_format::vec3;
        
        if (string == "vec4")
            return custom_format::vec4;
        
        if (string == "texture")
            return custom_format::texture;

        return custom_format::none;
    }

    asset_load_error material_instance_load_custom(const nlohmann::json& object, material_instance_asset& asset)
    {
        auto objectData = object["data"];
        if (!objectData.is_array())
            return asset_load_error::failed_to_load;

        if (objectData.empty())
            return asset_load_error::none;

        for (const auto& entry : objectData)
        {
            if (!entry.contains("type"))
            {
                HAR_LOG_ERROR("Material instance loaded custom format has improper type specification");
                continue;
            }

            if (!entry.contains("data"))
            {
                HAR_LOG_ERROR("Material instance loaded custom format has no data field");
                continue;
            }
            
            auto entryType = get_format_from_json_string(entry["type"]);
            switch (entryType)
            {
                case custom_format::int32:
                    asset.entries.push_back(material_instance_entry { .format = entryType, .value = entry["data"].get<i32>() });
                    break;

                case custom_format::float32:
                    asset.entries.push_back(material_instance_entry { .format = entryType, .value = entry["data"].get<f32>() });
                    break;

                case custom_format::vec2:
                    asset.entries.push_back(material_instance_entry { .format = entryType, .value = entry["data"].get<glm::vec2>() });
                    break;

                case custom_format::vec3:
                    asset.entries.push_back(material_instance_entry { .format = entryType, .value = entry["data"].get<glm::vec3>() });
                    break;
                    
                case custom_format::vec4:
                    asset.entries.push_back(material_instance_entry { .format = entryType, .value = entry["data"].get<glm::vec4>() });
                    break;

                case custom_format::texture:
                    asset.entries.push_back(material_instance_entry { .format = entryType, .value = entry["data"]["uuid"].get<uuid_t>() });
                    break;

                default:
                {
                    HAR_LOG_ERROR("Invalid format specifier passed to material instance.")
                } break;
            }
        }

        asset.entries.shrink_to_fit();
        return asset_load_error::none;
    }
}

namespace har
{
    inline std::expected<material_instance_asset, asset_load_error> asset_material_instance_load(const std::filesystem::path& filepath)
    {
        using namespace har::internal;

        nlohmann::json object = read_json(filepath);
        if (!object.contains("data"))
            return std::unexpected { asset_load_error::failed_to_load };

        material_instance_asset asset;
        if (material_instance_load_custom(object, asset) != asset_load_error::none)
            return std::unexpected { asset_load_error::failed_to_load };

        return asset;
    }
}