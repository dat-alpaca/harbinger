#pragma once
#include "core/config/config.hpp"
#include <cstdlib>
#include <ini.h>

#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0

namespace har
{
    static inline i32 __parse_handler(void* user, const char* section, const char* name, const char* value)
    {
        config_t* config = reinterpret_cast<config_t*>(user);

        // Bank:
        if (MATCH("asset_bank", "filepath"))
            config->assetBankFilepath = strdup(value);

        else if (MATCH("memory", "maxScriptCount"))
            config->maxScriptCount = std::atoi(value);

        // Window:
        else if (MATCH("window", "title"))
            config->windowSettings.title = strdup(value);

        else if (MATCH("window", "minWidth"))
            config->windowSettings.minWidth = std::atoi(value);

        else if (MATCH("window", "minHeight"))
            config->windowSettings.minHeight = std::atoi(value);

        else if (MATCH("window", "vsyncFrames"))
            config->windowSettings.vsyncFrames = std::atoi(value);

        // API:
        else if (MATCH("api", "major"))
            config->apiMajor = std::atoi(value);

        else if (MATCH("api", "minor"))
            config->apiMinor = std::atoi(value);

        // Event queue:
        else if (MATCH("events", "capacity"))
            config->eventQueueDesc.queueCapacity = std::atoi(value);

        else if (MATCH("events", "peepers"))
            config->eventQueueDesc.peeperCount = std::atoi(value);

        // RHI:
        else if (MATCH("rhi", "maxShaderCount"))
            config->rhiArguments.maxShaderCount = std::atoi(value);

        else if (MATCH("rhi", "maxBufferCount"))
            config->rhiArguments.maxBufferCount = std::atoi(value);

        else if (MATCH("rhi", "maxTextureCount"))
            config->rhiArguments.maxTextureCount = std::atoi(value);

        else if (MATCH("rhi", "maxSamplerCount"))
            config->rhiArguments.maxSamplerCount = std::atoi(value);

        else if (MATCH("rhi", "maxRenderTargetCount"))
            config->rhiArguments.maxRenderTargetCount = std::atoi(value);
        
        else if (MATCH("rhi", "maxVertexAttributesPerPipeline"))
            config->rhiArguments.maxVertexAttributesPerPipeline = std::atoi(value);

        else if (MATCH("rhi", "maxPipelineCount"))
            config->rhiArguments.maxPipelineCount = std::atoi(value);

        else if (MATCH("rhi", "maxCombinedTextureCount"))
            config->rhiArguments.maxCombinedTextureCount = std::atoi(value);

        // Worker Pools:
        else if (MATCH("workers", "workerThreadCount"))
            config->workerPoolDesc.threadCount = std::atoi(value);
        
        // Scene Holder:
        else if (MATCH("scene_holder", "maxSceneCount"))
            config->sceneHolderDesc.maxSceneCount = std::atoi(value);       

        else
            return false;

        return true;
    }

    inline config_t parse_config(const char* filepath)
    {
        config_t config = {};

        if (ini_parse(filepath, __parse_handler, &config) < 0)
        {
            // LOG: invalid
            return config;
        }

        return config;
    }
}

#undef MATCH