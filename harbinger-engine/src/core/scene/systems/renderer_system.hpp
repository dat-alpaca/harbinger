#pragma once
#include "core/defines.hpp"
#include "core/event/event.hpp"
#include "core/assets/asset_hub.hpp"
#include "core/assets/material_instance/material_instance_asset.hpp"

#include "core/scene/systems/material_instance_proxy.hpp"
#include "renderer/renderer.hpp"
#include "renderer/material.hpp"
#include "core/event/event_queue.hpp"
#include "core/scene/scene_holder.hpp"

namespace har
{
    struct harbinger_renderer_system_ctx
    {
        NON_OWNING harbinger_renderer* renderer;
        NON_OWNING asset_hub* assetHub;
        NON_OWNING event_queue* eventQueue;
        NON_OWNING scene_holder* sceneHolder;
        NON_OWNING material_instance_proxy* materialProxy;
    };

    class harbinger_renderer_system
    {
    public:
        void initialize(harbinger_renderer_system_ctx&& ctx);
        void on_event(event& event);
        void on_render();

    private:
        void handle_new_objects();
        void handle_asset_loaded(event& event);
        void handle_asset_uploaded(event& event);
    
    private:
        void update_material_instance_data_with_texture(material_instance_asset* asset, material_instance_handle handle);

    private:
        harbinger_renderer_system_ctx ctx;
    };
}