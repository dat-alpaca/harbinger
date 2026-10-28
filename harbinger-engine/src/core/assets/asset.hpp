#pragma once
#include <filesystem>
#include "asset_type.hpp"
#include "core/defines.hpp"
#include "core/utils/uuid.hpp"

namespace har
{
    struct asset_meta
    {
        std::filesystem::path filepath;
        uuid_t assetUUID;
    };

    enum class asset_status : u8
    {
        unloaded,
        loaded,
    };

    struct asset
    {
    public:
        virtual ~asset() = default;

    public:
        asset_meta metadata;
        asset_status status : 1 = asset_status::unloaded;
        asset_type type     : 3 = asset_type::unknown;
    };

    using asset_handle = small_handle_t;
}