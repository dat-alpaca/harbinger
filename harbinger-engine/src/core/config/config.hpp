#pragma once
#include "core/assets/asset_hub.hpp"
#include "core/defines.hpp"
#include "core/containers/nano_string.hpp"
#include "core/event/event_queue.hpp"
#include "core/scene/scene_holder.hpp"
#include "core/script/script_subsystem.hpp"
#include "core/worker/worker_pool.hpp"
#include "platform/window_settings.hpp"
#include "renderer/renderer.hpp"
#include "rhi/common/rhi_arguments.hpp"

namespace har
{
    struct config_t
    {
    public: 
        u32 get_required_static_memory();
        u32 get_required_dynamic_memory();
        u32 get_required_transient_memory();

    public:        
        nano_string assetBankFilepath;
        u32 maxScriptCount = 1;
        u8 apiMajor = 4;
        u8 apiMinor = 6;

        platform::window_settings windowSettings;
        harbinger_renderer_desc rendererDesc;
        scene_holder_desc sceneHolderDesc;
        worker_pool_desc workerPoolDesc;
        event_queue_desc eventQueueDesc;
        rhi_arguments rhiArguments;
        asset_hub_desc assetHubDesc;
        script_subsystem_desc scriptSubsystemDesc;
    };
}