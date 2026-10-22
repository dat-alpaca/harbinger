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
    harbinger_engine::harbinger_engine(const char* configFilepath)
    {
        // Config:
        context.config = parse_config(configFilepath);
        auto& config = context.config;

        u32 staticMemoryAmount = config.get_required_static_memory();
        u32 dynamicMemoryAmount = config.get_required_dynamic_memory();
        u32 transientMemoryAmount = config.get_required_transient_memory();
        u32 safeMemoryAmount = config.get_required_safe_memory();

        // Memory:
        context.memoryPartitions = memory_partitions_initialize({
            .staticMemorySizeBytes = staticMemoryAmount,
            .dynamicMemorySizeBytes = dynamicMemoryAmount, // + MiB(10),
            .transientMemorySizeBytes = transientMemoryAmount,
            .safeMemorySizeBytes = safeMemoryAmount
        });
        auto& memoryPartitions = context.memoryPartitions;

        internal::MainStaticAllocator.initialize(memoryPartitions.staticMemory, memoryPartitions.staticMemorySize);
        internal::MainTransientAllocator.initialize(memoryPartitions.transientMemory, memoryPartitions.transientMemorySize);
        internal::MainDynamicAllocator.initialize(memoryPartitions.dynamicMemory, memoryPartitions.dynamicMemorySize);

        // Logging:
        initialize_logger(&internal::MainDynamicAllocator);

        // Worker pools:
        context.workerPool.initialize(config.workerThreadCount);

        // Event queue:
        context.eventQueue = make_unique<event_queue>(config.eventQueueCapacity, config.eventQueuePeepers);
        auto& eventQueue = context.eventQueue;

        // Scene Holder:
        context.sceneHolder.initialize(eventQueue.get(), config.maxSceneCount);

        // Script:
        context.scriptEngine.initialize({
            .sceneHolder = &context.sceneHolder,
            .assetHandler = &context.assetHandler,
        });

        // Windowing:
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

        // RHI:
        context.rhi.initialize(&context.config, &context.windowing.window, config.rhiArguments);

        // Renderer:
        // renderer.initialize(&rhi, config.maxDrawInfoPerFrame, config.maxMaterialInstances, config.maxMeshes);

        /* Subsystems */
        // Asset Bank & Handler:
        context.assetBank.initialize(config.rhiArguments.maxTextureCount);
        context.assetBank.load_bank(config.assetBankFilepath.c_str());

        {
            asset_memory_options options =
            {
                .maxTextureCount = config.rhiArguments.maxTextureCount,
                .maxPipelineStateCount = config.rhiArguments.maxPipelineCount,
                .maxScriptCount = config.maxScriptCount,
            };
            
            context.assetHandler.initialize({
                .eventQueue = context.eventQueue.get(),
                .workerPool = &context.workerPool,
                .assetBank = &context.assetBank,
                .scriptEngine = &context.scriptEngine,
                .sceneHolder = &context.sceneHolder,
            }, options);
        
            eventQueue->register_peeper({.callback = [&](event& event) { context.assetHandler.on_event(event); }});
        }

        // GPU Uploader:
        context.gpuUploader.initialize({
            .assetHandler = &context.assetHandler,
            .eventQueue = context.eventQueue.get(),
            .assetBank = &context.assetBank,
            .config = &context.config, 
            .rhi = &context.rhi,
        });
        {
            eventQueue->register_peeper({.callback = [&](event& event) { context.gpuUploader.on_event(event); }});
        }


        /* Systems */
        context.scriptSystem.initialize(&context.assetHandler);
        {
            eventQueue->register_peeper({.callback = [&](event& event) { context.scriptSystem.on_event(event); }});
        }

        /* Stats */
        HAR_LOG_INFO("Static memory: {}B", staticMemoryAmount);
        HAR_LOG_INFO("Dynamic memory: {}MB", dynamicMemoryAmount / 1e6);
        HAR_LOG_INFO("Transient memory: {}B", transientMemoryAmount);
        HAR_LOG_INFO("Safe memory: {}B", safeMemoryAmount);
    }

    harbinger_engine::~harbinger_engine()
    {
        /* Blank */
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

    bool harbinger_engine::is_running() const
    {
        return platform::window_is_open(context.windowing.window);
    }
}
