#pragma once
#include "core/scene/scene.hpp"
#include "core/scene/scene_serialization.hpp"
#include "core/utils/file.hpp"
#include "nlohmann/json.hpp"

namespace har
{
    inline scene_asset_data load_scene_task(const std::filesystem::path& filepath, scene_t& scene)
    {
        nlohmann::json sceneObject = read_json(filepath);
    
        scene_asset_data output;
        deserialize_scene(sceneObject, scene, output);
        
        return output;
    }
}