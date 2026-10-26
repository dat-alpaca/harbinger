#pragma once
#include "core/assets/asset_hub.hpp"
#include "core/defines.hpp"
#include "core/event/event.hpp"
#include "renderer/renderer.hpp"

namespace har
{
    class harbinger_renderer_system
    {
    public:
        void initialize(NON_OWNING harbinger_renderer* renderer, NON_OWNING asset_hub* assetHub);
        void on_event(event& event);

    private:
        void handle_scene_switch(event& event);
    
    private:
        NON_OWNING harbinger_renderer* rRenderer = nullptr;
        NON_OWNING scene_t* rCurrentScene = nullptr;
        NON_OWNING asset_hub* rAssetHub = nullptr;
    };
}