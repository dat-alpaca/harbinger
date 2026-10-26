#pragma once
#include <cstring>
#include "core/defines.hpp"

namespace har
{
    enum class asset_type : u8
    {
        unknown,
        pipeline_object,
        scene,
        script,
        texture,
    };

    constexpr inline const char* get_name_from_asset(asset_type type)
    {
        switch (type)
        {
            case asset_type::pipeline_object:
                return "pso";
                
            case asset_type::scene:
                return "scene";

            case asset_type::script: 
                return "script";

            case asset_type::texture: 
                return "texture";

            default:
                return "unknown";
        }
    }

    constexpr inline asset_type get_asset_type_from_name(const char* name)
    {
        if (std::strcmp(name, "pso") == 0)
            return asset_type::pipeline_object;

        else if (std::strcmp(name, "scene") == 0)
            return asset_type::scene;

        else if (std::strcmp(name, "script") == 0)
            return asset_type::script;

        else if (std::strcmp(name, "texture") == 0)
            return asset_type::texture;
        
        return asset_type::unknown;
    }

    constexpr inline bool is_gpu_uploadable(asset_type type)
    {
        return (type == asset_type::texture || type == asset_type::pipeline_object);
    }
}