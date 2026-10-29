#include "config.hpp"

#include "core/defines.hpp"
#include "core/event/event_queue.hpp"
#include "core/scene/scene_holder.hpp"
#include "core/script/script_subsystem.hpp"

#include "platform/middleware.hpp"
#include "rhi/common/rhi_arguments.hpp"

namespace har
{
    u32 config_t::get_required_static_memory()
    {
        return 
            sceneHolderDesc.get_required_static_memory() +
            assetHubDesc.get_required_static_memory() +
            eventQueueDesc.get_required_static_memory() +
            rhiArguments.get_rhi_required_static_memory() + 
            scriptSubsystemDesc.get_required_static_memory() +
            rendererDesc.get_required_static_memory();
    }

    u32 config_t::get_required_dynamic_memory()
    {
        // TODO: Windowing takes most of the required initialization cost and releases it afterwards.
        u32 windowingMemory = platform::get_required_windowing_dynamic_memory();

        return 
            windowingMemory + 
            workerPoolDesc.required_dynamic_memory_worker_pool() +
            scriptSubsystemDesc.get_required_dynamic_memory();
    }

    u32 config_t::get_required_transient_memory()
    {
        return 
            rendererDesc.get_required_transient_memory();
    }
}