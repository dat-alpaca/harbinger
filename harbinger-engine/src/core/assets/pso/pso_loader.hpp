#pragma once
#include <expected>
#include <filesystem>

#include "core/utils/file.hpp"
#include "core/assets/asset.hpp"
#include "core/assets/pso/pso_asset.hpp"

namespace har
{
    inline std::expected<pso_asset, asset_load_error> load_pso_task(const std::filesystem::path& filepath)
    {
        using namespace har::internal;

        if (!std::filesystem::exists(filepath))
            return std::unexpected { asset_load_error::invalid_filepath };

        auto result = parse_json<pso_asset>(filepath);
        if (!result.has_value())
            return std::unexpected { asset_load_error::failed_to_load };

        return result.value();
    }
}