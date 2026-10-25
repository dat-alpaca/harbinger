#pragma once
#include <filesystem>

#include "core/bridges/asset_handler.hpp"
#include "core/assets/asset_bank.hpp"
#include "core/config/config.hpp"
#include "core/event/event_queue.hpp"
#include "rhi/glsl_compiler.hpp"
#include "rhi/rhi.hpp"

#include "core/defines.hpp"
#include "core/event/event.hpp"
#include "core/assets/asset.hpp"
#include "rhi/common/objects.hpp"

namespace har
{
    struct gpu_uploader_context
    {
        NON_OWNING asset_handler* assetHandler = nullptr;
        NON_OWNING event_queue*   eventQueue   = nullptr;
        NON_OWNING asset_bank*    assetBank    = nullptr;
        NON_OWNING config_t*      config       = nullptr;
        NON_OWNING rhi_t*         rhi          = nullptr;
    };

    class gpu_uploader
    {
    public:
        void initialize(gpu_uploader_context&& context);

    public:
        void on_event(event& event);

    private:
        combined_texture_handle create_and_upload_texture(asset_handle assetHandle);
        
        graphics_pipeline_handle create_pipeline_state(asset_handle assetHandle);
        shader_handle create_shader(shader_type type, const std::filesystem::path& shaderFilepath);

    private:
        command_buffer_t mTransferBuffer;
        gpu_uploader_context rContext;
        glsl_compiler mCompiler;
    };
}