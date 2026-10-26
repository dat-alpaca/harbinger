#include "asset_linker.hpp"

#include "core/assert/assert.hpp"
#include "core/assets/asset_type.hpp"
#include "core/event/concrete/scene_events.hpp"
#include "core/event/event_type.hpp"
#include "core/scene/renderer_component.hpp"
#include "core/event/concrete/asset_events.hpp"

namespace har
{
    void asset_linker::initialize(NON_OWNING asset_hub* assetHub, NON_OWNING scene_holder* sceneHolder)
    {
        HAR_ASS(assetHub);
        HAR_ASS(sceneHolder);

        rAssetHub = assetHub;
        rSceneHolder = sceneHolder;
    }

    void asset_linker::on_event(event& event)
    {
        if (event.type == event_type::asset_loaded)
        {
            auto eventData = event.get<asset_loaded_event_data>();
            const auto& eventUUID = rAssetHub->get_asset(eventData.handle)->metadata.assetUUID;

            if (eventData.type == asset_type::material_instance)
                handle_material_instance_asset_load(eventUUID, eventData.handle);

            else if (eventData.type == asset_type::pipeline_object)
                handle_pso_asset_load(eventUUID, eventData.handle);
        }
        else if (event.type == event_type::asset_uploaded)
        {
            auto eventData = event.get<asset_uploaded_event_data>();

            if (eventData.type == asset_type::material_instance)
                handle_material_instance_asset_upload(eventData.assetHandle, eventData.gpuHandle);

            else if (eventData.type == asset_type::pipeline_object)
                handle_pso_asset_upload(eventData.assetHandle, eventData.gpuHandle);
        }
    }

    void asset_linker::handle_material_instance_asset_load(uuid eventUUID, asset_handle handle)
    {
        if (!rSceneHolder->get_current_scene())
            return;

        auto view = rSceneHolder->get_current_scene()->get_registry().view<material_instance_component>();
        for (auto entity : view)
        {
            auto& materialInstance = view.get<material_instance_component>(entity);
            if (materialInstance.assetUUID == eventUUID)
                materialInstance.assetHandle = handle;
        }
    }
    void asset_linker::handle_material_instance_asset_upload(asset_handle assetHandle, handle_t gpuHandle)
    {
        if (!rSceneHolder->get_current_scene())
            return;

        auto view = rSceneHolder->get_current_scene()->get_registry().view<material_instance_component>();
        for (auto entity : view)
        {
            auto& materialInstance = view.get<material_instance_component>(entity);
            if (materialInstance.assetHandle == assetHandle)
                materialInstance.materialHandle = gpuHandle;
        }
    }

    void asset_linker::handle_pso_asset_load(uuid eventUUID, asset_handle handle)
    {
        if (!rSceneHolder->get_current_scene())
            return;

        auto view = rSceneHolder->get_current_scene()->get_registry().view<renderable_component>();
        for (auto entity : view)
        {
            auto& renderable = view.get<renderable_component>(entity);
            if (renderable.pipelineAssetUUID == eventUUID)
                renderable.pipelineAssetHandle = handle;
        }
    }
    void asset_linker::handle_pso_asset_upload(asset_handle assetHandle, handle_t gpuHandle)
    {
        if (!rSceneHolder->get_current_scene())
            return;

        auto view = rSceneHolder->get_current_scene()->get_registry().view<renderable_component>();
        for (auto entity : view)
        {
            auto& renderable = view.get<renderable_component>(entity);
            if (renderable.pipelineAssetHandle == assetHandle)
                renderable.pipelineHandle = gpuHandle;
        }
    }
}