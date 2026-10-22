#include "asset_bank.hpp"
#include "core/assert/assert.hpp"
#include "core/logging/logger.hpp"
#include "core/utils/file.hpp"

namespace har
{
    void asset_bank::initialize(u32 maxTextureCount)
    {
        mTextureBank.reserve(maxTextureCount);
    }

    void asset_bank::load_bank(const std::filesystem::path& bankFilepath)
    {
        auto jsonObject = read_json(bankFilepath);

        for (auto& [filepath, details] : jsonObject.items())
        {
            if (!details.contains("type"))
            {
                HAR_LOG_WARN("[Asset Bank]: Field with no valid specified 'type'.");
                continue;
            }

            load_texture_bank_options(filepath, details);
        }
    }

    void asset_bank::load_texture_bank_options(const std::filesystem::path& filepath, const nlohmann::json& details)
    {
        if (details["type"] != "texture")
            return;
        
        if (!details.contains("options"))
        {
            HAR_LOG_WARN("[Asset Bank]: Texture option with no valid descriptive 'option'.");
            return;
        }

        texture_asset_bank_options options = details["options"].get<texture_asset_bank_options>();
        mTextureBank[std::string(filepath)] = har::make_unique<texture_asset_bank_options>(options);
    }
    
    texture_asset_bank_options* asset_bank::get_texture_options(const std::filesystem::path& filepath)
    {
        // TODO: handle error
        HAR_ASS(mTextureBank.contains(filepath));
        return static_cast<texture_asset_bank_options*>(mTextureBank[filepath.string()].get());
    }
}