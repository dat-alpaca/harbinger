#pragma once
#include <glm/glm.hpp>

#include "rhi/common/objects.hpp"
#include "rhi/rhi.hpp"

#include "core/containers/containers.hpp"
#include "core/defines.hpp"
#include "core/memory/memory_tag.hpp"

#include "renderer/mesh.hpp"
#include "renderer/material.hpp"
#include "renderer/sort_key.hpp"

namespace har
{
    // TODO: make an asset cache subsystem (with loader functions that calls the worker pool)
    // TODO: use the asset cache to retrieve mesh, texture, materials (shaders), etc.

    class harbinger_renderer
    {
        struct draw_info
        {
            sort_key sortKey;
            u64 offset;
        };

        struct draw_group
        {
            sort_key sortKey;
            u32 instanceCount = 0;
        };

        struct object_data
        {
            glm::mat4 model;
        };

    public:
        static inline u64 get_required_transient_memory(u64 maxDrawInfoPerFrame) { return maxDrawInfoPerFrame * sizeof(draw_info); }
        static inline u64 get_required_static_memory(u16 maxMaterialInstances, u16 maxMeshes) 
        { 
            return maxMaterialInstances * sizeof(material_instance) +
                   maxMeshes * sizeof(mesh);
        }

    public:
        void initialize(NON_OWNING rhi_t* rhi, u64 maxDrawInfoPerFrame, u16 maxMaterialInstances = 65535, u16 maxMeshes = 65535);

    public:
        void insert_object(mesh_handle mesh, material_instance_handle materialInstance)
        {   
            // TODO: use the asset cache
            // auto& instance = mResources.material_instances()[materialInstance];

            mDrawInfo.emplace_back
            (draw_info {
                { 
                    //.pipeline = instance.pipeline,
                    //.meshHandle = mesh,
                    //.materialInstanceHandle = materialInstance,
                }, mCurrentOffset++ }
            );
        }
    
        void render();

    private:
        void prepare_draw_calls();
        void issue_render_commands();
        void finish_rendering();

        void rebind(const sort_key& key);

    private:
        command_buffer_t mCommandBuffer;
        command_buffer_t mTransferBuffer;
        NON_OWNING rhi_t* rRHI = nullptr;

    private:
        har_transient_vector<memory_tag::rhi, draw_info> mDrawInfo;
        har_transient_vector<memory_tag::rhi, draw_group> mDrawGroups;
        u32 mCurrentOffset = 0;

    private:
        graphics_pipeline_handle mCurrentPipeline = invalid_small_handle;
        mesh_handle mCurrentMeshHandle            = invalid_small_handle;
        mesh mCurrentMesh;

        buffer_handle mObjectDataSSBO             = invalid_small_handle;
        descriptor_set_handle mMainSet            = invalid_small_handle;
        descriptor_set_layout_handle mSetLayout   = invalid_small_handle;

        render_target_handle mRenderTarget        = invalid_small_handle;

    private:
        static inline u64 MaxObjectDataSize = 10'000 * sizeof(harbinger_renderer::object_data);
    };
}