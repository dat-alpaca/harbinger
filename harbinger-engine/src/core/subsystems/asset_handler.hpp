#pragma once
#include "core/scene/scene.hpp"
#include "rhi/common/objects.hpp"

#include "core/defines.hpp"
#include "core/event/event.hpp"
#include "core/assets/asset.hpp"
#include "core/assets/asset_cache.hpp"
#include "core/assets/pso/pso_asset.hpp"
#include "core/containers/containers.hpp"
#include "core/assets/script/script_asset.hpp"
#include "core/event/concrete/asset_events.hpp"
#include "core/assets/texture/texture_asset.hpp"
#include "core/assets/texture/texture_loader.hpp"

namespace har
{
    struct asset_memory_options
    {
        u32 maxTextureCount;
        u32 maxPipelineStateCount;
        u32 maxScriptCount;
    };

    class harbinger_engine;

    class asset_handler
    {
    public:
        constexpr static inline u64 get_required_static_memory(const asset_memory_options& options)
        {
            return 
                asset_cache<texture_asset>::get_required_static_memory(options.maxTextureCount) +
                asset_cache<pso_asset>::get_required_static_memory(options.maxPipelineStateCount) +
                asset_cache<script_asset>::get_required_static_memory(options.maxScriptCount);
        }

    public:
        void initialize(NON_OWNING harbinger_engine* engine, const asset_memory_options& options);

    public:
        void on_event(event& event);

    public:
        asset_handle load_texture(const std::filesystem::path& filepath, const texture_load_options& loadOptions, asset_upload_options uploadOptions);
        void unload_texture(asset_handle handle);
        void unload_texture(const std::filesystem::path& filepath);

    public:
        asset_handle load_pipeline_state(const std::filesystem::path& filepath);

    public:
        asset_handle load_script(const std::filesystem::path& filepath);

    public:
        scene_handle load_scene(const std::filesystem::path& filepath);

    public:
        constexpr auto& get_texture_map()       { return mTextureMap; }
        constexpr auto& get_pipeline_map()      { return mPipelineStateMap; }

        constexpr auto& get_textures()          { return mTextureCache; }
        constexpr auto& get_pipeline_states()   { return mPipelineStatesCache; }
        constexpr auto& get_scripts()           { return mScriptCache; }
        
    private:
        har_unordered_map<asset_handle, combined_texture_handle> mTextureMap;
        har_unordered_map<asset_handle, graphics_pipeline_handle> mPipelineStateMap;
        har_unordered_map<std::string, scene_handle> mSceneMap;

        asset_cache<texture_asset> mTextureCache;
        asset_cache<pso_asset> mPipelineStatesCache;
        asset_cache<script_asset> mScriptCache;

    private:
        NON_OWNING harbinger_engine* rEngine;
    };
}