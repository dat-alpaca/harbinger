#include "asset_linker.hpp"

#include "core/assert/assert.hpp"
#include "core/assets/asset_type.hpp"
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

        register_asset_loader<material_instance_component>(asset_type::material_instance);
        register_asset_loader<renderable_component>(asset_type::pipeline_object);
        register_asset_loader<mesh_component>(asset_type::mesh);

        register_asset_uploader<material_instance_component>(asset_type::material_instance);
        register_asset_uploader<renderable_component>(asset_type::pipeline_object);
    }

    void asset_linker::on_event(event& event)
    {
        switch (event.type)
        {
            case event_type::asset_loaded:
            {
                auto eventData = event.get<asset_loaded_event_data>();

                const auto& eventUUID = rAssetHub->get_asset(eventData.handle)->metadata.assetUUID;
                if (mLoaders.contains(eventData.type))
                    mLoaders.at(eventData.type)(eventUUID, eventData.handle);
            } break;

            case event_type::asset_uploaded:
            {
                auto eventData = event.get<asset_uploaded_event_data>();

                if (mUploaders.contains(eventData.type))
                    mUploaders.at(eventData.type)(eventData.assetHandle, eventData.gpuHandle);
            } break;

            default:
                return;
        }
    }
}