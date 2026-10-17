#pragma once
#include <stb_image.h>
#include <squish/squish.h>

#include "core/logging/logger.hpp"
#include "core/assets/texture/texture_asset.hpp"
#include "core/memory/allocators.hpp"
#include "core/memory/memory_tag.hpp"

namespace
{
    static inline har::i32 get_squish_flags(har::texture_compression compression)
    {
        switch (compression)
        {
            case har::texture_compression::dxt1: return squish::kDxt1;
            case har::texture_compression::dxt3: return squish::kDxt3;
            case har::texture_compression::dxt5: return squish::kDxt5;
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
    inline void compress_image_task(texture_asset& asset)
    {
        using namespace har::internal;

        i32 cFlags = get_squish_flags(asset.compressionLevel);
        i32 cSize = squish::GetStorageRequirements(asset.width, asset.height, cFlags);
        
        byte* compressedData = static_cast<u8*>(MainDynamicAllocator.allocate(memory_tag::assets, cSize, alignof(u8)));
        
        squish::CompressImage(asset.data, asset.width, asset.height, asset.width * asset.channels,
                              compressedData, cFlags);
                              
        MainDynamicAllocator.deallocate(memory_tag::assets, asset.data);
        asset.data = compressedData;
    }
}