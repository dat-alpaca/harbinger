#pragma once
#include "core/defines.hpp"

namespace har::platform
{
    struct window_settings
    {
        const char* title = nullptr;
        i32 minWidth    = 0;
        i32 minHeight   = 0;
        i32 vsyncFrames = 0;
    };

    struct window_api_settings
    {
        u8 major;
        u8 minor;
    };
}