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
    void harbinger_engine::initialize(const char* configFilepath)
    {
        // Config:
        context.config = parse_config(configFilepath);
        auto& config = context.config;

        u32 staticMemoryAmount      = config.get_required_static_memory();
        u32 dynamicMemoryAmount     = config.get_required_dynamic_memory();
        u32 transientMemoryAmount   = config.get_required_transient_memory();
        u32 safeMemoryAmount        = config.get_required_safe_memory();

        // Memory:
        {
            context.memoryPartitions = memory_partitions_initialize(
            {
                .staticMemorySizeBytes = staticMemoryAmount,
                .dynamicMemorySizeBytes = dynamicMemoryAmount, // + MiB(10),
                .transientMemorySizeBytes = transientMemoryAmount,
                .safeMemorySizeBytes = safeMemoryAmount
            });
            auto& memoryPartitions = context.memoryPartitions;

            internal::MainStaticAllocator.initialize(memoryPartitions.staticMemory, memoryPartitions.staticMemorySize);
            internal::MainTransientAllocator.initialize(memoryPartitions.transientMemory, memoryPartitions.transientMemorySize);
            internal::MainDynamicAllocator.initialize(memoryPartitions.dynamicMemory, memoryPartitions.dynamicMemorySize);
        }
        
        // Logging:
        initialize_logger(&internal::MainDynamicAllocator);

        // Event queue:
        context.eventQueue = make_unique<event_queue>(config.eventQueueCapacity, config.eventQueuePeepers);
        auto& eventQueue = context.eventQueue;

        // Worker pools:
        context.workerPool.initialize(config.workerThreadCount);

        // Scene Holder:
        context.sceneHolder.initialize(eventQueue.get(), config.sceneHolderDesc);

        // Script:
        {
            context.scriptSubsystem.initialize(
            {
                .sceneHolder = &context.sceneHolder,
                .assetHub = &context.assetHub,
            });
        }

        // Windowing:
        {
            context.windowing.initialize(context.eventQueue.get(),
            {
                .major = config.apiMajor,
                .minor = config.apiMinor
            },
            {
                .title = config.windowTitle.c_str(),
                .minWidth = config.windowMinWidth,
                .minHeight = config.windowMinHeight,
                .vsyncFrames = config.windowVsyncFrames
            });
        }

        // RHI:
        context.rhi.initialize(&context.config, &context.windowing.window, config.rhiArguments);

        // Renderer:
        // renderer.initialize(&rhi, config.maxDrawInfoPerFrame, config.maxMaterialInstances, config.maxMeshes);

        // Asset Bank:
        {
            context.assetHub.initialize(
            {
                .workerPool = &context.workerPool,
                .eventQueue = context.eventQueue.get(),
                .sceneHolder = &context.sceneHolder,
            }, config.assetHubDesc);
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

        /* Systems */
        {
            // Script:
            context.scriptSystem.initialize(&context.assetHub);
            eventQueue->register_peeper({.callback = [&](event& event) { context.scriptSystem.on_event(event); }});
        }

        /* Stats */
        HAR_LOG_INFO("Static memory: {}B", staticMemoryAmount);
        HAR_LOG_INFO("Dynamic memory: {}MB", dynamicMemoryAmount / 1e6);
        HAR_LOG_INFO("Transient memory: {}B", transientMemoryAmount);
        HAR_LOG_INFO("Safe memory: {}B", safeMemoryAmount);
    }

    void harbinger_engine::shutdown()
    {
        context.workerPool.shutdown();
    }

    void harbinger_engine::start_frame()
    {
        internal::MainTransientAllocator.clear();

        // Events:
        platform::window_poll_events(context.windowing.window);
        context.eventQueue->process_events();
    }

    void harbinger_engine::on_update()
    {
        context.scriptSystem.on_update();
    }

    void harbinger_engine::invoke_startup_script(const std::filesystem::path& filepath)
    {
        context.scriptSubsystem.invoke_startup_script(filepath);
    }

    bool harbinger_engine::is_running() const
    {
        return platform::window_is_open(context.windowing.window);
    }
}
