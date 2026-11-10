# Cross-platform files:
cross_platform = {
    'linux': {
        'platform': {
            'sources': files(
                'src/platform/linux/memory.cpp',
                'src/platform/linux/breakpoint.cpp',
                'src/platform/glfw/window_glfw.cpp',
                'src/platform/glfw/input.cpp',
            ),
            'deps': [dependency('x11')],
            'args': ['-DHAR_LINUX']
        },
        'rhi': {
            'opengl': files(
                'src/platform/glfw/gl_middleware.cpp'
            ),
        }
    }
}

# RHI files:
rhi = {
    'opengl': {
        'files': files(
            'src/rhi/opengl/rhi_gl.cpp',
            'src/rhi/opengl/command_buffer_gl.cpp',
            'src/rhi/opengl/resources.cpp',
            'src/rhi/opengl/rhi_arguments.cpp',
            'src/rhi/opengl/shader_compat.cpp',
        ),
        'args': ['-DHAR_GL']
    }
}

# Definitions:
harbinger_engine_include = include_directories('src/')
harbinger_engine_deps = [
    tlsf_dep,
    fmt_dep,
    spdlog_dep,
    gl_dep,
    glfw_dep,
    glad_dep,
    dl_dep,
    lua_dep,
    inih_dep,
    glm_dep,
    stb_dep,
    entt_dep,
    spirvc_dep,
    nlohmann_json_dep,
    libsquish_dep,
    glslang_dep,
    sol2_dep,
    nuklear_dep
]
args = []

sources = files(
    'src/core/implementation.cpp',
    
    'src/core/assets/asset_hub.cpp',
    'src/core/config/config.cpp',
    'src/core/engine/engine.cpp',

    'src/core/memory/memory_partitions.cpp',
    'src/core/memory/linear_allocator.cpp',
    'src/core/memory/dynamic_allocator.cpp',

    'src/core/script/script_subsystem.cpp',
    'src/core/windowing/windowing.cpp',

    'src/renderer/renderer.cpp',
    
    'src/core/gui/gui_subsystem.cpp',

    'src/core/bridges/gpu_uploader.cpp',

    'src/core/event/event_queue.cpp',

    'src/core/scene/scene.cpp',
    'src/core/scene/entity.cpp',

    'src/core/scene/scene_holder.cpp',
    'src/core/scene/systems/renderer_system.cpp',
    'src/core/scene/systems/asset_linker.cpp',

    'src/core/worker/worker_pool.cpp',

    'src/core/utils/json_serializable.cpp',

    'src/rhi/glsl_compiler.cpp',
)

# Platform-specific:
if host_machine.system() == 'windows'

elif host_machine.system() == 'linux'
    sources += cross_platform['linux']['platform']['sources']
    harbinger_engine_deps += cross_platform['linux']['platform']['deps']
    args += cross_platform['linux']['platform']['args']

    sources += cross_platform['linux']['rhi']['opengl']
endif

# Graphics API:
sources += rhi['opengl']['files']
args += rhi['opengl']['args']

# Library:
foreach arg : args
    add_project_arguments(arg, language: 'cpp')
endforeach

# STB:
c_args = []
c_args += 'STBI_NO_TGA=1' 
c_args += 'STBI_NO_GIF=1'
c_args += 'STBI_NO_PIC=1'
c_args += 'STBI_NO_PNM=1'

harbinger_engine = static_library(
    'harbinger-engine',
    sources: sources,
    c_args: c_args,
    cpp_args: args,
    dependencies: harbinger_engine_deps,
    include_directories: harbinger_engine_include
)

harbinger_engine_dep = declare_dependency(
    link_with : harbinger_engine,
    dependencies: harbinger_engine_deps,
    include_directories : harbinger_engine_include
)
