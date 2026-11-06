#pragma once
#include <expected>
#include <nlohmann/json.hpp>

#include "core/assets/asset_error.hpp"
#include "core/scene/renderer_component.hpp"
#include "entt/entity/fwd.hpp"
#include "core/logging/logger.hpp"
#include "core/containers/containers.hpp"
#include "core/assets/scene/scene_asset.hpp"

#include "core/scene/scene.hpp"
#include "core/scene/entity.hpp"
#include "core/scene/components.hpp"
#include "core/utils/json_serializable.hpp"

namespace
{
    using namespace har;

    inline std::expected<scene_unloaded_asset_data, asset_load_error> deserialize_assets(const nlohmann::json& sceneObject)
    {
        using namespace har;
        scene_unloaded_asset_data data;

        if (!sceneObject.contains("assets") || !sceneObject["assets"].is_array())
        {
            HAR_LOG_ERROR("Invalid scene object: invalid asset registry");
            return std::unexpected { asset_load_error::failed_to_load };
        }

        for (const auto& assetObject : sceneObject["assets"])
        {
            if (!assetObject.contains("filepath") || !assetObject.contains("type") || !assetObject.contains("uuid") )
            {
                HAR_LOG_WARN("Invalid asset entry. Assets must have a 'filepath', 'uuid' and 'type' field.");
                continue;
            }

            har_string typeText = assetObject["type"].get<har_string>();
            asset_type type = get_asset_type_from_name(typeText.c_str());
            
            if (type == asset_type::unknown)
            {
                HAR_LOG_WARN("[Scene] Invalid scene asset entry: {}", assetObject["filepath"].get<std::string>());
                continue;
            }

            uuid_t assetUUID = assetObject["uuid"].get<uuid_t>();

            data.entries.push_back({ .filepath = assetObject["filepath"], .type = type, .assedUUID = assetUUID });
        }

        return data;
    }

    inline void deserialize_entities(const nlohmann::json& sceneObject, har::scene_t& scene)
    {
        using namespace har;

        if (!sceneObject.contains("entities") || !sceneObject["entities"].is_array())
        {
            HAR_LOG_CRITICAL("Invalid scene object. Invalid entities registry");
            return;
        }

        for (const auto& entityObject : sceneObject["entities"])
        {
            entity_t entity = scene.create_entity(static_cast<entt::entity>(entityObject["id"]));

            if (entityObject.contains("name"))
                entity.emplace<name_component>(entityObject["name"].get<har_string>().c_str());

            if (entityObject.contains("transform"))
                entity.emplace<transform_component>(entityObject["transform"].get<transform_t>());

            if (entityObject.contains("material_instance"))
            {
                auto& instance = entity.emplace<material_instance_component>();
                instance.assetUUID = static_cast<uuid_t>(entityObject["material_instance"].at("uuid").get<u64>());
            }

            if (entityObject.contains("renderable"))
            {
                auto& instance = entity.emplace<renderable_component>();
                instance.assetUUID = entityObject["renderable"]["pipeline_uuid"];
                instance.isImmutable = entityObject["renderable"]["is_immutable"];
            }

            if (entityObject.contains("mesh"))
            {
                auto& mesh = entity.emplace<mesh_component>();
                mesh.assetUUID = static_cast<uuid_t>(entityObject["mesh"].at("uuid").get<u64>());
            }

            // TODO: Script deferral 
            if (entityObject.contains("script"))
            {
            }
        }
    }
}

namespace har
{
    inline nlohmann::json serialize_scene(const scene_t& scene)
    {
        using namespace nlohmann;

        auto& registry = scene.get_registry();

        json root = json::object();
        json entityList = json::array();


        registry.each([&](entt::entity entity)
        {
            json entityObject;
            entityObject["id"] = static_cast<u32>(entity);

            if (registry.all_of<name_component>(entity))
            {
                auto& nameComponent = registry.get<name_component>(entity);
                entityObject["name"] = nameComponent.name;
            }

            if (registry.all_of<transform_component>(entity))
            {
                auto& transform = registry.get<transform_component>(entity).transform;
                entityObject["transform"] = transform;
            }

            entityList.push_back(entityObject);
        });

        root["entities"] = entityList;
        return root;
    }

    inline void deserialize_scene_registry(const nlohmann::json& sceneObject, scene_t& scene)
    {
        using namespace nlohmann;

        auto& registry = scene.get_registry();
        registry.clear();

        deserialize_entities(sceneObject, scene);
    }

    inline std::expected<scene_unloaded_asset_data, asset_load_error> deserialize_scene_assets(const nlohmann::json& sceneObject)
    {
        return deserialize_assets(sceneObject);
    }
}