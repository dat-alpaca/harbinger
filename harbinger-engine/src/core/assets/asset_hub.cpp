#include "asset_hub.hpp"

#include "core/assets/asset_type.hpp"
#include "core/assets/material_instance/material_instance_asset.hpp"
#include "core/assets/material_instance/material_instance_loader.hpp"
#include "core/assets/mesh/mesh_asset.hpp"
#include "core/assets/mesh/mesh_loader.hpp"
#include "core/assets/pso/pso_asset.hpp"
#include "core/assets/pso/pso_loader.hpp"
#include "core/assets/scene/scene_asset.hpp"
#include "core/assets/scene/scene_loader.hpp"
#include "core/assets/script/script_asset.hpp"
#include "core/assets/texture/texture_asset.hpp"
#include "core/defines.hpp"

#include "core/event/concrete/asset_events.hpp"

#include "core/event/concrete/scene_events.hpp"
#include "core/logging/logger.hpp"
#include "core/assert/assert.hpp"

#include "core/memory/memory_tag.hpp"
#include "core/memory/allocators.hpp"
#include "core/utils/uuid.hpp"
#include "core/worker/worker_pool.hpp"

#include "core/serializers/asset_bank_serializer.hpp"

#include "core/assets/asset.hpp"
#include "core/assets/texture/texture_loader.hpp"
#include "core/assets/texture/texture_bank_options.hpp"
#include <cstring>
#include <memory>

namespace har
{
    using namespace har::internal;

    void asset_hub::initialize(asset_hub_context&& context, const asset_hub_desc& desc, const std::filesystem::path& assetBankFilepath)
    {
        HAR_ASS(context.workerPool);
        HAR_ASS(context.eventQueue);
        HAR_ASS(context.sceneHolder);
        mContext = std::move(context);

        // Asset free list:
        auto* staticAllocator = &internal::MainStaticAllocator;
        {
            byte* backingBuffer = staticAllocator->push
            (
                mAssets.required_memory(desc.maxAssets),
                1
            );
            mAssets.initialize(backingBuffer, desc.maxAssets);
        }

        deserialize_asset_bank(assetBankFilepath, mAssetBank);

        // Texture:
        register_asset_loader(asset_type::texture, [](const std::filesystem::path& filepath, asset* assetData, NON_OWNING asset_bank_option* bankOption) 
        {
            auto textureAssetResult = asset_texture_load(filepath);
            if (!textureAssetResult.has_value())
            {
                HAR_LOG_ERROR("Failed to load texture at: {}", filepath.string());
                return;
            }

            const texture_asset_bank_options& options = *static_cast<texture_asset_bank_options*>(bankOption);
            asset_texture_compress(textureAssetResult.value(), options.compression);
            
            auto* textureAsset = static_cast<texture_asset*>(assetData);
            {
                textureAsset->data = textureAssetResult->data;
                textureAsset->width = textureAssetResult->width;
                textureAsset->height = textureAssetResult->height;
                textureAsset->channels = textureAssetResult->channels;
            }
        });
        register_asset_unloader(asset_type::texture, [](asset* assetData)
        {
            auto* asset = static_cast<texture_asset*>(assetData);
            asset_texture_free(asset->data);
            std::destroy_at(asset);
        });

        // TODO: write script loader
        // TODO: register unloader
        // Script:
        register_asset_loader(asset_type::script, [&](const std::filesystem::path& filepath, asset* assetData, NON_OWNING asset_bank_option* _) 
        {
            
        });

        // TODO: register unloader
        // Scene:
        register_asset_loader(asset_type::scene, [&](const std::filesystem::path& filepath, asset* assetData, NON_OWNING asset_bank_option* _) 
        {
            auto result = asset_scene_load(filepath, *mContext.sceneHolder, static_cast<scene_asset*>(assetData));
            if (!result.has_value())
            {
                HAR_LOG_ERROR("Failed to load scene at: {}", filepath.string());
                return;
            }
        });

        // TODO: register unloader
        // PSO:
        register_asset_loader(asset_type::pipeline_object, [&](const std::filesystem::path& filepath, asset* assetData, NON_OWNING asset_bank_option* _) 
        {
            auto result = asset_pso_load(filepath);
            if (!result.has_value())
            {
                HAR_LOG_ERROR("Failed to load PSO at: {}", filepath.string());
                return;
            }

            auto* psoAsset = static_cast<pso_asset*>(assetData);
            {
                psoAsset->rasterizer            = result->rasterizer;
                psoAsset->vertexShaderPath      = result->vertexShaderPath;
                psoAsset->fragmentShaderPath    = result->fragmentShaderPath;
                psoAsset->multisampling         = result->multisampling;
                psoAsset->inputAssembly         = result->inputAssembly;
            }
        });

        // TODO: register unloader
        // Material instance:
        register_asset_loader(asset_type::material_instance, [&](const std::filesystem::path& filepath, asset* assetData, NON_OWNING asset_bank_option* _)
        {
            auto result = asset_material_instance_load(filepath);
            if (!result.has_value())
            {
                HAR_LOG_ERROR("Failed to load material instance at: {}", filepath.string());
                return;
            }

            auto* materialInstanceAsset = static_cast<material_instance_asset*>(assetData);
            {
                materialInstanceAsset->type = result->type;

                materialInstanceAsset->data.resize(result->data.size());
                std::memcpy(materialInstanceAsset->data.data(), result->data.data(), result->data.size());
            }
        });

        // TODO: register unloader
        register_asset_loader(asset_type::mesh, [&](const std::filesystem::path& filepath, asset* assetData, NON_OWNING asset_bank_option* _)
        {
            auto result = asset_mesh_load(filepath);
            if (!result.has_value())
            {
                HAR_LOG_ERROR("Failed to load mesh at: {}", filepath.string());
                return;
            }

            auto* meshAsset = static_cast<mesh_asset*>(assetData);
            {
                meshAsset->vertices.resize(result->vertices.size());
                meshAsset->indices.resize(result->indices.size());

                std::memcpy(meshAsset->vertices.data(), result->vertices.data(), result->vertices.size());
                std::memcpy(meshAsset->indices.data(), result->indices.data(), result->indices.size());
                meshAsset->offsets = result->offsets;
                meshAsset->vertexCount = result->vertexCount;
            }
        });
    }   

