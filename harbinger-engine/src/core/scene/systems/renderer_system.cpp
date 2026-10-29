#include "renderer_system.hpp"

#include "core/assets/material_instance/material_instance_asset.hpp"
#include "core/assets/mesh/mesh_asset.hpp"
#include "core/defines.hpp"
#include "core/assert/assert.hpp"
#include "core/event/event_type.hpp"
#include "core/scene/components.hpp"
#include "core/scene/scene_holder.hpp"
#include "core/scene/renderer_component.hpp"

namespace har
{
    void harbinger_renderer_system::initialize(NON_OWNING harbinger_renderer* renderer, NON_OWNING asset_hub* assetHub, NON_OWNING scene_holder* sceneHolder)
    {
        HAR_ASS(renderer);
        HAR_ASS(assetHub);
        HAR_ASS(sceneHolder);
        rRenderer = renderer;
        rAssetHub = assetHub;
        rSceneHolder = sceneHolder;
    }

    void harbinger_renderer_system::on_render()
    {
        if (!rSceneHolder->get_current_scene())
            return;

        rRenderer->clear_draw_info();
        handle_new_objects();
        
        rRenderer->render();
    }

    void harbinger_renderer_system::on_event(event& event)
    {
        if (!rSceneHolder->get_current_scene())
            return;

        if (event.type == event_type::scene_switched)
            rRenderer->clear_mesh_data();

        if (event.type == event_type::asset_handle_linked)
            handle_scene_assets_linked(event);
    }

    void harbinger_renderer_system::handle_new_objects()
    {
        auto& registry = rSceneHolder->get_current_scene()->get_registry();

        auto view = registry.view<const transform_component, const renderable_component, const material_instance_component>().each();
        for (auto [entity, transform, renderable, matInstance] : view)
        {
            const auto* mesh = registry.try_get<const mesh_component>(entity);
            mesh_handle meshHandle = mesh ? mesh->gpuObjectHandle : invalid_small_handle;

            // TODO: Only mutable
            //if (!renderable.isImmutable)
            //    continue;

            if (renderable.gpuObjectHandle == invalid_small_handle)
                continue;

            // TODO: mateiral instance component
            auto* materialInstanceAssetData = static_cast<material_instance_asset*>(rAssetHub->get_asset(matInstance.assetHandle));

            // Pipeline:
            rRenderer->push_object(transform.transform, renderable.gpuObjectHandle, meshHandle, materialInstanceAssetData->data, renderable.isImmutable);
        }
    }

    void harbinger_renderer_system::handle_scene_assets_linked(event& event)
    {
        auto view = rSceneHolder->get_current_scene()->get_registry().view<mesh_component>();
        for (auto entity : view)
        {
            auto& mesh = view.get<mesh_component>(entity);

            mesh_asset* meshAsset = static_cast<mesh_asset*>(rAssetHub->get_asset(mesh.assetHandle));
            mesh.gpuObjectHandle = rRenderer->push_mesh(mesh.assetHandle, *meshAsset);
        }
    }
}