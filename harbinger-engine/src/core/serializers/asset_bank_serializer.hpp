#pragma once
#include <filesystem>
#include "core/utils/file.hpp"
#include "core/assets/asset_bank.hpp"
#include "asset_bank_texture_options.hpp"

namespace
{
    using namespace har;

    static inline void load_texture_bank_options(const std::filesystem::path& filepath, const nlohmann::json& details, asset_bank& bank)
    {

        if (!details.contains("options"))
        {
            HAR_LOG_WARN("[Asset Bank]: Asset option with no valid descriptive 'option'.");
            return;
        }
    
        if (!details.contains("type"))
        {
            HAR_LOG_WARN("[Asset Bank]: Asset option with no valid descriptive 'type'.");
            return;
        }

        if (details["type"] == "texture")
        {
            texture_asset_bank_options options = details["options"].get<texture_asset_bank_options>();
            bank[std::string(filepath)] = har::make_unique<texture_asset_bank_options>(options);
        }
        else
            HAR_LOG_WARN("[Asset Bank]: Asset has invalid 'type'.");
    }
}

namespace har
{
    inline void deserialize_asset_bank(const std::filesystem::path& bankFilepath, asset_bank& bank)
    {
        auto jsonObject = read_json(bankFilepath);

        for (auto& [filepath, details] : jsonObject.items())
        {
            if (!details.contains("type"))
            {
                HAR_LOG_WARN("[Asset Bank]: Field with no valid specified 'type'.");
                continue;
            }

            load_texture_bank_options(filepath, details, bank);
        }
    }
}