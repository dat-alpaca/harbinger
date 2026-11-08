#include "core/logging/logger.hpp"
#include "platform/middleware.hpp"
#include "core/assert/assert.hpp"

#define GLFW_INCLUDE_NONE
#include <glad.h>
#include <GLFW/glfw3.h>

using namespace har;

static void APIENTRY gl_debug_output(u32 source, u32 type, u32 id,  u32 severity, i32 length, const char* message, const void* userParam)
{
    // https://learnopengl.com/In-Practice/Debugging
    if(id == 131169 || id == 131185 || id == 131218 || id == 131204)
        return; 

    har_string sourceStr;
    switch (source)
    {
        case GL_DEBUG_SOURCE_API:
            sourceStr = "API";
            break;

        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            sourceStr = "Window System";
            break;

        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            sourceStr = "Shader Compiler";
            break;

        case GL_DEBUG_SOURCE_THIRD_PARTY:     
            sourceStr = "Third Party"; 
            break;

        case GL_DEBUG_SOURCE_APPLICATION:     
            sourceStr = "Application";
            break;

        case GL_DEBUG_SOURCE_OTHER:           
            sourceStr = "Other";
            break;
    }

    har_string typeStr;
    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:               
            typeStr = "Type: Error"; 
            break;

        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: 
            typeStr = "Type: Deprecated Behaviour";
            break;

        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  
            typeStr = "Type: Undefined Behaviour";
            break;

        case GL_DEBUG_TYPE_PORTABILITY:         
            typeStr = "Type: Portability"; 
            break;

        case GL_DEBUG_TYPE_PERFORMANCE:         
            typeStr = "Type: Performance"; 
            break;

        case GL_DEBUG_TYPE_MARKER:              
            typeStr = "Type: Marker"; 
            break;

        case GL_DEBUG_TYPE_PUSH_GROUP:          
            typeStr = "Type: Push Group";
            break;

        case GL_DEBUG_TYPE_POP_GROUP:
            typeStr = "Type: Pop Group";
            break;

        case GL_DEBUG_TYPE_OTHER:
            typeStr = "Type: Other"; 
            break;
    }

    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:
            HAR_LOG_ERROR("[{}][{}] {}: {}", sourceStr, type, id, message);
            break;

        case GL_DEBUG_SEVERITY_MEDIUM:
            HAR_LOG_WARN("[{}][{}] {}: {}", sourceStr, type, id, message);
            break;
        
        case GL_DEBUG_SEVERITY_LOW:
            HAR_LOG_INFO("[{}][{}] {}: {}", sourceStr, type, id, message);
            break;

        case GL_DEBUG_SEVERITY_NOTIFICATION:
            HAR_LOG_DEBUG("[{}][{}] {}: {}", sourceStr, type, id, message);
            break;
    }
}


namespace har::platform
{
    void pre_initialize(const window_api_settings& settings)
    {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, settings.major);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, settings.minor);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
        #ifdef HAR_DEBUG
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
        #endif
    }

    void post_initialize(window_t& window)
    {
        HAR_ASS(window.window);
        glfwMakeContextCurrent(window.window);

        i32 result = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        HAR_ASS(result);

        #ifdef HAR_DEBUG
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); 
        glDebugMessageCallback(gl_debug_output, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        #endif
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
