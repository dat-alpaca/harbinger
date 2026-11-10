project(
    'harbinger', 
    'cpp', 'c', 
    meson_version: '>=1.2.0',
    default_options: 
    {
        'default_library': 'static',
        'cpp_std': 'c++23',
        'c_std': 'c11',
    }
)

# -- Global arguments:
add_global_arguments('-DGLEW_NO_GLU', language: 'cpp')

if get_option('buildtype') == 'debug'
    add_global_arguments('-DHAR_DEBUG', language: 'cpp')
endif

# -- CMake:
cmake = import('cmake')

# spirv-cross:
spirv_cmake_options = cmake.subproject_options()
spirv_cmake_options.add_cmake_defines({
    'SPIRV_CROSS_CLI': false,
    'SPIRV_CROSS_ENABLE_TESTS': false,
})

spirvc_subproject  = cmake.subproject('spirv-cross', options: spirv_cmake_options)

# libquish:
libsquish_cmake_opts = cmake.subproject_options()
libsquish_cmake_opts.add_cmake_defines({
    'LIBSQUISH_USE_OPENMP'    : false,
    'LIBSQUISH_USE_SSE2'      : false,
    'LIBSQUISH_USE_ALTIVEC'   : false,
    'LIBSQUISH_EXTRAS'        : false,
    'LIBSQUISH_DOCS'          : false,
    'LIBSQUISH_TESTS'         : false,
    'LIBSQUISH_INSTALL_MISC'  : false,
    'BUILD_SHARED_LIBS'       : false,
})
libsquish_sub  = cmake.subproject('libsquish', options: libsquish_cmake_opts)

# -- Dependencies:
dl_dep            = dependency('dl')
gl_dep            = dependency('gl')
glfw_dep          = dependency('glfw3')
glad_dep          = dependency('glad', default_options: { 'loader': true })
fmt_dep           = dependency('fmt')
spdlog_dep        = dependency('spdlog', default_options: { 'warning_level': '0' })
lua_dep           = dependency('lua', required: true, default_options: {'loadlib': 'disabled', 'default_library': 'static'})
tlsf_dep          = dependency('tlsf')
inih_dep          = dependency('inih')
glm_dep           = dependency('glm')
stb_dep           = dependency('stb')
entt_dep          = dependency('entt', include_type: 'system')
nlohmann_json_dep = dependency('nlohmann_json')
glslang_dep       = dependency('glslang')
sol2_dep          = dependency('sol2')
nuklear_dep       = dependency('nuklear')
spirvc_dep        = spirvc_subproject.dependency('spirv-cross-glsl') 
libsquish_dep     = libsquish_sub.dependency('libsquish_squish')

# -- Projects:
if get_option('build_engine')
    subdir('harbinger-engine')
endif

if get_option('build_testbed')
    subdir('harbinger-testbed')
endif

if get_option('build_editor')
    subdir('harbinger-editor')
endif

install_subdir(
    'res', 
    install_dir: '.', 
    install_tag: 'deploy'
)

install_data(
    'asset_bank.json',
    install_dir: '.', 
    install_tag: 'deploy'
)


install_data(
    'config.ini',
    install_dir: '.', 
    install_tag: 'deploy'
)