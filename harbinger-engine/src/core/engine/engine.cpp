#include "engine.hpp"

#include "core/defines.hpp"
#include "core/config/config.hpp"
#include "core/logging/logger.hpp"
#include "core/event/event_queue.hpp"
#include "core/config/config_parser.hpp"
#include "core/memory/allocators.hpp"
#include "core/memory/dynamic_allocator.hpp"
#include "core/memory/memory_partitions.hpp"

#include "platform/window.hpp"

namespace har
{
    harbinger_engine::harbinger_engine(const char* configFilepath)
        : rhi(this)
    {
        // Config:
        config = parse_config(configFilepath);

        u32 staticMemoryAmount = config.get_required_static_memory();
        u32 dynamicMemoryAmount = config.get_required_dynamic_memory();
        u32 transientMemoryAmount = config.get_required_transient_memory();
        u32 safeMemoryAmount = config.get_required_safe_memory();

        // Memory:
        memoryPartitions = memory_partitions_initialize({
            .staticMemorySizeBytes = staticMemoryAmount,
            .dynamicMemorySizeBytes = dynamicMemoryAmount, // + MiB(10),
            .transientMemorySizeBytes = transientMemoryAmount,
            .safeMemorySizeBytes = safeMemoryAmount
        });

        internal::MainStaticAllocator.initialize(memoryPartitions.staticMemory, memoryPartitions.staticMemorySize);
        internal::MainTransientAllocator.initialize(memoryPartitions.transientMemory, memoryPartitions.transientMemorySize);
        internal::MainDynamicAllocator.initialize(memoryPartitions.dynamicMemory, memoryPartitions.dynamicMemorySize);

        // Logging:
        initialize_logger(&internal::MainDynamicAllocator);

        // Worker pools:
        workerPool.initialize(config.workerThreadCount);

        // Event queue:
        eventQueue = make_unique<event_queue>(config.eventQueueCapacity, config.eventQueuePeepers);

        // Scene Holder:
        sceneHolder.initialize(eventQueue.get(), config.maxSceneCount);

        // Script:
        scriptEngine.initialize(this);

        // Windowing:
        windowing.initialize(this, 
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
        rhi.initialize(config.rhiArguments);

        // Renderer:
        // renderer.initialize(&rhi, config.maxDrawInfoPerFrame, config.maxMaterialInstances, config.maxMeshes);

        // GLSL Compiler:
        glslCompiler.initialize();

        /* Subsystems */
        // Asset Bank & Handler:
        assetBank.initialize(config.rhiArguments.maxTextureCount);
        assetBank.load_bank(config.assetBankFilepath.c_str());

        {
            asset_memory_options options =
            {
                .maxTextureCount = config.rhiArguments.maxTextureCount,
                .maxPipelineStateCount = config.rhiArguments.maxPipelineCount,
                .maxScriptCount = config.maxScriptCount,
            };
            
            assetHandler.initialize(this, options);
        
            eventQueue->register_peeper({.callback = [&](event& event) { assetHandler.on_event(event); }});
        }

        // GPU Uploader:
        gpuUploader.initialize(this);
        {
            eventQueue->register_peeper({.callback = [&](event& event) { gpuUploader.on_event(event); }});
        }


        /* Systems */
        scriptSystem.initialize(&assetHandler);
        {
            eventQueue->register_peeper({.callback = [&](event& event) { scriptSystem.on_event(event); }});
        }

        /* Stats */
        HAR_LOG_INFO("Static memory: {}B", staticMemoryAmount);
        HAR_LOG_INFO("Dynamic memory: {}MB", dynamicMemoryAmount / 1e6);
        HAR_LOG_INFO("Transient memory: {}B", transientMemoryAmount);
        HAR_LOG_INFO("Safe memory: {}B", safeMemoryAmount);
    }

    harbinger_engine::~harbinger_engine()
    {
        // glslCompiler.shutdown();
    }

    void harbinger_engine::start_frame()
    {
        internal::MainTransientAllocator.clear();

        // Events:
        platform::window_poll_events(windowing.window);
        eventQueue->process_events();
    }

    void harbinger_engine::on_update()
    {
        scriptSystem.on_update();
    }

    bool harbinger_engine::is_running() const
    {
        return platform::window_is_open(windowing.window);
    }
}
