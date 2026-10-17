#pragma once
#include "core/containers/nano_string.hpp"
#include "core/defines.hpp"
#include "rhi/common/rhi_arguments.hpp"

namespace har
{
    struct config_t
    {
    public: 
        u32 get_required_static_memory();
        u32 get_required_dynamic_memory();
        u32 get_required_transient_memory();
        u32 get_required_safe_memory();

    public:        
        rhi_arguments rhiArguments;
        nano_string assetBankFilepath;

        nano_string windowTitle = "Engine";
        i32 windowMinWidth  = 640;
        i32 windowMinHeight = 480;
        i32 windowVsyncFrames = 0;

        u32 maxSceneCount = 1;

        u32 tileRendererMaxTileCount = 0;

        u8 eventQueueCapacity = 8;
        u8 eventQueuePeepers = 16;

        u8 apiMajor = 4;
        u8 apiMinor = 6;

        u8 workerThreadCount = 4;
    };
}