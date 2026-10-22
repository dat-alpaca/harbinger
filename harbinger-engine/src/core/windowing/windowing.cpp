#include "windowing.hpp"
#include "platform/middleware.hpp"

namespace har
{
    void windowing_system::initialize(NON_OWNING event_queue* eventQueue, const platform::window_api_settings& api, const platform::window_settings& settings)
    {
        platform::initialize_windowing();

        {
            platform::pre_initialize(api);
            
            window = platform::window_initialize(settings, eventQueue);
            platform::window_set_user_pointer(&window);
            
            platform::post_initialize(window);
        }
    }
}