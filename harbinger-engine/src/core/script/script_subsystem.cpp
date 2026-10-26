#include "script_subsystem.hpp"

#include <memory>
#include <stdlib.h>

#include <lualib.h>
#include <sol/forward.hpp>
#include <sol/load_result.hpp>
#include <sol/object.hpp>
#include <sol/overload.hpp>
#include <sol/raii.hpp>
#include <sol/state_view.hpp>
#include <sol/types.hpp>

#include "core/assert/assert.hpp"
#include "core/assets/asset.hpp"
#include "core/bridges/gpu_uploader.hpp"
#include "core/defines.hpp"
#include "core/event/concrete/asset_events.hpp"
#include "core/event/concrete/scene_events.hpp"
#include "core/event/concrete/window_events.hpp"
#include "core/event/event.hpp"
#include "core/logging/logger.hpp"
#include "core/memory/allocators.hpp"
#include "core/memory/memory_tag.hpp"
#include "core/scene/scene.hpp"
#include "core/script/texture_script_options.hpp"

static void* lua_allocator(void* user, void* address, size_t oldSize, size_t newSize)
{
    using namespace har;
    using namespace internal;

    if (newSize == 0)
    {
        if (address)
            MainDynamicAllocator.deallocate(memory_tag::script_system, address);
        
        return nullptr;
    }

    if (!address)
        return MainDynamicAllocator.allocate(har::memory_tag::script_system, newSize);

    return MainDynamicAllocator.reallocate(har::memory_tag::script_system, address, newSize);
}

constexpr static sol::load_mode get_load_mode(har::file_type type)
{
    switch (type)
    {
        case har::file_type::text: 
            return sol::load_mode::text;

        case har::file_type::binary: 
            return sol::load_mode::binary;

        default:
            return sol::load_mode::any;
    }
}

namespace har
{
    void script_subsystem::initialize(script_subsystem_context&& context)
    {
        rContext = std::move(context);
        HAR_ASS(rContext.sceneHolder && rContext.assetHub && rContext.gpuUploader);

        mState = lua_newstate(lua_allocator, &rContext);
        HAR_ASS(mState);

        mView = reinterpret_cast<sol::state_view*>(internal::MainStaticAllocator.push(sizeof(sol::state_view), alignof(u8)));
        std::construct_at(mView, mState);

        luaL_openlibs(mState);

        initialize_engine();
        initialize_event();
    }

    sol::load_result script_subsystem::open_file(const std::filesystem::path& filepath, file_type type)
    {
        return mView->load_file(filepath, get_load_mode(type));
    }

    void script_subsystem::open_script(const std::filesystem::path& filepath, file_type type)
    {
        mView->script_file(filepath, get_load_mode(type));
    }

    void script_subsystem::invoke_startup_script(const std::filesystem::path& filepath, file_type type)
    {
        open_script(filepath, type);

        try 
        {
            (*mView)["startup"]();
        } catch(const sol::error& error) {
            HAR_LOG_ERROR("[Script Engine] Startup script failed: {}", error.what());
        }

        (*mView)["startup"] = sol::nil;

        mView->script("collectgarbage('collect')");
    }
    
    void script_subsystem::initialize_engine()
    {
        sol::table engineTable = mView->create_table();
        
        // Scene:
        sol::table scenesTable = mView->create_table();
        {
            scenesTable["set_current_scene"] = [&](scene_handle handle) {
                rContext.sceneHolder->set_current_scene(handle);
            };
        
            engineTable["scenes"] = scenesTable;
        }
        
        // Assets:
        sol::table assetsTable = mView->create_table();
        {
            // Texture:
            assetsTable["load_texture"] = 
            [&](const std::string& filepath, const sol::table& options) -> asset_handle
            {
                if (!options.valid())
                {
                    HAR_LOG_ERROR("[Script Engine] invalid option arguments passed to load_texture.");
                    return invalid_small_handle;
                }

                asset_handle handle = rContext.assetHub->load_asset(asset_type::texture, filepath, {}, false);

                if (options["upload_immediate"].get_or(true))
                    rContext.gpuUploader->upload_asset_from_hub(asset_type::texture, handle);
            };

            assetsTable["unload_texture"] = sol::overload
            (
                [&](scene_handle handle) {
                    rContext.assetHub->unload_asset(asset_type::texture, handle);
                },

                [&](std::string filepath) {
                    rContext.assetHub->unload_asset(asset_type::texture, filepath);
                }
            );

            // Scene:
            assetsTable["load_scene"] = [&](const std::string& filepath, bool setAsCurrent) -> scene_handle
            {
                scene_handle handle = rContext.assetHub->load_asset(asset_type::scene, filepath, [this, setAsCurrent](asset_handle handle)
                {
                    if (setAsCurrent)
                        rContext.sceneHolder->set_current_scene(handle);
                });
                
                return handle;
            };
            assetsTable["load_scene_assets"] = [&](scene_handle handle) -> void
            {
                auto& filepath = rContext.assetHub->get_asset(handle)->metadata.filepath;
                rContext.assetHub->call_asset_loader(asset_type::scene_assets, filepath);
            };

            engineTable["assets"] = assetsTable;
        }

        (*mView)["_engine"] = engineTable;
    }

    void script_subsystem::initialize_event()
    {
        // Event data:
        
        {
            // Window
            auto type = mView->new_usertype<window_resize_event>("window_resize", sol::no_constructor);
            type["width"] = &window_resize_event::width;
            type["height"] = &window_resize_event::height;
        }

        {
            // Scene 
            auto createType = mView->new_usertype<scene_create_data>("scene_create", sol::no_constructor);
            createType["data"] = sol::object(); // TODO: decide whether to use the scene pointer or not 
        
            auto switchType = mView->new_usertype<scene_switch_data>("scene_switch", sol::no_constructor);
            switchType["data"] = sol::object(); // TODO: decide whether to use the scene pointer or not 
        }

        {
            // Asset 
            auto createType = mView->new_usertype<asset_loaded_event_data>("asset_loaded", sol::no_constructor);
            createType["handle"] = &asset_loaded_event_data::handle;
            createType["type"] = &asset_loaded_event_data::type;
        }

        // Event:
        sol::usertype<event> eventType = mView->new_usertype<event>("event", sol::no_constructor);
        eventType["type"] = &event::type;

        eventType["get"] = [](event& event, sol::state_view lua) -> sol::object
        {
            switch (event.type)
            {
                // case event_type::window_close:

                case event_type::window_resize:
                    return sol::make_object(lua, event.get<window_resize_event>());

                case event_type::scene_created:
                    return sol::make_object(lua, event.get<scene_create_data>());

                case event_type::scene_switched:
                    return sol::make_object(lua, event.get<scene_switch_data>());

                case event_type::asset_loaded:
                    return sol::make_object(lua, event.get<asset_loaded_event_data>());

                default:
                    return sol::object();
            }

            return sol::lua_nil;
        };
    }
}