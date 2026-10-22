#pragma once
#include <lua.hpp>
#include <filesystem>
#include <sol/sol.hpp>
#include "core/defines.hpp"
#include "core/scene/scene_holder.hpp"
#include "core/subsystems/asset_handler.hpp"
#include "sol/forward.hpp"
#include "sol/load_result.hpp"

namespace har
{
    class harbinger_engine;

    struct script_engine_context
    {
        NON_OWNING scene_holder* sceneHolder;
        NON_OWNING asset_handler* assetHandler;
    };

    struct script_engine
    {
    public:
        static constexpr inline u32 get_required_dynamic_memory() { return KiB(544); }
        static constexpr inline u32 get_required_static_memory()
        { 
            return sizeof(sol::state_view);
        }

    public:
        void initialize(script_engine_context&& context);

    public:
        sol::load_result open_file(const std::filesystem::path& filepath, file_type type = file_type::any); 
        void open_script(const std::filesystem::path& filepath, file_type type = file_type::any); 
        void invoke_startup_script(const std::filesystem::path& filepath, file_type type = file_type::any);

    private:
        void initialize_engine();
        void initialize_event();

    private:
        NON_OWNING script_engine_context rContext;
        lua_State* mState = nullptr;
        sol::state_view* mView;
    };
}