#include "asset_handler.hpp"

#include "core/defines.hpp"
#include "core/assets/asset.hpp"
#include "core/assert/assert.hpp"
#include "core/logging/logger.hpp"
#include "core/event/event_type.hpp"
#include "core/assets/pso/pso_loader.hpp"
#include "core/assets/scene/scene_loader.hpp"
#include "core/event/concrete/asset_events.hpp"
#include "core/assets/script/script_loader.hpp"
#include "core/assets/texture/texture_loader.hpp"
#include "core/assets/texture/texture_compress.hpp"

namespace har
{
    void asset_handler::initialize(asset_handler_context&& context, const asset_memory_options& options)
    {
        using namespace internal;
    
        rContext = std::move(context);
        HAR_ASS(rContext.eventQueue && rContext.workerPool && rContext.assetBank && rContext.scriptEngine && rContext.sceneHolder);

        mTextureCache.initialize(&MainStaticAllocator, options.maxTextureCount);
        mPipelineStatesCache.initialize(&MainStaticAllocator, options.maxPipelineStateCount);
        mScriptCache.initialize(&MainStaticAllocator, options.maxScriptCount);
    }

    void asset_handler::on_event(event& event)
    {
        auto* eventQueue = rContext.eventQueue;

        switch (event.type)
        {
            case event_type::texture_loaded:
            {
                texture_loaded_data textureData = event.get<texture_loaded_data>();
                if ((textureData.options & asset_upload_options::upload_immediate) == 0)
                    return;
                
                send_texture_upload_request_event(*eventQueue,
                { 
                    .handle = textureData.handle,
                    .freeHostMemory = textureData.freeHostMemory
                });
            } break;

            case event_type::texture_uploaded:
            {
                texture_uploaded_data data = event.get<texture_uploaded_data>();
                mTextureMap[data.handle] = data.combinedTextureHandle;
                if (data.freeHostMemory)
                    unload_texture_task(mTextureCache.get_asset(data.handle)->resource.data);
            } break;

            case event_type::pso_loaded:
            {
                pso_loaded_data psoData = event.get<pso_loaded_data>();
                send_pipeline_state_upload_request_event(*eventQueue, std::move(psoData));
            } break;

            case har::event_type::pso_uploaded:
            {
                pso_uploaded_data data = event.get<pso_uploaded_data>();
                mPipelineStateMap[data.handle] = data.pipeline;
                mPipelineStatesCache.remove_asset(mPipelineStatesCache.get_asset(data.handle)->filepath);
            } break;

            default:
                break;
        }
    }
}

namespace har
{
    asset_handle asset_handler::load_texture(const std::filesystem::path& filepath, const texture_load_options& loadOptions, asset_upload_options uploadOptions)
    {   
        // TODO: if a texture is not upload_imm, but has a free request, issue a warning.
        // TODO: if a texture has a free request, and a separate call to rhi_t::create_texture is done, warn or reload.

        asset_handle existingHandle = mPipelineStatesCache.get_asset_handle(filepath);
        if (existingHandle != invalid_small_handle)
        {
            HAR_LOG_ERROR("IN");
            return existingHandle;
        }

        asset_handle handle = mTextureCache.register_empty_asset();
        rContext.workerPool->insert_job([&, handle, filepath, loadOptions, uploadOptions]()
        {
            auto& assetBank = rContext.assetBank; 
            auto* eventQueue = rContext.eventQueue; 
            
            auto textureResult = load_texture_task(filepath, loadOptions);
            if (!textureResult.has_value())
            {
                HAR_LOG_ERROR("[Asset] Failed to load texture: {} | Code: {}", 
                    filepath.string(), 
                    get_asset_load_error_string(textureResult.error())
                );
                return;
            }

            auto textureAsset = textureResult.value();

            // Compression:
            auto* bankOptions = assetBank->get_texture_options(filepath);
            if (bankOptions->compression != texture_compression::none)
            {
                textureAsset.compressionLevel = bankOptions->compression; 
                compress_image_task(textureAsset);
            }

            mTextureCache.update_asset(handle, filepath, textureAsset);
            
            send_texture_loaded_event(*eventQueue,
            { 
                .handle = handle,
                .options = uploadOptions,
                .freeHostMemory = loadOptions.freeHostMemory
            });
        });

        return handle;
    }
    
