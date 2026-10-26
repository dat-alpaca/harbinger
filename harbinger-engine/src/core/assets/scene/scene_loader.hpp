#pragma once
#include <expected>
#include <nlohmann/json.hpp>

#include "core/defines.hpp"
#include "core/utils/file.hpp"
#include "core/assets/asset_error.hpp"
#include "core/scene/scene_holder.hpp"
#include "core/assets/scene/scene_asset.hpp"
#include "core/serializers/scene_serializer.hpp"

namespace har
{
    inline std::expected<scene_unloaded_asset_data, asset_load_error> asset_scene_load(const std::filesystem::path& filepath, scene_holder& sceneHolder, scene_asset* asset)
    {
        nlohmann::json sceneObject = read_json(filepath);

        asset->handle = sceneHolder.create_scene();
        if (asset->handle == invalid_small_handle)
            return std::unexpected { asset_load_error::failed_to_load };

        auto* scene = sceneHolder.get_scene(asset->handle);
        if (!scene)
            return std::unexpected { asset_load_error::failed_to_load };

        scene_unloaded_asset_data output;
        deserialize_scene(sceneObject, *scene, output);
        
        return output;
    }
}