#include "platform/middleware.hpp"
#include "core/assert/assert.hpp"

#define GLFW_INCLUDE_NONE
#include <glad.h>
#include <GLFW/glfw3.h>

namespace har::platform
{
    void pre_initialize(const window_api_settings& settings)
    {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, settings.major);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, settings.minor);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    }

    void post_initialize(window_t& window)
    {
        HAR_ASS(window.window);
        glfwMakeContextCurrent(window.window);

        HAR_ASS(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress));
    }

    void present(window_t& window)
    {
        glfwSwapBuffers(window.window);
    }

    u32 get_required_windowing_dynamic_memory()
    {
        return MiB(32);
    }
}
