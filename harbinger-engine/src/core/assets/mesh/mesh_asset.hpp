#pragma once
#include "core/defines.hpp"
#include "core/assets/asset.hpp"
#include "core/memory/memory_tag.hpp"
#include "core/containers/containers.hpp"
#include "rhi/default_layout.hpp"

namespace har
{
    struct mesh_offsets
    {
    public:
        mesh_offsets()
        {
            for (u32& i : mOffsets)
                i = invalid_handle;
        }

    public:
        void set_offset(vertex_layout layoutIndex, u32 value) { mOffsets[static_cast<u8>(layoutIndex)] = value; }
        u32 get_offset(vertex_layout layoutIndex) const { return mOffsets[static_cast<u8>(layoutIndex)]; }

    private:
        u32 mOffsets[DefaultDescriptor_Count];
    };

    struct mesh_asset : public asset
    {
        har_vector<memory_tag::assets, byte> vertices;
        har_vector<memory_tag::assets, byte> indices;
        mesh_offsets offsets;
    };
}