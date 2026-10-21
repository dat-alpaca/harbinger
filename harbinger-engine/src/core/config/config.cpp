#include "config.hpp"
#include "core/defines.hpp"
#include "core/event/event.hpp"
#include "core/event/event_queue.hpp"
#include "core/scene/scene_holder.hpp"
#include "core/script/script_engine.hpp"
#include "core/subsystems/asset_handler.hpp"
#include "core/engine/engine.hpp"
#include "platform/middleware.hpp"
#include "rhi/common/rhi_arguments.hpp"

namespace har
{
    // TODO: asset cache
    u32 config_t::get_required_static_memory()
    {
        u32 eventQueueMemory = sizeof(event_peeper) * eventQueuePeepers;
        u32 rhiMemory = get_rhi_required_static_memory(rhiArguments);
        //u32 rendererMemory = har_renderer::get_required_static_memory(maxMaterialInstances, maxMeshes);
        u32 sceneHolderMemory = scene_holder::get_required_static_memory(maxSceneCount);
        u32 assetHandler = asset_handler::get_required_static_memory
        ({ 
            .maxTextureCount = rhiArguments.maxTextureCount,
            .maxPipelineStateCount = rhiArguments.maxPipelineCount
        });
        u32 scriptEngineMemory = script_engine::get_required_static_memory();

        return rhiMemory + eventQueueMemory + sceneHolderMemory + assetHandler + scriptEngineMemory;// + rendererMemory;
    }

    u32 config_t::get_required_dynamic_memory()
    {
        // u32 luaMemory = script_system::get_required_dynamic_memory();
        u32 windowingMemory = platform::get_required_windowing_dynamic_memory();
        //u32 rhiMemory = get_rhi_required_dynamic_memory(rhiArguments);
        //u32 workerPoolMemory = worker_pool::required_dynamic_memory_worker_pool(workerThreadCount);
        //u32 loggerMemory = required_logger_dynamic_memory();
        // return luaMemory + windowingMemory + rhiMemory + workerPoolMemory + loggerMemory;

        // TODO: Windowing takes most of the required initialization cost and releases it afterwards.
        return windowingMemory;
    }

    u32 config_t::get_required_transient_memory()
    {
        u32 eventQueueMemory = sizeof(event) * eventQueueCapacity;
        // u32 rendererMemory = har_renderer::get_required_transient_memory(maxDrawInfoPerFrame);

        return eventQueueMemory; // + tilemapRendererTiles + rendererMemory;
    }

    u32 config_t::get_required_safe_memory()
    {
        return 64;
    }
}