#include "windowing.hpp"
#include "core/engine/engine.hpp"
#include "platform/middleware.hpp"

namespace har
{
    void windowing_system::initialize(NON_OWNING harbinger_engine* engine, const platform::window_api_settings& api, const platform::window_settings& settings)
    {
        platform::initialize_windowing(engine);

        {
            platform::pre_initialize(api);
            
            window = platform::window_initialize(settings, engine->eventQueue.get());
            platform::window_set_user_pointer(&window);
            
            platform::post_initialize(window);
        }
    }
}