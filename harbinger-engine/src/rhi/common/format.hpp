#pragma once
#include "core/defines.hpp"

namespace har
{
    enum class format : u8
    {
        invalid = invalid_type_t<u8>,

        // 8-bit:
        r_ui8 = 0,             // R8 unsigned int
        // r_i8,               // R8 signed int
        // r_unorm8,           // R8 unsigned normalized
        r_snorm8,           // R8 signed normalized

        rg_ui8,             // RG8 unsigned int
        // rg_i8,              // RG8 signed int
        // rg_unorm8,          // RG8 unsigned normalized
        rg_snorm8,          // RG8 signed normalized

        rgb_ui8,            // RGB8 unsigned int
        // rgb_i8,             // RGB8 signed int
        // rgb_unorm8,         // RGB8 unsigned normalized
        rgb_snorm8,         // RGB8 signed normalized

        rgba_ui8,           // RGB8 unsigned int
        // rgba_i8,            // RGB8 signed int
        // rgba_unorm8,        // RGB8 unsigned normalized
        rgba_snorm8,        // RGB8 signed normalized

        // 16-bit:
        // r_ui16,             // R16 unsigned int
        // r_i16,              // R16 signed int
        // r_unorm16,          // R16 unsigned normalized
        // r_snorm16,          // R16 signed normalized
        // r_float16,          // R16 float

        // Depth & Stencil:
        d16,                    // Depth 16-bit
        d32,                    // Depth 32-bit
        d24_s8,                 // Depth 24-bit, Stencil 8-bit

        // Floats:
        rgb_f32,                // RGB Float 32

        // Compressed:
        dxt1_rgba,
        dxt3_rgba,
        dxt5_rgba,
    };
}