    void asset_handler::unload_texture(asset_handle handle)
    {
        auto* asset = mTextureCache.get_asset(handle);
            
        unload_texture_task(asset->resource.data);

        if (mTextureMap.contains(handle))
            mTextureMap.erase(handle);

        mTextureCache.remove_asset(asset->filepath);
    }

    void asset_handler::unload_texture(const std::filesystem::path& filepath)
    {
        auto* asset = mTextureCache.get_asset(filepath);
        asset_handle handle = mTextureCache.get_asset_handle(filepath);

        unload_texture_task(asset->resource.data);
            
        if (mTextureMap.contains(handle))
            mTextureMap.erase(handle);
            
        mTextureCache.remove_asset(filepath);
    }
}

namespace har
{
    asset_handle asset_handler::load_pipeline_state(const std::filesystem::path& filepath)
    {
        asset_handle existingHandle = mPipelineStatesCache.get_asset_handle(filepath);
        if (existingHandle != invalid_small_handle)
            return existingHandle;

        asset_handle handle = mPipelineStatesCache.register_empty_asset();
        rContext.workerPool->insert_job([&, handle, filepath]()
        {
            auto* eventQueue = rContext.eventQueue;

            auto result = load_pso_task(filepath);
            if (!result.has_value())
            {
                HAR_LOG_ERROR("[Asset] Failed to load pipeline state object: {} | Code: {}", 
                    filepath.string(), 
                    get_asset_load_error_string(result.error())
                );
                return;
            }

            mPipelineStatesCache.update_asset(handle, filepath, result.value());

            send_pipeline_state_loaded_event(*eventQueue, 
            {
                .handle = handle,
            });
        });

        return handle;
    }
}

namespace har
{
    asset_handle asset_handler::load_script(const std::filesystem::path& filepath)
    {
        asset_handle existingHandle = mScriptCache.get_asset_handle(filepath);
        if (existingHandle != invalid_small_handle)
            return existingHandle;

        asset_handle handle = mScriptCache.register_empty_asset();
        if (handle == invalid_small_handle)
            HAR_LOG_CRITICAL("No space");

        auto result = load_script_task(filepath, rContext.scriptEngine);
        if (!result.has_value())
        {
            HAR_LOG_ERROR
            ("[Asset] Failed to load script: {} | Code: {}", 
                filepath.string(), 
                get_asset_load_error_string(result.error())
            );
            return invalid_small_handle;
        }

        auto script = result.value();

        mScriptCache.update_asset(handle, filepath, script);

        return handle;
    }
}

namespace har
{
    scene_handle asset_handler::load_scene(const std::filesystem::path& filepath)
    {
        // TODO: better cache with filepath

        auto& scenes = rContext.sceneHolder;
        scene_handle sceneHandle = scenes->create_scene(scene_options::none);

        rContext.workerPool->insert_job([&, sceneHandle, filepath]()
        {
            auto sceneData = load_scene_task(filepath, *scenes->get_scene(sceneHandle));
            for (const auto& entry : sceneData.entries)
            {
                switch (entry.type)
                {
                    // TODO: send_request_texture_load() type functions. This is not the right thread to do this

                    case asset_type::pso:
                        //load_pipeline_state(entry.filepath);
                        break;

                    case asset_type::scene:
                        //load_scene(entry.filepath);
                        break;

                    case asset_type::script:
                    {
                        load_script(entry.filepath);
                    } break;
                        //load_script(entry.filepath);
                        

                    case asset_type::texture:
                        //load_texture(entry.filepath, {}, asset_upload_options::upload_immediate);
                        break;

                    default:
                    {
                        HAR_LOG_CRITICAL("[Asset Handler] Attempted to load scene entry with invalid asset type.");
                    } break;
                }
            }
            
            mSceneMap[filepath.string()] = sceneHandle;

            send_scene_loaded_event(*rContext.eventQueue, { .handle = sceneHandle });
        });

        return sceneHandle;
    }
}