#pragma once
#include "core/assets/asset.hpp"
#include "core/assets/asset_type.hpp"
#include "core/containers/containers.hpp"
#include "core/defines.hpp"
#include "core/assets/asset_hub.hpp"
#include "core/logging/logger.hpp"
#include "core/scene/scene_concepts.hpp"
#include "core/scene/scene_holder.hpp"
#include "core/utils/uuid.hpp"

namespace har
{
    class asset_linker
    {
        using asset_loader_function   = std::function<void(uuid_t, asset_handle)>;
        using asset_uploader_function = std::function<void(asset_handle, handle_t)>;

    public:
        void initialize(NON_OWNING asset_hub* assetHub, NON_OWNING scene_holder* sceneHolder);
        void on_event(event& event);

    private:
        void handle_material_instance_asset_upload(asset_handle assetHandle, handle_t gpuHandle);
        void handle_pso_asset_upload(asset_handle assetHandle, handle_t gpuHandle);
    
    private:
        template <is_component_linkable_through_load Component>
        void register_asset_loader(asset_type type)
        {
            mLoaders[type] = [this](uuid_t uuid, asset_handle handle)
            {
                this->handle_asset_load<Component>(uuid, handle);
            };
        }

        template <is_component_linkable_through_upload Component>
        void register_asset_uploader(asset_type type)
        {
            mUploaders[type] = [this](asset_handle assetHandle, handle_t gpuObjectHandle)
            {
                this->handle_asset_upload<Component>(assetHandle, gpuObjectHandle);
            };
        }

        template <is_component_linkable_through_load Component>
        void handle_asset_load(uuid_t eventUUID, asset_handle handle)
        {
            if (!rSceneHolder->get_current_scene())
            {
                HAR_LOG_ERROR("Attempted to link asset handles from uuid too early.")
                return;
            }

            auto view = rSceneHolder->get_current_scene()->get_registry().view<Component>();
            for (auto entity : view)
            {
                auto& component = view.template get<Component>(entity);
                if (component.assetUUID == eventUUID)
                    component.assetHandle = handle;
            }
        }

        template <is_component_linkable_through_upload Component>
        void handle_asset_upload(asset_handle assetHandle, handle_t gpuObjectHandle)
        {
            if (!rSceneHolder->get_current_scene())
            {
                HAR_LOG_ERROR("Attempted to link gpu handles from asset handles too early.")
                return;
            }

            auto view = rSceneHolder->get_current_scene()->get_registry().view<Component>();
            for (auto entity : view)
            {
                auto& renderable = view.template get<Component>(entity);
                if (renderable.assetHandle == assetHandle)
                    renderable.gpuObjectHandle = gpuObjectHandle;
            }
        }

    private:
        NON_OWNING asset_hub* rAssetHub = nullptr;
        NON_OWNING scene_holder* rSceneHolder = nullptr;

    private:
        har_unordered_map<asset_type, asset_loader_function>   mLoaders;
        har_unordered_map<asset_type, asset_uploader_function> mUploaders;
    };
}