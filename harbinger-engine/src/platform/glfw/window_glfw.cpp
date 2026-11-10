#include "window_glfw.hpp"
#include "GLFW/glfw3.h"

#include "core/input/keys.hpp"
#include "platform/input.hpp"
#include "platform/window.hpp"

#include "core/defines.hpp"
#include "core/assert/assert.hpp"
#include "core/memory/memory_tag.hpp"
#include "core/event/concrete/window_events.hpp"

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
    void initialize_windowing()
    {
        GLFWallocator allocator =
        {
            .allocate = glfw_allocate,
            .reallocate = glfw_reallocate,
            .deallocate = glfw_deallocate,
            .user = nullptr
        };

        glfwInitAllocator(&allocator);

        i32 result = glfwInit();
        HAR_ASS(result);
    }

    window_glfw window_initialize(const window_settings& settings, NON_OWNING event_queue* queue)
    {
        HAR_ASS(queue);
        HAR_ASS(settings.title);
        HAR_ASS(settings.minWidth);
        HAR_ASS(settings.minHeight);

        window_glfw window { .queue = queue };
        {
            window.window = glfwCreateWindow(settings.minWidth, settings.minHeight, settings.title, nullptr, nullptr);
            window_set_vsync(window, settings.vsyncFrames);
            HAR_ASS(window.window);
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
    void window_get_framebuffer_dimensions(const window_t& window, i32& width, i32& height)
    {
        glfwGetFramebufferSize(window.window, &width, &height);
    }

    const char* window_get_clipboard_string(const window_t& window)
    {
        return glfwGetClipboardString(window.window);
    }

    void window_set_clipboard_string(const window_t& window, const char* string)
    {
        HAR_ASS(string);
        glfwSetClipboardString(window.window, string);
    }

    input_action window_get_key(const window_t& window, key_t key)
    {
        return get_input_action(glfwGetKey(window.window, get_native_key(key)));
    }
    input_action window_get_mouse_button(const window_t& window, mouse_button button)
    {
        return get_input_action(glfwGetMouseButton(window.window, get_native_mouse_button(button)));
    }
    void window_get_cursor_position(const window_t& window, f64* x, f64* y)
    {
        glfwGetCursorPos(window.window, x, y);
    }
    void window_set_cursor_position(const window_t& window, f64 x, f64 y)
    {
        glfwSetCursorPos(window.window, x, y);
    }

    f64 window_get_time_seconds(const window_t& window)
    {
        return glfwGetTime();
    }

    void window_show_cursor(const window_t& window)
    {
        glfwSetInputMode(window.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    void window_hide_cursor(const window_t& window)
    {
        glfwSetInputMode(window.window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    }
}

namespace har::platform
{
    void window_set_mouse_button_callback(window_t& window, mouse_button_clbk&& callback)
    {
        window.mouseButtonCallback = std::move(callback);

        glfwSetMouseButtonCallback(window.window, [](GLFWwindow* windowHandle, i32 button, i32 action, i32 mods)
        {
            window_glfw* mainWindow = static_cast<window_glfw*>(glfwGetWindowUserPointer(windowHandle));
            mainWindow->mouseButtonCallback(button, action, mods);
        });
    }

    void window_set_scroll_callback(window_t& window, scroll_clbk&& callback)
    {
        window.scrollCallback = std::move(callback);

        glfwSetScrollCallback(window.window, [](GLFWwindow* windowHandle, f64 x, f64 y)
        {
            window_glfw* mainWindow = static_cast<window_glfw*>(glfwGetWindowUserPointer(windowHandle));
            mainWindow->scrollCallback(x, y);
        });
    }

    void window_set_character_callback(window_t& window, character_clbk&& callback)
    {
        window.characterCallback = std::move(callback);

        glfwSetCharCallback(window.window, [](GLFWwindow* windowHandle, u32 codepoint)
        {
            window_glfw* mainWindow = static_cast<window_glfw*>(glfwGetWindowUserPointer(windowHandle));
            mainWindow->characterCallback(codepoint);
        });
    }

    void window_set_key_callback(window_t& window, key_clbk&& callback)
    {
        window.keyCallback = std::move(callback);

        glfwSetKeyCallback(window.window, [](GLFWwindow* windowHandle, i32 key, i32 scancode, i32 action, i32 mods)
        {
            window_glfw* mainWindow = static_cast<window_glfw*>(glfwGetWindowUserPointer(windowHandle));
            mainWindow->keyCallback(get_key(key), scancode, get_input_action(action), mods);
        });
    }
}