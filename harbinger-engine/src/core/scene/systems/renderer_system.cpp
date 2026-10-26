#include "renderer_system.hpp"
#include "core/assert/assert.hpp"
#include "core/assets/asset_type.hpp"
#include "core/assets/pso/pso_asset.hpp"
#include "core/defines.hpp"
#include "core/event/concrete/scene_events.hpp"
#include "core/event/event_type.hpp"
#include "core/scene/components.hpp"
#include "core/scene/renderer_component.hpp"

namespace har
{
    void harbinger_renderer_system::initialize(NON_OWNING harbinger_renderer* renderer, NON_OWNING asset_hub* assetHub)
    {
        HAR_ASS(renderer);
        HAR_ASS(assetHub);
        rRenderer = renderer;
        rAssetHub = assetHub;
    }

    void harbinger_renderer_system::on_event(event& event)
    {
        if (event.type == event_type::scene_switched)
            handle_scene_switch(event);
    }

    void harbinger_renderer_system::handle_scene_switch(event& event)
    {
        rCurrentScene = event.get<scene_switch_data>().current;
        rRenderer->clear_draw_info();

        auto& registry = rCurrentScene->get_registry();
        auto view = registry.view<const transform_component, const renderable_component, const material_instance_component>().each();
        for (auto [_, transform, renderable, matInstance] : view)
        {
            if (!renderable.isImmutable)
                continue;

            // TODO: mateiral instance component
            asset* materialInstanceAssetData = rAssetHub->get_asset(matInstance.assetHandle);

            // Pipeline:
            rRenderer->push_draw_info(renderable.pipelineHandle, true);
        }
    }
}