    asset_handle asset_hub::load_asset(asset_type type, const std::filesystem::path& filepath, asset_loader_callback&& callback, bool inPlace, uuid_t assetUUID)
    {
        if (!filepath.string().starts_with(HAR_ENGINE_INTERNAL_URI) && !std::filesystem::exists(filepath))
        {
            HAR_LOG_ERROR("Invalid filepath: {}", filepath.string());
            return invalid_small_handle;
        }

        if (mCache.find(filepath) != mCache.end())
            return mCache[filepath];
        
        asset_handle handle = mAssets.insert(allocate_appropriate_asset_type(type, filepath, assetUUID));
        HAR_ASS(handle != invalid_small_handle);

        if (!inPlace)
            issue_loader_call(handle, type, filepath, std::move(callback));
        else
            issue_loader_call_in_place(handle, type, filepath, std::move(callback));

        return handle;
    }

    void asset_hub::unload_asset(asset_type type, const std::filesystem::path& filepath, bool inPlace)
    {
        if (!std::filesystem::exists(filepath))
        {
            HAR_LOG_ERROR("Invalid filepath: {}", filepath.string());
            return;
        }

        if (mCache.find(filepath) == mCache.end())
        {
            HAR_LOG_ERROR("The asset at `{}` is not loaded into the asset hub.", filepath.string());
            return;
        }
        
        asset_handle handle = mCache[filepath];
        mCache.erase(filepath);

        asset* asset = mAssets[handle];

        if (inPlace)
        {
            mUnloaders[type](asset);
            mAssets.remove(handle);
        }
        else
        {
            mContext.workerPool->insert_job([this, type, asset, handle]()
            {
                mUnloaders[type](asset);
                mAssets.remove(handle);
            });
        }
    }

