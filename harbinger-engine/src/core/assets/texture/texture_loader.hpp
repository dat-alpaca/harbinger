#pragma once
#include <cassert>
#include <string.h>
#include <expected>
#include <filesystem>
#include <stb_image.h>

#include "core/defines.hpp"
#include "core/memory/allocators.hpp"
#include "core/memory/memory_tag.hpp"
#include "texture_asset.hpp"
#include "core/assets/asset.hpp"

namespace har
{
    constexpr inline u8 DefaultChannelCount = 0; 

    struct texture_load_options
    {
        u8 desiredChannels  = DefaultChannelCount;
        bool freeHostMemory = false;
        // TODO: float texture loading
    };

    inline std::expected<texture_asset, asset_load_error> load_texture_task(const std::filesystem::path& filepath, const texture_load_options& options)
    {
        using namespace har::internal;
        UNUSED options.freeHostMemory;

        if (!std::filesystem::exists(filepath))
            return std::unexpected { asset_load_error::invalid_filepath };

        i32 width, height, channels;
        byte* data = stbi_load(filepath.c_str(), &width, &height, &channels, options.desiredChannels);
        if (!data)
            return std::unexpected { asset_load_error::failed_to_load };

        byte* newData = static_cast<byte*>(MainDynamicAllocator.allocate(memory_tag::assets, width * height * channels, alignof(u8)));
        mempcpy(newData, data, width * height * channels * sizeof(byte));

        stbi_image_free(data);
        return texture_asset(newData, width, height, channels);
    }
    
    inline void unload_texture_task(byte* data)
    {
        assert(data);
        using namespace har::internal;
        
        MainDynamicAllocator.deallocate(memory_tag::assets, data);
    }
}