#include "gpu_uploader.hpp"
#include "core/assets/asset.hpp"
#include "core/assets/pso/pso_asset.hpp"
#include "core/defines.hpp"
#include "core/event/concrete/asset_events.hpp"
#include "core/utils/file.hpp"
#include "core/engine/engine.hpp"
#include "rhi/common/objects.hpp"

namespace har
{
    void gpu_uploader::initialize(NON_OWNING harbinger_engine* engine)
    {
        rEngine = engine;
        mTransferBuffer = rEngine->rhi.create_command_buffer();
    }
        
    void gpu_uploader::on_event(event& event)
    {
        switch (event.type)
        {
            case event_type::request_texture_upload:
            {
                texture_upload_request_data uploadData = event.get<texture_upload_request_data>();
                combined_texture_handle combinedHandle = create_and_upload_texture(uploadData.handle);
    
                send_texture_uploaded_event(*rEngine->eventQueue, 
                { 
                    .handle = uploadData.handle,
                    .combinedTextureHandle = combinedHandle,
                    .freeHostMemory = uploadData.freeHostMemory
                });
            } break;

            case event_type::request_pso_upload:
            {
                pso_loaded_data uploadData = event.get<pso_loaded_data>();
                graphics_pipeline_handle pipeline = create_pipeline_state(uploadData.handle);

                send_pipeline_uploaded_event(*rEngine->eventQueue, 
                { 
                    .handle = uploadData.handle,
                    .pipeline = pipeline,
                });
            } break;

            default:
                break;
        }
    }

    combined_texture_handle gpu_uploader::create_and_upload_texture(asset_handle assetHandle)
    {
        auto& assetHandler = rEngine->assetHandler;
        auto& assetBank = rEngine->assetBank;
        auto& rhi = rEngine->rhi;

        auto* textureAsset = assetHandler.get_textures().get_asset(assetHandle);
        auto* options = assetBank.get_texture_options(textureAsset->filepath);

        texture_handle texture = rhi.create_texture(options->gpuOptions);
        sampler_handle sampler = rhi.create_sampler(options->samplerOptions);
        
        {
            mTransferBuffer.begin();
            mTransferBuffer.update_texture(texture, textureAsset->resource, options->dataFormat, options->dataType);
            mTransferBuffer.end();
            rhi.execute(mTransferBuffer);
        }

        return rhi.create_combined_texture({ .textureHandle = texture, .samplerHandle = sampler });
    }

    graphics_pipeline_handle gpu_uploader::create_pipeline_state(asset_handle assetHandle)
    {
        auto& rhi = rEngine->rhi;

        auto* psoAsset = rEngine->assetHandler.get_pipeline_states().get_asset(assetHandle);
        auto& pso = psoAsset->resource;

        // Shaders:
        shader_handle vertexShader = create_shader(shader_type::vertex, pso.vertexShaderPath);
        shader_handle fragmentShader = create_shader(shader_type::fragment, pso.fragmentShaderPath);

        // Pipeline:
        graphics_pipeline_handle pipeline = rhi.create_graphics_pipeline(
        {
            .rasterizer = pso.rasterizer,
            .vertexShader = vertexShader,
            .fragmentShader = fragmentShader,
            .multisampling = pso.multisampling,
            .inputAssembly = pso.inputAssembly
        });

        return pipeline;
    }

    shader_handle gpu_uploader::create_shader(shader_type type, const std::filesystem::path& shaderFilepath)
    {
        auto& compiler = rEngine->glslCompiler;
        auto& config = rEngine->config;
        auto& rhi = rEngine->rhi;

        auto vertexText = read_file_contents(shaderFilepath);
        auto vertexResult = compiler.compile_shader(type, vertexText, config);

        if (!vertexResult.has_value())
        {
            HAR_LOG_ERROR("[GPU Uploader] Failed to load and compile vertex shader: {} | Code: {}", 
                shaderFilepath.string(), 
                get_compiler_error_string(vertexResult.error())
            );
            return invalid_small_handle;
        }

        return rhi.create_shader(shader_type::vertex, vertexResult.value());
    }
}