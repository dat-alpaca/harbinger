/* https://github.com/skaarj1989/entt-meets-sol2/blob/master/examples/system/main.cpp */

#pragma once
#include <sol/sol.hpp>
#include "core/event/event.hpp"

namespace har
{
    struct script_asset
    {
    public:
        void on_update()
        {
            if (!hooks.onUpdate.valid())
                return;

            hooks.onUpdate();
        }

        void on_event(event& event)
        {
            if (!hooks.onEvent.valid())
                return;

            hooks.onUpdate(event);
        }

    public:
        sol::table self;
        struct
        {
            sol::function onUpdate;
            sol::function onEvent;
        } hooks;
    };
}