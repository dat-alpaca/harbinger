#pragma once
#include "core/assets/asset_hub.hpp"
#include "core/defines.hpp"
#include "core/event/event.hpp"
#include "core/scene/scene_holder.hpp"
#include "renderer/renderer.hpp"

namespace har
{
    class harbinger_renderer_system
    {
    public:
        void initialize(NON_OWNING harbinger_renderer* renderer, NON_OWNING asset_hub* assetHub, NON_OWNING scene_holder* sceneHolder);
        void on_event(event& event);
        void on_render();

    private:
        void handle_new_objects();
        void handle_scene_assets_linked(event& event);
    
    private:
        NON_OWNING harbinger_renderer* rRenderer = nullptr;
        NON_OWNING scene_holder* rSceneHolder = nullptr;
        NON_OWNING asset_hub* rAssetHub = nullptr;
    };
}