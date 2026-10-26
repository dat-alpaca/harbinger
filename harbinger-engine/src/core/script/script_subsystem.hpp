#pragma once
#include <lua.hpp>
#include <filesystem>
#include <sol/sol.hpp>

#include "core/defines.hpp"
#include "core/scene/scene_holder.hpp"
#include "core/assets/asset_hub.hpp"
#include "core/bridges/gpu_uploader.hpp"

namespace har
{
    struct script_subsystem_context
    {
        NON_OWNING scene_holder* sceneHolder;
        NON_OWNING gpu_uploader* gpuUploader;
        NON_OWNING asset_hub* assetHub;
    };

    struct script_subsystem_desc
    {
        constexpr inline u32 get_required_dynamic_memory()
        { 
            return KiB(544); 
        }

        constexpr inline u32 get_required_static_memory()
        { 
            return sizeof(sol::state_view);
        }
    };

    struct script_subsystem
    {
    public:
        void initialize(script_subsystem_context&& context);

    public:
        sol::load_result open_file(const std::filesystem::path& filepath, file_type type = file_type::any); 
        void open_script(const std::filesystem::path& filepath, file_type type = file_type::any); 
        void invoke_startup_script(const std::filesystem::path& filepath, file_type type = file_type::any);

    private:
        void initialize_engine();
        void initialize_event();

    private:
        NON_OWNING script_subsystem_context rContext;
        lua_State* mState = nullptr;
        sol::state_view* mView;
    };
}