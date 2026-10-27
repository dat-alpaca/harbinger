#pragma once
#include <expected>
#include <filesystem>
#include <glm/glm.hpp>

#include "core/containers/containers.hpp"
#include "core/logging/logger.hpp"
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

    enum class custom_format : u8
    {
        none,
        int32,
        float32,
        vec2,
        vec3,
        vec4,
        mat4
    };

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

        return custom_format::none;
    }

    asset_load_error material_instance_load_custom(const nlohmann::json& object, har_vector<memory_tag::assets, byte>& data)
    {
        if (!object.contains("data"))
            return asset_load_error::failed_to_load;

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
                {
                    auto dataValue = entry["data"].get<i32>();
                    const auto* dataPtr = reinterpret_cast<const byte*>(&dataValue);
                    data.insert(data.end(), dataPtr, dataPtr + sizeof(i32));
                } break;

                case custom_format::float32:
                {
                    auto dataValue = entry["data"].get<f32>();
                    const auto* dataPtr = reinterpret_cast<const byte*>(&dataValue);
                    data.insert(data.end(), dataPtr, dataPtr + sizeof(f32));
                } break;

                case custom_format::vec2:
                {
                    auto dataValue = entry["data"].get<glm::vec2>();
                    const auto* dataPtr = reinterpret_cast<const byte*>(&dataValue);
                    data.insert(data.end(), dataPtr, dataPtr + sizeof(glm::vec2));
                } break;

                case custom_format::vec3:
                {
                    auto dataValue = entry["data"].get<glm::vec3>();
                    const auto* dataPtr = reinterpret_cast<const byte*>(&dataValue);
                    data.insert(data.end(), dataPtr, dataPtr + sizeof(glm::vec3));
                } break;

                case custom_format::vec4:
                {
                    auto dataValue = entry["data"].get<glm::vec4>();
                    const auto* dataPtr = reinterpret_cast<const byte*>(&dataValue);
                    data.insert(data.end(), dataPtr, dataPtr + sizeof(glm::vec4));
                } break;

                default:
                    HAR_LOG_ERROR("Invalid format specifier passed to material instance.")
                    break;
            }
            
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

            case material_instance_type::custom:
            {
                if (material_instance_load_custom(object, asset.data) != asset_load_error::none)
                    return std::unexpected { asset_load_error::failed_to_load };
            } break;

            default:
                return std::unexpected { asset_load_error::failed_to_load };
        }

        return asset;
    }
}