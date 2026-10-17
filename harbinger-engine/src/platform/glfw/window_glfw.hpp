#pragma once
#include "core/defines.hpp"
#include "core/event/event_queue.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace har::platform
{
    struct window_glfw
    { 
        GLFWwindow* window = nullptr;
        NON_OWNING event_queue* queue = nullptr;
    };   
}