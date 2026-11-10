#include "core/gui/gui_subsystem.hpp"
#include <harbinger.hpp>

using namespace har;

int main()
{
    context_t context;
    harbinger_engine_initialize(context, "config.ini");

    gui_subsystem sub;
    sub.initialize(&context.windowing.window, &context.renderer);

    while(harbinger_engine_is_running(context))
    {
        harbinger_engine_start_frame(context);
        harbinger_engine_on_update(context);

        {
            context.rendererSystem.on_render();
            context.rhi.submit();
            context.rhi.present();
        }
    }

    harbinger_engine_shutdown(context);

    return 0;
}