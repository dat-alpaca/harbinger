#include "renderer_system.hpp"

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

    void harbinger_renderer_system::on_event(event& event)
    {
        if (event.type == event_type::scene_assets_loaded)
            handle_scene_switch(event);
    }

    void harbinger_renderer_system::handle_scene_switch(event& event)
    {
        rRenderer->clear_draw_info();

        auto& registry = rSceneHolder->get_current_scene()->get_registry();
        auto view = registry.view<const transform_component, const renderable_component, const material_instance_component>().each();
        for (auto [_, transform, renderable, matInstance] : view)
        {
            if (!renderable.isImmutable)
                continue;

            if (renderable.pipelineHandle == invalid_small_handle)
                continue;

            // TODO: mateiral instance component
            // asset* materialInstanceAssetData = rAssetHub->get_asset(matInstance.assetHandle);

            // Pipeline:
            rRenderer->push_draw_info(renderable.pipelineHandle, true);
        }
    }
}