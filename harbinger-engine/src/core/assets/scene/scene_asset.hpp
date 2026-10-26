#pragma once
#include "core/scene/scene.hpp"
#include "core/assets/asset.hpp"
#include "core/utils/uuid.hpp"

namespace har
{
    struct scene_asset : public asset
    {
        scene_handle handle = invalid_small_handle;
    };

    struct scene_unloaded_asset_data
    {   
        struct entry
        {
            std::filesystem::path filepath;
            asset_type type = asset_type::unknown;
            uuid assedUUID;
        };

        har_vector<memory_tag::scene, entry> entries;
    };
}