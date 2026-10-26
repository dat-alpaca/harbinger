#pragma once
#include "core/defines.hpp"

namespace har
{
    enum class asset_load_error : u8
    {
        none,
        failed_to_load
    };

    constexpr inline const char* get_asset_load_error_string(asset_load_error error)
    {
        switch(error)
        {
            case asset_load_error::none: return "none";
            case asset_load_error::failed_to_load: return "failed_to_load";
        };

        return "invalid load error";
    }
}