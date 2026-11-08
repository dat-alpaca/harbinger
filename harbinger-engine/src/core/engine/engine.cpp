#include "engine.hpp"

#include "core/defines.hpp"
#include "core/config/config.hpp"
#include "core/logging/logger.hpp"
#include "core/event/event_queue.hpp"
#include "core/config/config_parser.hpp"
#include "core/memory/allocators.hpp"
#include "core/memory/dynamic_allocator.hpp"
#include "core/memory/memory_partitions.hpp"

#include "core/scene/scene_holder.hpp"
#include "platform/window.hpp"

namespace har
{
    void harbinger_engine_initialize(context_t& context, const char* configFilepath)
    {
        // Config:
        context.config = parse_config(configFilepath);
        auto& config = context.config;

        u32 staticMemoryAmount      = config.get_required_static_memory();
        u32 dynamicMemoryAmount     = config.get_required_dynamic_memory();
        u32 transientMemoryAmount   = config.get_required_transient_memory();

        // Memory:
        {
            context.memoryPartitions = memory_partitions_initialize(
            {
                .staticMemorySizeBytes = staticMemoryAmount,
                .dynamicMemorySizeBytes = dynamicMemoryAmount,
                .transientMemorySizeBytes = transientMemoryAmount,
            });
            auto& memoryPartitions = context.memoryPartitions;

            internal::MainStaticAllocator.initialize(memoryPartitions.staticMemory, memoryPartitions.staticMemorySize);
            internal::MainTransientAllocator.initialize(memoryPartitions.transientMemory, memoryPartitions.transientMemorySize);
            internal::MainDynamicAllocator.initialize(memoryPartitions.dynamicMemory, memoryPartitions.dynamicMemorySize);
        }
        
        // Logging:
        initialize_logger(&internal::MainDynamicAllocator);

        // Event queue:
        context.eventQueue = make_unique<event_queue>(config.eventQueueDesc);
        auto& eventQueue = context.eventQueue;

        // Worker pools:
        context.workerPool.initialize(config.workerPoolDesc);

        // Scene Holder:
        context.sceneHolder.initialize(eventQueue.get(), config.sceneHolderDesc);

        // Windowing:
        {
            context.windowing.initialize(context.eventQueue.get(),
            {
                .major = config.apiMajor,
                .minor = config.apiMinor
            }, config.windowSettings);
        }

        // RHI:
        context.rhi.initialize(&context.config, &context.windowing.window, config.rhiArguments);

        // GLSL:
        glsl_compiler::initialize();

        // Asset Bank:
        {
            context.assetHub.initialize(
            {
                .workerPool = &context.workerPool,
                .eventQueue = context.eventQueue.get(),
                .sceneHolder = &context.sceneHolder,
            }, config.assetHubDesc, config.assetBankFilepath.c_str());
        }

        // GPU Uploader:
        {
            context.gpuUploader.initialize(
            {
                .assetHub = &context.assetHub,
                .eventQueue = context.eventQueue.get(),
                .config = &context.config, 
                .rhi = &context.rhi,
            });
        }

        // Script Subsystem:
        {
            context.scriptSubsystem.initialize(
            {
                .sceneHolder = &context.sceneHolder,
                .gpuUploader = &context.gpuUploader,
                .eventQueue = context.eventQueue.get(),
                .assetHub = &context.assetHub,
            });
        }

        // Asset Linker:
        {
            context.assetLinker.initialize(
            {
                .assetHub = &context.assetHub,
                .sceneHolder = &context.sceneHolder,
                .eventQueue = context.eventQueue.get(),
            });
            eventQueue->register_peeper({.callback = [&](event& event) { context.assetLinker.on_event(event); }});
        }

        // Renderer:
        context.renderer.initialize(&context.rhi, &context.assetLinker, context.eventQueue.get(), config.rendererDesc);

        /* Systems */
        {
            // Script:
            context.scriptSystem.initialize(&context.assetHub);
            eventQueue->register_peeper({.callback = [&](event& event) { context.scriptSystem.on_event(event); }});
        
            // Material Instance Proxy:
            context.materialInstanceProxy.initialize(&context.assetHub, &context.renderer, context.eventQueue.get());
            eventQueue->register_peeper({.callback = [&](event& event) { context.materialInstanceProxy.on_event(event); }});

            // Renderer:
            context.rendererSystem.initialize
            ({
                .renderer = &context.renderer,
                .assetHub = &context.assetHub,
                .eventQueue = context.eventQueue.get(), 
                .sceneHolder = &context.sceneHolder,
                .materialProxy = &context.materialInstanceProxy
            });
            eventQueue->register_peeper({.callback = [&](event& event) { context.rendererSystem.on_event(event); }});
        }

        /* Stats */
        HAR_LOG_INFO("Static memory: {}B", staticMemoryAmount);
        HAR_LOG_INFO("Dynamic memory: {}MB", dynamicMemoryAmount / 1e6);
        HAR_LOG_INFO("Transient memory: {}B", transientMemoryAmount);
    }

    void harbinger_engine_shutdown(context_t& context)
    {
        glsl_compiler::shutdown();

        context.workerPool.shutdown();
    }

    void harbinger_engine_start_frame(context_t& context)
    {
        internal::MainTransientAllocator.clear();

        // Events:
        platform::window_poll_events(context.windowing.window);
        context.eventQueue->process_events();
    }

    void harbinger_engine_on_update(context_t& context)
    {
        context.scriptSystem.on_update();
    }

    bool harbinger_engine_is_running(context_t& context)
    {
        return platform::window_is_open(context.windowing.window);
    }
}
