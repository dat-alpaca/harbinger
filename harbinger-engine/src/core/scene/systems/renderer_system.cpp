#include "renderer_system.hpp"

#include "core/defines.hpp"
#include "core/assert/assert.hpp"
#include "core/assets/asset_type.hpp"
#include "core/assets/material_instance/material_instance_asset.hpp"
#include "core/assets/mesh/mesh_asset.hpp"

#include "core/event/concrete/asset_events.hpp"
#include "core/event/event_type.hpp"
#include "core/scene/components.hpp"
#include "core/scene/scene_holder.hpp"
#include "core/scene/renderer_component.hpp"

#include "renderer/renderer.hpp"

namespace har
{
    void harbinger_renderer_system::initialize(harbinger_renderer_system_ctx&& ctx)
    {
        HAR_ASS(ctx.renderer);
        HAR_ASS(ctx.assetHub);
        HAR_ASS(ctx.eventQueue);
        HAR_ASS(ctx.sceneHolder);
        HAR_ASS(ctx.materialProxy);

        this->ctx = std::move(ctx); 
    }

    void harbinger_renderer_system::on_render()
    {
        if (!ctx.sceneHolder->get_current_scene())
            return;

        ctx.renderer->clear_draw_info();
        handle_new_objects();
        
        ctx.renderer->render();
    }

    void harbinger_renderer_system::on_event(event& event)
    {
        if (!ctx.sceneHolder->get_current_scene())
            return;

        if (event.type == event_type::scene_switched)
            ctx.renderer->clear_mesh_data();

        if (event.type == event_type::asset_loaded)
            handle_asset_loaded(event);

        if (event.type == event_type::asset_uploaded)
            handle_asset_uploaded(event);
    }

    void harbinger_renderer_system::handle_new_objects()
    {
        auto& registry = ctx.sceneHolder->get_current_scene()->get_registry();

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

            if (matInstance.gpuObjectHandle != invalid_small_handle)
            {
                auto instanceData = ctx.materialProxy->get_material_instance_data(matInstance.gpuObjectHandle);
                ctx.renderer->push_object
                (
                    transform.transform,
                    renderable.gpuObjectHandle,
                    meshHandle,
                    { instanceData.first, instanceData.second }, 
                    renderable.isImmutable
                );
            }
            else
                ctx.renderer->push_object(transform.transform, renderable.gpuObjectHandle, meshHandle, {}, renderable.isImmutable);
        }
    }
}

namespace har
{
    void harbinger_renderer_system::handle_asset_loaded(event& event)
    {
        auto eventData = event.get<asset_loaded_event_data>();
        
        if (eventData.type == asset_type::mesh)
        {
            send_asset_uploaded_event
            (
                *ctx.eventQueue, 
                { 
                    .assetHandle = eventData.handle, 
                    .gpuHandle = ctx.renderer->push_mesh(eventData.handle),
                    .type = eventData.type
                }
            );
        }
    }
    void harbinger_renderer_system::handle_asset_uploaded(event& event)
    {
        auto eventData = event.get<asset_uploaded_event_data>();
        if (eventData.type != asset_type::texture)
            return;

        ctx.renderer->push_texture(eventData.assetHandle, eventData.gpuHandle);
    }
}