#include "core/defines.hpp"
#include "core/engine/engine.hpp"
#include "core/logging/logger.hpp"
#include "core/memory/memory_stats.hpp"
#include "core/memory/memory_tag.hpp"
#include "core/subsystems/asset_handler.hpp"
#include "core/utils/time.hpp"

#include "platform/window.hpp"

using namespace har;

static void initialize(harbinger_engine& engine)
{
    /*
    auto& rhi = engine.rhi;

    // Textures:
    texture_handle texture = rhi.create_texture
    ({ 
        .width = 800, 
        .height = 640, 
        .dimension = har::texture_dimension::texture_2d, 
        .isRenderTarget = true 
    });
    
    sampler_handle textureSampler = rhi.create_sampler({});

    // Depth texture:
    texture_handle depth = rhi.create_texture
    ({ 
        .width = 800, 
        .height = 640, 
        .depth = 1,
        .dimension = har::texture_dimension::texture_2d, 
        .internalFormat = format::d24_s8,
        .isRenderTarget = true,
    });

    sampler_handle depthSampler = rhi.create_sampler({});

    // Render target:
    render_target_attachment colorAttachments[] = {{ .texture = texture, .sampler = textureSampler }};
    render_target_descriptor renderTargetDescriptor;
    {
        renderTargetDescriptor.colorAttachments = colorAttachments;
        renderTargetDescriptor.depthAttachment = { .texture = depth, .sampler = depthSampler };
        renderTargetDescriptor.width  = 800;
        renderTargetDescriptor.height = 640;
    }
    renderTarget = rhi.create_render_target(renderTargetDescriptor);
    */
}

static void initialize_assets(harbinger_engine& engine)
{
    engine.context.scriptEngine.invoke_startup_script("res/startup.lua");
}

/*
static void render_static(harbinger_engine& engine)
{
    auto& renderer = engine.renderer;
    auto& rhi = engine.rhi;
    if (!rhi.begin_frame())
        return;

    i32 w, h;
    platform::window_get_dimensions(engine.windowing.window, w, h);

    renderer.render();

    rhi.end_frame();
    rhi.submit({});
    rhi.present({});
}
*/

int main(int argc, char** argv)
{
    harbinger_engine engine("config.ini");
    initialize(engine);
    initialize_assets(engine);

    u64 total = 0;
    stopwatch stopwatch;
    while(engine.is_running())
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

        engine.start_frame();
        engine.on_update();

        // render_static(engine);
    }

    return 0;
}