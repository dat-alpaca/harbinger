#pragma once
#include "core/assets/asset_bank.hpp"
#include "core/config/config.hpp"
#include "core/defines.hpp"
#include "core/event/event_queue.hpp"
#include "core/memory/allocators.hpp"
#include "core/memory/dynamic_allocator.hpp"
#include "core/memory/linear_allocator.hpp"
#include "core/memory/memory_partitions.hpp"

#include "core/scene/scene_holder.hpp"
#include "core/script/script_engine.hpp"
#include "core/scene/systems/script_system.hpp"
#include "core/subsystems/asset_handler.hpp"
#include "core/subsystems/gpu_uploader.hpp"
#include "core/windowing/windowing.hpp"

#include "core/worker/worker_pool.hpp"
#include "renderer/renderer.hpp"
#include "rhi/glsl_compiler.hpp"
#include "rhi/rhi.hpp"

namespace har
{
    class harbinger_engine
    {
    public:
        static constexpr u64 get_required_static_memory()
        {
            return 0;
        } 

    public:
        harbinger_engine(const char* configFilepath = "config.ini");
        ~harbinger_engine();

    public:
        void start_frame();
        void on_update();

    public:
        bool is_running() const;

    public:
        static inline dynamic_allocator* get_main_dynamic_allocator() { return &internal::MainDynamicAllocator; }
        static inline transient_allocator* get_main_transient_allocator() { return &internal::MainTransientAllocator; }
        static inline static_allocator* get_main_static_allocator() { return &internal::MainStaticAllocator; }

    public:
        config_t config;

        scene_holder sceneHolder;

        memory_partitions memoryPartitions;
        har_unique<event_queue> eventQueue;

        worker_pool workerPool;
        windowing_system windowing;
        script_engine scriptEngine;

        asset_bank assetBank;
        glsl_compiler glslCompiler;

    public:
        asset_handler assetHandler;
        gpu_uploader gpuUploader;

    public:
        har_renderer renderer;
        rhi_t rhi;
    
    private:
        script_system scriptSystem;
    };
}