    void asset_hub::unload_asset(asset_type type, asset_handle handle, bool inPlace)
    {
        if (handle == invalid_small_handle)
        {
            HAR_LOG_ERROR("Invalid handle: {}", handle);
            return;
        }

        for (auto it = mCache.begin(); it != mCache.end();)
        {
            if (it->second != handle)
            {
                ++it;
                continue;
            }
            
            mCache.erase(it++);
        }

        asset* asset = mAssets[handle];
        if (inPlace)
        {
            mUnloaders[type](asset);
            mAssets.remove(handle);
        }
        else
        {
            mContext.workerPool->insert_job([this, type, asset, handle]()
            {
                mUnloaders[type](asset);
                mAssets.remove(handle);
            });
        }
    }

    asset* asset_hub::get_asset(const std::filesystem::path& filepath)
    {
        if (mCache.find(filepath) != mCache.end())
            return mAssets[mCache[filepath]];

        return nullptr;
    }
    asset* asset_hub::get_asset(asset_handle handle)
    {
        return mAssets[handle];
    }

    void asset_hub::register_asset_loader(asset_type type, asset_loader_function&& function)
    {
        mLoaders[type] = std::move(function);
    }
    void asset_hub::register_asset_unloader(asset_type type, asset_unloader_function&& function)
    {
        mUnloaders[type] = std::move(function);
    }

    asset* asset_hub::allocate_appropriate_asset_type(asset_type type, const std::filesystem::path& filepath, uuid_t assetUUID)
    {
        using namespace har::internal;
        
        asset* assetData = nullptr;
        switch (type)
        {
            case asset_type::unknown:
            {
                HAR_LOG_CRITICAL("Attempted to allocate an unknown or invalid asset type.");
            } break;

            case asset_type::pipeline_object:
            {
                assetData = static_cast<asset*>(MainDynamicAllocator.allocate(memory_tag::assets, sizeof(pso_asset)));
                std::construct_at(static_cast<pso_asset*>(assetData));
            } break;
                
            case asset_type::texture:
            {
                assetData = static_cast<asset*>(MainDynamicAllocator.allocate(memory_tag::assets, sizeof(texture_asset)));
                std::construct_at(static_cast<texture_asset*>(assetData));
            } break;

            case asset_type::scene:
            {
                assetData = static_cast<asset*>(MainDynamicAllocator.allocate(memory_tag::assets, sizeof(scene_asset)));
                std::construct_at(static_cast<scene_asset*>(assetData));
            } break;

            case asset_type::script:
            {
                assetData = static_cast<asset*>(MainDynamicAllocator.allocate(memory_tag::assets, sizeof(script_asset)));
                std::construct_at(static_cast<script_asset*>(assetData));
            } break;

            case asset_type::material_instance:
            {
                assetData = static_cast<asset*>(MainDynamicAllocator.allocate(memory_tag::assets, sizeof(material_instance_asset)));
                std::construct_at(static_cast<material_instance_asset*>(assetData));
            } break;

            case asset_type::mesh:
            {
                assetData = static_cast<asset*>(MainDynamicAllocator.allocate(memory_tag::assets, sizeof(mesh_asset)));
                std::construct_at(static_cast<mesh_asset*>(assetData));
            } break;
        }

        if (assetUUID != invalid_uuid)
            assetData->metadata.assetUUID = assetUUID;
        else
            assetData->metadata.assetUUID = generate_uuid();
        
        assetData->metadata.filepath = filepath;
        assetData->type = type;
        return assetData;
    }

    void asset_hub::issue_loader_call(asset_handle handle, asset_type type, const std::filesystem::path& filepath, asset_loader_callback&& callback)
    {
        mContext.workerPool->insert_job([this, filepath, handle, type, callback = std::move(callback)]() mutable
        {
            issue_loader_call_in_place(handle, type, filepath, std::move(callback));
        });
    }

    void asset_hub::issue_loader_call_in_place(asset_handle handle, asset_type type, const std::filesystem::path& filepath, asset_loader_callback&& callback)
    {
        mLoaders[type](filepath, mAssets[handle], mAssetBank[filepath].get());
        mAssets[handle]->status = asset_status::loaded;

        send_asset_loaded_event(*mContext.eventQueue, { handle, type });

        if (callback)
            callback(handle);
    }
}