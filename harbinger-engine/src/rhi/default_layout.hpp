#pragma once
#include "core/defines.hpp"

namespace har
{
    constexpr u8 DefaultDescriptor_Position = 0;
    constexpr u8 DefaultDescriptor_Normals  = 1;
    constexpr u8 DefaultDescriptor_UV0      = 2;
    constexpr u8 DefaultDescriptor_Instance = 3;
    constexpr u8 DefaultDescriptor_User     = 4;
    constexpr u8 DefaultDescriptor_Count    = 5;

    enum class vertex_layout : u8
    {
        position = DefaultDescriptor_Position,
        normals  = DefaultDescriptor_Normals,
        uv0      = DefaultDescriptor_UV0,
        instance = DefaultDescriptor_Instance,
        user     = DefaultDescriptor_User,
        count,
    };

    constexpr inline vertex_layout operator+(vertex_layout lhs, vertex_layout rhs)
    {
        u8 dataLHS = static_cast<u8>(lhs);
        u8 dataRHS = static_cast<u8>(rhs);
        return static_cast<vertex_layout>(dataLHS + dataRHS);
    }
    constexpr inline vertex_layout& operator+=(vertex_layout& lhs, const vertex_layout& rhs)
    {
        lhs = static_cast<vertex_layout>(lhs + rhs);
        return lhs;
    }
}