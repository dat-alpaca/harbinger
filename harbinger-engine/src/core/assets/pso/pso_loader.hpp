#pragma once
#include <expected>
#include <filesystem>

#include "core/utils/file.hpp"
#include "core/assets/asset_error.hpp"
#include "core/assets/pso/pso_asset.hpp"

namespace har
{
    inline std::expected<pso_asset, asset_load_error> asset_pso_load(const std::filesystem::path& filepath)
    {
        using namespace har::internal;

        auto result = parse_json<pso_asset>(filepath);
        if (!result.has_value())
            return std::unexpected { asset_load_error::failed_to_load };

        return result.value();
    }
}