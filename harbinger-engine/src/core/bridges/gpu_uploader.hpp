#pragma once
#include <filesystem>
#include <functional>

#include "core/defines.hpp"
#include "core/assets/asset.hpp"
#include "core/config/config.hpp"
#include "core/assets/asset_hub.hpp"
#include "core/assets/asset_type.hpp"
#include "core/event/event_queue.hpp"
#include "core/containers/containers.hpp"
#include "core/assets/asset_bank_option.hpp"


#include "rhi/rhi.hpp"
#include "rhi/glsl_compiler.hpp"
#include "rhi/common/objects.hpp"


namespace har
{
    struct gpu_uploader_context
    {
        NON_OWNING asset_hub*     assetHub     = nullptr;
        NON_OWNING event_queue*   eventQueue   = nullptr;
        NON_OWNING config_t*      config       = nullptr;
        NON_OWNING rhi_t*         rhi          = nullptr;
    };

    using gpu_upload_function = std::function<handle_t(asset*, asset_bank_option*, const std::filesystem::path)>;

    class gpu_uploader
    {
    public:
        void initialize(gpu_uploader_context&& context);

    public:
        void upload_asset_from_hub(asset_type type, const std::filesystem::path& filepath);

    private:
        shader_handle create_shader(shader_type type, const std::filesystem::path& shaderFilepath);

    private:
        void register_gpu_upload_function(asset_type type, gpu_upload_function&& function);

    private:
        command_buffer_t mTransferBuffer;
        gpu_uploader_context rContext;
        glsl_compiler mCompiler;
        har_unordered_map<asset_type, gpu_upload_function> mUploadCalls;
    };
}