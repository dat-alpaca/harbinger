#pragma once
#include "core/assets/texture/texture_asset.hpp"
#include "nlohmann/json.hpp"
#include "rhi/common/objects.hpp"
#include "core/containers/containers.hpp"

namespace har
{
    struct bank_option
    {
    public:
        virtual ~bank_option() = default;
    };

    struct texture_asset_bank_options : public bank_option
    {
        // Upload options:
        texture_descriptor gpuOptions;
        sampler_descriptor samplerOptions;
        format dataFormat;
        data_type dataType;

        // Load options:
        texture_compression compression;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(texture_asset_bank_options, compression, dataFormat, dataType, gpuOptions, samplerOptions);
    };

    struct asset_bank
    {
    public:
        void initialize(u32 maxTextureCount);
        
    public:
        void load_bank(const std::filesystem::path& bankFilepath);

    private:
        void load_texture_bank_options(const std::filesystem::path& filepath, const nlohmann::json& object);

    public:
        texture_asset_bank_options* get_texture_options(const std::filesystem::path& filepath);

    private:
        har_unordered_map<std::string, har_unique<bank_option>> mTextureBank;
    };
}