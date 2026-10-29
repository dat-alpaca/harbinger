#pragma once
#include <cstring>
#include <expected>
#include <filesystem>
#include <glm/glm.hpp>

#include "core/assets/mesh/mesh_asset.hpp"
#include "core/assets/asset_error.hpp"

#include "mesh_constants.hpp"
#include "rhi/default_layout.hpp"

namespace
{
    using namespace har;

    static inline void asset_load_default_triangle(mesh_asset& asset)
    {
        f32 positionData[] = 
        { 
            -0.5f, -0.5f, 0.0f,
             0.5f, -0.5f, 0.0f,
             0.5f,  0.5f, 0.0f,
        };
        f32 normalData[] = 
        { 
            0, 0, 1,
            0, 0, 1,
            0, 0, 1,
        };
        f32 uv0Data[] = 
        { 
            0.0f,  0.0f,
            1.0f,  0.0f,
            1.0f,  1.0f,
        };

        asset.vertexCount = 3;
        asset.vertices.resize(sizeof(positionData) + sizeof(normalData) + sizeof(uv0Data));

        u32 offset = 0;
        {
            std::memcpy(asset.vertices.data() + offset, positionData, sizeof(positionData));
            
            asset.offsets.set_data(vertex_layout::position, { .byteOffset = offset, .byteCount = sizeof(positionData) });
            offset += sizeof(positionData);
        }
        
        {
            std::memcpy(asset.vertices.data() + offset, normalData, sizeof(normalData));
            
            asset.offsets.set_data(vertex_layout::normals, { .byteOffset = offset, .byteCount = sizeof(normalData) });
            offset += sizeof(normalData);
        }

        {
            std::memcpy(asset.vertices.data() + offset, uv0Data, sizeof(uv0Data));
            asset.offsets.set_data(vertex_layout::uv0, { .byteOffset = offset, .byteCount = sizeof(uv0Data) });
        }

        u32 indices[] = { 0, 1, 2 };
        asset.indices.insert(asset.indices.end(), indices, indices + std::size(indices));
    }

    static inline void asset_load_default_quad(mesh_asset& asset)
    {
        f32 _positionData[] = 
        { 
            -0.5f, -0.5f, 0.0f,
             0.5f, -0.5f, 0.0f,
             0.5f,  0.5f, 0.0f,
            -0.5f,  0.5f, 0.0f,
        };
        f32 positionData[] = 
        { 
            -0.5f, -0.5f, 0.0f,
             0.5f, -0.5f, 0.0f,
             0.5f,  0.5f, 0.0f,
             0.5f,  0.5f, 0.0f,
            -0.5f,  0.5f, 0.0f,
            -0.5f, -0.5f, 0.0f,
        };
        f32 normalData[] = 
        { 
            0, 0, 1,
            0, 0, 1,
            0, 0, 1,
            0, 0, 1,
            0, 0, 1,
            0, 0, 1,
        };
        f32 uv0Data[] = 
        { 
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f,
            0.0f, 1.0f,
            0.0f, 1.0f,
        };

        asset.vertexCount = 6;
        asset.vertices.resize(sizeof(positionData) + sizeof(normalData) + sizeof(uv0Data));

        u32 offset = 0;
        {
            std::memcpy(asset.vertices.data() + offset, positionData, sizeof(positionData));
            
            asset.offsets.set_data(vertex_layout::position, { .byteOffset = offset, .byteCount = sizeof(positionData) });
            offset += sizeof(positionData);
        }
        
        {
            std::memcpy(asset.vertices.data() + offset, normalData, sizeof(normalData));
            
            asset.offsets.set_data(vertex_layout::normals, { .byteOffset = offset, .byteCount = sizeof(normalData) });
            offset += sizeof(normalData);
        }

        {
            std::memcpy(asset.vertices.data() + offset, uv0Data, sizeof(uv0Data));
            asset.offsets.set_data(vertex_layout::uv0, { .byteOffset = offset, .byteCount = sizeof(uv0Data) });
        }

        u32 indices[] = { 0, 1, 2, 2, 3, 0 };
        asset.indices.insert(asset.indices.end(), indices, indices + std::size(indices));
    }
}

namespace har
{
    inline std::expected<mesh_asset, asset_load_error> asset_mesh_load(const std::filesystem::path& filepath)
    {
        mesh_asset asset;

        if (filepath == TriangleMeshFilepath)
        {
            asset_load_default_triangle(asset); 
            return asset;
        }
        else if (filepath == QuadMeshFilepath)
        {
            asset_load_default_quad(asset); 
            return asset;
        }
        else 
            return std::unexpected { asset_load_error::failed_to_load };

        return asset;
    }
}