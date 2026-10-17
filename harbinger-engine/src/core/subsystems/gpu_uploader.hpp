#pragma once
#include "rhi/rhi.hpp"
#include "core/defines.hpp"
#include "core/event/event.hpp"
#include "core/assets/asset.hpp"
#include "rhi/common/objects.hpp"
#include <filesystem>

namespace har
{
    class harbinger_engine;

    class gpu_uploader
    {
    public:
        void initialize(NON_OWNING harbinger_engine* engine);

    public:
        void on_event(event& event);

    private:
        combined_texture_handle create_and_upload_texture(asset_handle assetHandle);
        
        graphics_pipeline_handle create_pipeline_state(asset_handle assetHandle);
        shader_handle create_shader(shader_type type, const std::filesystem::path& shaderFilepath);

    private:
        NON_OWNING harbinger_engine* rEngine = nullptr;
        command_buffer_t mTransferBuffer;
    };
}