#include "core/engine/engine.hpp"
#include <harbinger.hpp>

int main(int argc, char** argv)
{
    using namespace har;

    context_t context;
    {
        harbinger_engine_initialize(context, "config.ini");
        context.scriptSubsystem.invoke_startup_script("res/startup.lua");
    }

    u64 total = 0;
    stopwatch stopwatch;
    while(harbinger_engine_is_running(context))
    {
        u64 durationMicro = stopwatch.elapsed();
        total += durationMicro;
        stopwatch.restart();

        if (0 && total >= 1'000'000)
        {
            f64 fps = 1.0f / (static_cast<f64>(durationMicro) / 1'000'000.0f);

            HAR_LOG_INFO("Timing:");
            HAR_LOG_WARN("* mspf : {}", durationMicro);
            HAR_LOG_WARN("* fps  : {}", fps);

            HAR_LOG_INFO("Memory Used: {}kB | {}kB", gMemoryStats.get_raw_bytes() / 1000, gMemoryStats.get_peak_bytes() / 1000);
            HAR_LOG_INFO("* Window : {}kB", gMemoryStats.get(memory_tag::windowing) / 1000);
            HAR_LOG_INFO("* Scripts: {}kB", gMemoryStats.get(memory_tag::script_system) / 1000);
            HAR_LOG_INFO("* Workers: {}B", gMemoryStats.get(memory_tag::worker_system));
            HAR_LOG_INFO("* Events : {}B", gMemoryStats.get(memory_tag::event_queue));
            HAR_LOG_INFO("* Logging: {}B", gMemoryStats.get(memory_tag::logging));
            HAR_LOG_INFO("* RHI    : {}B", gMemoryStats.get(memory_tag::rhi));
            HAR_LOG_INFO("* Assets : {}B", gMemoryStats.get(memory_tag::assets));
            
            total = 0;
        }

        harbinger_engine_start_frame(context);
        harbinger_engine_on_update(context);
    }

    harbinger_engine_shutdown(context);
    return 0;
}