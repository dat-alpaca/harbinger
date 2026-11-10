#pragma once
#include "core/defines.hpp"
#include "core/event/event_queue.hpp"
#include "platform/window_callback.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace har::platform
{
    struct window_glfw
    { 
        GLFWwindow* window = nullptr;
        NON_OWNING event_queue* queue = nullptr;

        mouse_button_clbk mouseButtonCallback;
        scroll_clbk scrollCallback;
        character_clbk characterCallback;
        key_clbk keyCallback;
    };   
}