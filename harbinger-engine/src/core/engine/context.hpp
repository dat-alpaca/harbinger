#pragma once
#include "core/defines.hpp"
#include "core/config/config.hpp"
#include "core/assets/asset_hub.hpp"
#include "core/event/event_queue.hpp"
#include "core/memory/memory_partitions.hpp"

#include "core/scene/systems/material_instance_proxy.hpp"
#include "core/scene/systems/renderer_system.hpp"
#include "core/worker/worker_pool.hpp"
#include "core/scene/scene_holder.hpp"
#include "core/windowing/windowing.hpp"

#include "core/bridges/gpu_uploader.hpp"
#include "core/script/script_subsystem.hpp"
#include "core/scene/systems/script_system.hpp"
#include "core/scene/systems/asset_linker.hpp"

#include "renderer/renderer.hpp"
#include "rhi/rhi.hpp"

namespace har
{
    struct context_t
    {
    public:
        config_t config;
        memory_partitions memoryPartitions;

    public:
        har_unique<event_queue> eventQueue;
        script_subsystem scriptSubsystem;
        scene_holder sceneHolder;
        worker_pool workerPool;
        asset_hub assetHub;
        
    public:
        windowing_subsystem windowing;
        harbinger_renderer renderer;
        rhi_t rhi;

    public:
        gpu_uploader gpuUploader;
        asset_linker assetLinker;
    
    public:
        script_system scriptSystem;
        harbinger_renderer_system rendererSystem;
        material_instance_proxy materialInstanceProxy;
    };
}