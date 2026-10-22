#pragma once
#include "core/assert/assert.hpp"
#include "core/defines.hpp"

namespace har
{
    enum class texture_compression : u8
    {
        none,
        dxt1,
        dxt3,
        dxt5
    };

    struct texture_asset
    {
    public:
        texture_asset(byte* data, u16 width, u16 height, u8 channels)
            : data(data)
            , width(width)
            , height(height)
            , channels(channels)
        {
            HAR_ASS(data);
            HAR_ASS(width    < 16384);
            HAR_ASS(height   < 16384);
            HAR_ASS(channels < 16);
        }

        texture_asset() = default;

    public:
        byte* data        = nullptr;
        u16 width    : 14 = 0;       
        u16 height   : 14 = 0;
        u8 channels  : 4  = 0;
        texture_compression compressionLevel = texture_compression::none;
    };
}