#pragma once
#include <expected>
#include <stb_image.h>
#include <filesystem>
#include <squish/squish.h>

#include "core/memory/allocators.hpp"
#include "core/assets/asset_error.hpp"
#include "core/assets/texture/texture_asset.hpp"

namespace har
{
    inline std::expected<texture_asset, asset_load_error> asset_texture_load(const std::filesystem::path& filepath)
    {
        i32 width, height, channels;
        byte* data = stbi_load(filepath.c_str(), &width, &height, &channels, DefaultDesiredChannels);
        if (!data)
            return std::unexpected { asset_load_error::failed_to_load };

        return texture_asset(data, width, height, channels);
    }
    
    inline void asset_texture_free(byte* data)
    {
        HAR_ASS(data);
        stbi_image_free(data);
    }
}

namespace
{
    static inline har::i32 get_squish_flags(har::texture_compression compression)
    {
        switch (compression)
        {
            case har::texture_compression::dxt1: 
                return squish::kDxt1;
            
            case har::texture_compression::dxt3:
                return squish::kDxt3;
            
            case har::texture_compression::dxt5: 
                return squish::kDxt5;

            default:
            {
                HAR_LOG_CRITICAL("Invalid texture compression option");
                return 0;
            }
        }
    }
}

namespace har
{
    inline void asset_texture_compress(texture_asset& asset, texture_compression compressionLevel)
    {
        using namespace har::internal;

        i32 cFlags = get_squish_flags(compressionLevel);
        i32 cSize = squish::GetStorageRequirements(asset.width, asset.height, cFlags);
        
        byte* compressedData = static_cast<u8*>(MainDynamicAllocator.allocate(memory_tag::assets, cSize, alignof(u8)));
        
        squish::CompressImage(asset.data, asset.width, asset.height, asset.width * asset.channels,
                              compressedData, cFlags);
                              
        MainDynamicAllocator.deallocate(memory_tag::assets, asset.data);
        asset.data = compressedData;
    }
}