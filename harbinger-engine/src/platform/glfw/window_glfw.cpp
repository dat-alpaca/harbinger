#include "window_glfw.hpp"
#include "GLFW/glfw3.h"
#include "core/event/concrete/window_events.hpp"
#include "platform/window.hpp"

#include "core/defines.hpp"
#include "core/engine/engine.hpp"
#include "core/memory/memory_tag.hpp"

#include <cassert>

using namespace har;

// Memory:
static void* glfw_allocate(har::u64 size, void* user)
{
    return internal::MainDynamicAllocator.allocate(har::memory_tag::windowing, size);
}

static void glfw_deallocate(void* block, void* user)
{
    return internal::MainDynamicAllocator.deallocate(har::memory_tag::windowing, block);
}

static void* glfw_reallocate(void* block, har::u64 size, void* user)
{
    return internal::MainDynamicAllocator.reallocate(har::memory_tag::windowing, block, size);
}

// Callbacks:
static void glfw_on_window_resize(GLFWwindow* window, i32 width, i32 height)
{
    using namespace har::platform;
    window_glfw* mainWindow = static_cast<window_glfw*>(glfwGetWindowUserPointer(window));

    send_window_resize_event(*mainWindow->queue, { width, height });
}

static void glfw_on_window_close(GLFWwindow* window)
{
    using namespace har::platform;
    window_glfw* mainWindow = static_cast<window_glfw*>(glfwGetWindowUserPointer(window));

    send_window_close_event(*mainWindow->queue);
}

namespace har::platform
{
    void initialize_windowing(NON_OWNING harbinger_engine* engine)
    {
        GLFWallocator allocator =
        {
            .allocate = glfw_allocate,
            .reallocate = glfw_reallocate,
            .deallocate = glfw_deallocate,
            .user = reinterpret_cast<void*>(engine)
        };

        glfwInitAllocator(&allocator);

        assert(glfwInit());
    }

    window_glfw window_initialize(const window_settings& settings, NON_OWNING event_queue* queue)
    {
        assert(queue);
        assert(settings.title);
        assert(settings.minWidth);
        assert(settings.minHeight);

        window_glfw window { .queue = queue };
        {
            window.window = glfwCreateWindow(settings.minWidth, settings.minHeight, settings.title, nullptr, nullptr);
            window_set_vsync(window, settings.vsyncFrames);
            assert(window.window);
        }

        // Callbacks:
        glfwSetWindowCloseCallback(window.window, glfw_on_window_close);
        glfwSetWindowSizeCallback(window.window, glfw_on_window_resize);

        return window;
    }

    void window_set_user_pointer(window_t* window)
    {
        glfwSetWindowUserPointer(window->window, static_cast<void*>(window));
    }
    void window_set_vsync(const window_t& window, u32 frames)
    {
        glfwSwapInterval(frames);
    }

    bool window_is_open(const window_t& window)
    {
        return !glfwWindowShouldClose(window.window);
    }

    void window_poll_events([[maybe_unused]] const window_t&)
    {
        glfwPollEvents();
    }

    void window_hide(const window_t& window)
    {
        glfwHideWindow(window.window);
    }
    
    void window_show(const window_t& window)
    {
        glfwShowWindow(window.window);
    }

    void window_get_dimensions(const window_t& window, i32& width, i32& height)
    {
        glfwGetWindowSize(window.window, &width, &height);
    }
}
