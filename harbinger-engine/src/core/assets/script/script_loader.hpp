#pragma once
#include <entt/entity/fwd.hpp>
#include <expected>
#include <filesystem>

#include "core/assets/asset.hpp"
#include "core/logging/logger.hpp"
#include "core/script/script_subsystem.hpp"
#include "core/assets/script/script_asset.hpp"

namespace har
{
    inline std::expected<script_asset, asset_load_error> load_script_task(const std::filesystem::path& filepath, NON_OWNING script_subsystem* scripts)
    {
        if (!std::filesystem::exists(filepath))
            return std::unexpected { asset_load_error::invalid_filepath };

        // TODO: allow compiling and/or loading binary.
        auto scriptFile = scripts->open_file(filepath);
        if (!scriptFile.valid())
            return std::unexpected { asset_load_error::failed_to_load };

        script_asset script { .self = scriptFile.call() };
        auto& self = script.self;

        if (self["on_event"].valid())
            script.hooks.onEvent = self["on_event"]; 

        if (self["on_update"].valid())
            script.hooks.onEvent = self["on_update"]; 

        return script;
    }

    inline void post_load_script_task(script_asset& script, entt::entity entity)
    {
        if (!script.self.valid())
        {
            HAR_LOG_CRITICAL("[Script]: Invalid script loaded.");
            return;
        }

        auto& self = script.self;
        self["id"] = entity;
    }

    inline void post_unload_script_task(script_asset& script)
    {
        script.self.abandon();
    }
}