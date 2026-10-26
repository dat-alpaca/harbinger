#include "gpu_uploader.hpp"
#include "core/assert/assert.hpp"
#include "core/assets/asset.hpp"
#include "core/assets/asset_bank_option.hpp"
#include "core/assets/asset_hub.hpp"
#include "core/assets/asset_type.hpp"
#include "core/assets/pso/pso_asset.hpp"
#include "core/assets/texture/texture_asset.hpp"
#include "core/assets/texture/texture_bank_options.hpp"

#include "core/defines.hpp"
#include "core/logging/logger.hpp"
#include "core/utils/file.hpp"
#include "rhi/common/objects.hpp"
#include <filesystem>

namespace har
{
    void gpu_uploader::initialize(gpu_uploader_context&& context)
    {
        rContext = std::move(context);
        HAR_ASS(rContext.assetHub && rContext.eventQueue && rContext.rhi && rContext.config);
        
        mTransferBuffer = rContext.rhi->create_command_buffer();

        register_gpu_upload_function(asset_type::texture, [&](asset* asset, asset_bank_option* option, const std::filesystem::path& filepath) -> handle_t
        {
            auto* textureAsset = static_cast<texture_asset*>(asset);
            const auto& options = *static_cast<texture_asset_bank_options*>(option);

            texture_handle texture = rContext.rhi->create_texture(options.gpuOptions);
            sampler_handle sampler = rContext.rhi->create_sampler(options.samplerOptions);
            
            {
                mTransferBuffer.begin();
                mTransferBuffer.update_texture(texture, *textureAsset, options.dataFormat, options.dataType);
                mTransferBuffer.end();
                rContext.rhi->execute(mTransferBuffer);
            }

            return rContext.rhi->create_combined_texture({ .textureHandle = texture, .samplerHandle = sampler });
        });

        register_gpu_upload_function(asset_type::pipeline_object, [&](asset* asset, asset_bank_option* _, const std::filesystem::path& __) -> handle_t
        {
            auto* psoAsset = static_cast<pso_asset*>(asset);

            auto* rhi = rContext.rhi;

            // Shaders:
            shader_handle vertexShader = create_shader(shader_type::vertex, psoAsset->vertexShaderPath);
            shader_handle fragmentShader = create_shader(shader_type::fragment, psoAsset->fragmentShaderPath);

            // Pipeline:
            graphics_pipeline_handle pipeline = rhi->create_graphics_pipeline(
            {
                .rasterizer     = psoAsset->rasterizer,
                .vertexShader   = vertexShader,
                .fragmentShader = fragmentShader,
                .multisampling  = psoAsset->multisampling,
                .inputAssembly  = psoAsset->inputAssembly
            });

            return pipeline;
        });
    }
        
    void gpu_uploader::upload_asset_from_hub(asset_type type, const std::filesystem::path& filepath)
    {
        auto* assetData = static_cast<asset*>(rContext.assetHub->get_asset(filepath));
        auto* bankOptions = static_cast<asset_bank_option*>(rContext.assetHub->get_asset_bank()[filepath].get());

        if (!is_gpu_uploadable(type))
        {
            HAR_LOG_ERROR("Attempted to upload a non-gpu uploadable asset: {} [{}]", filepath.string(), get_name_from_asset(type));
            return;
        }

        if (!std::filesystem::exists(filepath))
        {
            HAR_LOG_ERROR("Failed to upload asset at: {}", filepath.string());
            return;
        }

        mUploadCalls[type](assetData, bankOptions, filepath);
    }

    shader_handle gpu_uploader::create_shader(shader_type type, const std::filesystem::path& shaderFilepath)
    {
        auto& config = *rContext.config;
        auto* rhi = rContext.rhi;

        auto vertexText = read_file_contents(shaderFilepath);
        auto vertexResult = mCompiler.compile_shader(type, vertexText, config);

        if (!vertexResult.has_value())
        {
            HAR_LOG_ERROR("[GPU Uploader] Failed to load and compile vertex shader: {} | Code: {}", 
                shaderFilepath.string(), 
                get_compiler_error_string(vertexResult.error())
            );
            return invalid_small_handle;
        }

        return rhi->create_shader(type, vertexResult.value());
    }

    void gpu_uploader::register_gpu_upload_function(asset_type type, gpu_upload_function&& function)
    {
        mUploadCalls[type] = std::move(function);
    }
}