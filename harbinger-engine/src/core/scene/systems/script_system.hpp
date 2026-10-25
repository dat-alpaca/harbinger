#pragma once
#include "core/defines.hpp"
#include "core/event/event.hpp"
#include "core/scene/scene.hpp"
#include "core/assert/assert.hpp"
#include "core/scene/components.hpp"
#include "core/event/event_type.hpp"
#include "core/bridges/asset_handler.hpp"
#include "core/assets/script/script_loader.hpp"
#include "core/event/concrete/scene_events.hpp"

namespace har
{
    class script_system
    {
    public:
        void initialize(NON_OWNING asset_handler* handler)
        {
            HAR_ASS(handler);
            rAssetHandler = handler;
        }

    public:
        void on_event(event& event)
        {
            handle_entity_on_event(event);

            if (event.type != event_type::scene_switched)
                return;

            handle_entity_on_event(event);
        }

        void on_update()
        {
            if (!mCurrentScene)
                return;

            for (auto [e, scriptComponent] : mCurrentScene->get_registry().view<script_component>().each())
            {
                auto& script = rAssetHandler->get_scripts().get_asset(scriptComponent.handle)->resource;
                script.on_update();
            }
        }

    private:
        void handle_entity_on_event(event& event)
        {
            if (!mCurrentScene)
                return;

            for (auto [e, scriptComponent] : mCurrentScene->get_registry().view<script_component>().each())
            {
                auto& script = rAssetHandler->get_scripts().get_asset(scriptComponent.handle)->resource;
                script.on_event(event);
            }
        }

        void handle_scene_switch(event& event)
        {
            scene_switch_data data = event.get<scene_switch_data>();
            mCurrentScene = data.current;

            for (auto [entity, scriptComponent] : mCurrentScene->get_registry().view<script_component>().each())
            {
                auto& script = rAssetHandler->get_scripts().get_asset(scriptComponent.handle)->resource;
                post_load_script_task(script, entity);
            }
        }

    private:
        NON_OWNING asset_handler* rAssetHandler;
        NON_OWNING scene_t* mCurrentScene = nullptr;
    };
}