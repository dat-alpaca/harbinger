#pragma once
#include <glm/glm.hpp>
#include "core/defines.hpp"
#include "core/logging/logger.hpp"

namespace har
{
    enum class custom_format : u8
    {
        none,
        int32,
        float32,
        vec2,
        vec3,
        vec4,

        texture,
    };
    constexpr inline u64 get_format_byte_size(custom_format format)
    {
        switch (format)
        {
            case custom_format::none:
                return 0;
            
            case custom_format::int32: 
                return sizeof(i32);

            case custom_format::float32: 
                return sizeof(f32);

            case custom_format::vec2: 
                return sizeof(glm::vec2);

            case custom_format::vec3: 
                return sizeof(glm::vec3);

            case custom_format::vec4: 
                return sizeof(glm::vec4);

            case custom_format::texture: 
                return sizeof(u64);

            default:
            {
                HAR_LOG_ERROR("Invalid format byte size");
            } break;
        }

        return 0;
    }
    constexpr inline u64 get_format_final_byte_size(custom_format format)
    {
        switch (format)
        {
            case custom_format::none:
                return 0;
            
            case custom_format::int32: 
                return sizeof(i32);

            case custom_format::float32: 
                return sizeof(f32);

            case custom_format::vec2: 
                return sizeof(glm::vec2);

            case custom_format::vec3: 
                return sizeof(glm::vec4);

            case custom_format::vec4: 
                return sizeof(glm::vec4);

            case custom_format::texture: 
                return sizeof(handle_t);

            default:
            {
                HAR_LOG_ERROR("Invalid format byte size");
            } break;
        }

        return 0;
    } 
}