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
        struct data
        {
            u32 byteOffset;
            u32 byteCount;
        };

    public:
        mesh_offsets()
        {
            for (data& i : mData)
            {
                i.byteOffset = invalid_handle;
                i.byteCount = 0;
            }
        }

    public:
        void set_data(vertex_layout layoutIndex, const data& value) { mData[static_cast<u8>(layoutIndex)] = value; }
        data get_data(vertex_layout layoutIndex) const { return mData[static_cast<u8>(layoutIndex)]; }

    private:
        data mData[DefaultDescriptor_Count];
    };

    struct mesh_asset : public asset
    {
        har_vector<memory_tag::assets, byte> vertices;
        har_vector<memory_tag::assets, byte> indices;
        mesh_offsets offsets;
        u32 vertexCount;
    };

    using mesh_handle = small_handle_t;
}