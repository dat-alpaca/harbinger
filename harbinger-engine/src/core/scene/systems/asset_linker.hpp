#pragma once
#include "core/assets/asset.hpp"
#include "core/assets/asset_type.hpp"
#include "core/containers/containers.hpp"
#include "core/defines.hpp"
#include "core/assets/asset_hub.hpp"
#include "core/event/concrete/asset_events.hpp"
#include "core/event/event_queue.hpp"
#include "core/logging/logger.hpp"
#include "core/scene/scene_concepts.hpp"
#include "core/scene/scene_holder.hpp"
#include "core/utils/uuid.hpp"
#include "rhi/common/objects.hpp"

namespace har
{
    struct asset_linker_context
    {
        asset_hub* assetHub;
        scene_holder* sceneHolder;
        event_queue* eventQueue;
    };

    class asset_linker
    {
        using asset_loader_function   = std::function<void(uuid_t, asset_handle)>;
        using asset_uploader_function = std::function<void(asset_handle, handle_t)>;

    public:
        void initialize(asset_linker_context&& context);
        void on_event(event& event);

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

    private:
        template <is_component_linkable_through_load Component>
        void handle_asset_load(uuid_t eventUUID, asset_handle handle)
        {
            auto* sceneHolder = mContext.sceneHolder;
            if (!sceneHolder->get_current_scene())
            {
                HAR_LOG_ERROR("Attempted to link asset handles from uuid too early.")
                return;
            }

            auto view = sceneHolder->get_current_scene()->get_registry().view<Component>();
            for (auto entity : view)
            {
                auto& component = view.template get<Component>(entity);
                if (component.assetUUID == eventUUID)
                    component.assetHandle = handle;
            }

            send_asset_linked_after_load_event(*mContext.eventQueue);
        }

        template <is_component_linkable_through_upload Component>
        void handle_asset_upload(asset_handle assetHandle, handle_t gpuObjectHandle)
        {
            auto* sceneHolder = mContext.sceneHolder;
            if (!sceneHolder->get_current_scene())
            {
                HAR_LOG_ERROR("Attempted to link gpu handles from asset handles too early. No scene is set.");
                return;
            }

            auto view = sceneHolder->get_current_scene()->get_registry().view<Component>();
            for (auto entity : view)
            {
                auto& renderable = view.template get<Component>(entity);
                if (renderable.assetHandle == assetHandle)
                    renderable.gpuObjectHandle = gpuObjectHandle;
            }

            send_asset_linked_after_upload_event(*mContext.eventQueue);
        }

    public:
        combined_texture_handle fetch_texture_from_uuid(uuid_t uuid) const { return mTextureMap.at(uuid); }

    public:
        asset_hub* get_asset_hub() const { return mContext.assetHub; }

    private:
        asset_linker_context mContext;
        har_unordered_map<uuid_t, combined_texture_handle> mTextureMap;

    private:
        har_unordered_map<asset_type, asset_loader_function>   mLoaders;
        har_unordered_map<asset_type, asset_uploader_function> mUploaders;
    };
}