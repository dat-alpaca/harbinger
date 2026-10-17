#pragma once
#include "core/defines.hpp"
#include <cstring>

namespace har
{
    using asset_handle = small_handle_t;

    enum class asset_load_error
    {
        invalid_filepath,
        failed_to_load
    };

    inline const char* get_asset_load_error_string(asset_load_error error)
    {
        switch(error)
        {
            case asset_load_error::invalid_filepath: return "invalid_filepath";
            case asset_load_error::failed_to_load: return "failed_to_load";
        };

        return "invalid load error";
    }

    enum class asset_type
    {
        invalid,
        pso,
        scene,
        script,
        texture
    };
    inline asset_type get_asset_type_from_name(const char* name)
    {
        if (strcmp(name, "pso") == 0)
            return asset_type::pso;

        else if (strcmp(name, "scene") == 0)
            return asset_type::scene;

        else if (strcmp(name, "script") == 0)
            return asset_type::script;

        else if (strcmp(name, "texture") == 0)
            return asset_type::texture;
        
        return asset_type::invalid;
    }
}