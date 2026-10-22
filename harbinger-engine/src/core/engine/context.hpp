#pragma once
#include "core/assets/asset_bank.hpp"
#include "core/config/config.hpp"
#include "core/defines.hpp"
#include "core/event/event_queue.hpp"
#include "core/memory/memory_partitions.hpp"

#include "core/scene/scene_holder.hpp"
#include "core/script/script_engine.hpp"
#include "core/scene/systems/script_system.hpp"
#include "core/subsystems/asset_handler.hpp"
#include "core/subsystems/gpu_uploader.hpp"
#include "core/windowing/windowing.hpp"

#include "core/worker/worker_pool.hpp"
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
        script_engine scriptEngine;
        asset_handler assetHandler;
        scene_holder sceneHolder;
        worker_pool workerPool;
        asset_bank assetBank;
        
    public:
        windowing_system windowing;
        gpu_uploader gpuUploader;
        har_renderer renderer;
        rhi_t rhi;
    
    public:
        script_system scriptSystem;
    };
}