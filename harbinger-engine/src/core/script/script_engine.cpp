#include "script_engine.hpp"

#include <assert.h>
#include <memory>
#include <stdlib.h>

#include "core/assets/texture/texture_loader.hpp"
#include "core/defines.hpp"
#include "core/engine/engine.hpp"
#include "core/event/concrete/asset_events.hpp"
#include "core/event/concrete/scene_events.hpp"
#include "core/event/concrete/window_events.hpp"
#include "core/event/event.hpp"
#include "core/logging/logger.hpp"
#include "core/memory/allocators.hpp"
#include "core/memory/memory_tag.hpp"
#include "core/scene/scene.hpp"
#include "lualib.h"
#include "sol/forward.hpp"
#include "sol/load_result.hpp"
#include "sol/object.hpp"
#include "sol/overload.hpp"
#include "sol/raii.hpp"
#include "sol/state_view.hpp"
#include "sol/types.hpp"

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
    void script_engine::initialize(NON_OWNING harbinger_engine* engine)
    {
        rEngine = engine;

        mState = lua_newstate(lua_allocator, engine);
        assert(mState);

        mView = reinterpret_cast<sol::state_view*>(internal::MainStaticAllocator.push(sizeof(sol::state_view), alignof(u8)));
        std::construct_at(mView, mState);

        luaL_openlibs(mState);

        initialize_engine();
        initialize_event();
    }

    sol::load_result script_engine::open_file(const std::filesystem::path& filepath, file_type type)
    {
        return mView->load_file(filepath, get_load_mode(type));
    }

    void script_engine::open_script(const std::filesystem::path& filepath, file_type type)
    {
        mView->script_file(filepath, get_load_mode(type));
    }

    void script_engine::invoke_startup_script(const std::filesystem::path& filepath, file_type type)
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
    
    void script_engine::initialize_engine()
    {
        sol::table engineTable = mView->create_table();
        
        // Scene:
        sol::table scenesTable = mView->create_table();
        {
            scenesTable["set_current_scene"] = [&](scene_handle handle) {
                rEngine->sceneHolder.set_current_scene(handle);
            };
        
            engineTable["scenes"] = scenesTable;
        }
        
        // Assets:
        sol::table assetsTable = mView->create_table();
        {
            // Texture:
            mView->new_enum("asset_upload_options", 
                "none", asset_upload_options::none,
                "upload_immediate", asset_upload_options::upload_immediate
            );
            
            assetsTable["load_texture"] = 
            [&](const std::string& filepath, const sol::table& options, asset_upload_options assetOptions) -> asset_handle {
                if (!options.valid())
                {
                    HAR_LOG_ERROR("[Script Engine] invalid option arguments passed to load_scene.");
                    return invalid_small_handle;
                }

                texture_load_options textureOptions
                {
                    .desiredChannels = options["desired_channels"].get_or(DefaultChannelCount),
                    .freeHostMemory = options["free_host_memory"].get_or(false)
                };

                return rEngine->assetHandler.load_texture(filepath, textureOptions, assetOptions);
            };

            assetsTable["unload_texture"] = sol::overload(
                [&](scene_handle handle) {
                    rEngine->assetHandler.unload_texture(handle);
                },

                [&](std::string filepath) {
                    rEngine->assetHandler.unload_texture(filepath);
                }
            );

            // Scene:
            assetsTable["load_scene"] = [&](const std::string& filepath) -> scene_handle {
                return rEngine->assetHandler.load_scene(filepath);
            };

            engineTable["assets"] = assetsTable;
        }

        (*mView)["_engine"] = engineTable;
    }

    void script_engine::initialize_event()
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
            // Texture
            auto loadedType = mView->new_usertype<texture_loaded_data>("texture_load", sol::no_constructor);
            loadedType["handle"] = &texture_loaded_data::handle;
            loadedType["freeHostMemory"] = &texture_loaded_data::freeHostMemory;
            // options
            
            auto uploadRequestType = mView->new_usertype<texture_upload_request_data>("texture_upload_request", sol::no_constructor);
            uploadRequestType["handle"] = &texture_upload_request_data::handle;
            
            auto uploadedType = mView->new_usertype<texture_uploaded_data>("texture_upload", sol::no_constructor);
            uploadedType["assetHandle"] = &texture_uploaded_data::handle;
            uploadedType["textureHandle"] = &texture_uploaded_data::combinedTextureHandle;
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

                case event_type::texture_loaded:
                    return sol::make_object(lua, event.get<texture_loaded_data>());

                case event_type::texture_uploaded:
                    return sol::make_object(lua, event.get<texture_uploaded_data>());

                case event_type::request_texture_upload:
                    return sol::make_object(lua, event.get<texture_upload_request_data>());

                case event_type::pso_loaded:
                    return sol::make_object(lua, event.get<pso_loaded_data>());

                case event_type::pso_uploaded:
                    return sol::make_object(lua, event.get<pso_uploaded_data>());

                case event_type::request_pso_upload:
                    return sol::make_object(lua, event.get<pso_loaded_data>());

                default:
                    return sol::object();
            }

            return sol::lua_nil;
        };
    }
}