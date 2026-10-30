#pragma once
#include <glm/glm.hpp>
#include "core/assets/asset.hpp"
#include "core/assets/mesh/mesh_asset.hpp"
#include "core/containers/nano_free_list.hpp"
#include "core/defines.hpp"
#include "core/utils/transform.hpp"
#include "core/memory/memory_tag.hpp"
#include "core/containers/containers.hpp"

#include "rhi/default_layout.hpp"
#include "rhi/rhi.hpp"
#include "rhi/common/objects.hpp"
#include "renderer/draw_info.hpp"

namespace har
{
    struct instance_data
    {
        glm::mat4 model;
    };

    struct gpu_mesh_offset
    {
        u32 offset[DefaultDescriptor_Count];
        u32 count;
    };

    struct harbinger_renderer_desc
    {
    public:
        inline u64 get_required_transient_memory()
        {
            return 
                sizeof(draw_info)  * maxDrawInfoPerFrame +
                sizeof(draw_group) * maxDrawInfoPerFrame;
        }
        inline u64 get_required_static_memory() 
        { 
            return 
                sizeof(instance_data) * maxInstances +
                sizeof(byte) * maxInstances * DefaultMaxDataPerInstance +
                sizeof(glm::vec4) * maxInstances +
                nano_free_list<gpu_mesh_offset>::required_memory(maxMeshes);
        }
        inline u64 get_required_dynamic_memory() 
        { 
            return 
                sizeof(glm::vec4) * DefaultMaxPositions +
                sizeof(glm::vec4) * DefaultMaxNormals +
                sizeof(glm::vec2) * DefaultMaxUV0s;
        }

    public:
        u64 maxDrawInfoPerFrame = DefaultMaxDrawInfoCount;
        u64 maxInstances = DefaultMaxInstances;
        u32 maxDataPerInstance = DefaultMaxDataPerInstance;
        u32 maxMeshes = DefaultMaxMeshes;

        u32 maxPositions = DefaultMaxPositions;
        u32 maxNormals = DefaultMaxNormals;
        u32 maxUV0s = DefaultMaxUV0s;
    
    private:
        constexpr static inline u64 DefaultMaxDrawInfoCount = 16;
        constexpr static inline u64 DefaultMaxInstances = 16;
        constexpr static inline u64 DefaultMaxDataPerInstance = sizeof(float) * 8;
        constexpr static inline u64 DefaultMaxMeshes = 8;

        constexpr static inline u64 DefaultMaxPositions = 32;
        constexpr static inline u64 DefaultMaxNormals = 32;
        constexpr static inline u64 DefaultMaxUV0s = 16;
    };

    class harbinger_renderer
    {
    public:
        void initialize(NON_OWNING rhi_t* rhi, const harbinger_renderer_desc& desc);
        void render();

    public:
        void push_object(const transform_t& transform, graphics_pipeline_handle handle, mesh_handle mesh, std::span<const byte> userData = {}, bool isImmutable = false);
        void clear_draw_info();
        
        mesh_handle push_mesh(asset_handle assetHandle, const mesh_asset& asset);
        void clear_mesh_data();

    private:
        void prepare_draw_info();
        void rebind(const draw_group& group);

    private:
        NON_OWNING rhi_t* rRHI = nullptr;
        command_buffer_t mCommandBuffer;
        command_buffer_t mTransferBuffer;
    
    private:
        har_transient_vector<memory_tag::renderer, draw_info> mDrawInfo;
        har_transient_vector<memory_tag::renderer, draw_group> mDrawGroups;
        u32 mMaxDrawInfoPerFrame = 0;

    private:
        graphics_pipeline_handle mBoundPipeline = invalid_small_handle;
        u32 mCurrentOffset = 0;

    private:
        descriptor_set_layout_handle mainLayout = invalid_small_handle;
        descriptor_set_handle mainSet = invalid_small_handle;

        har_vector<memory_tag::renderer, glm::vec4> mPositionBuffer;
        u32 mPositionByteOffset = 0;

        har_vector<memory_tag::renderer, glm::vec4> mNormalBuffer;
        u32 mNormalByteOffset = 0;

        har_vector<memory_tag::renderer, glm::vec4> mUV0Buffer;
        u32 mUV0ByteOffset = 0;

        har_vector<memory_tag::renderer, instance_data> mInstanceBuffer;
        har_vector<memory_tag::renderer, byte> mUserBuffer;
        
        buffer_handle mPositionSSBO = invalid_small_handle;
        buffer_handle mNormalSSBO = invalid_small_handle;
        buffer_handle mUV0SSBO = invalid_small_handle;
        buffer_handle mInstanceSSBO = invalid_small_handle;
        buffer_handle mUserSSBO = invalid_small_handle;

    private:
        har_unordered_map<asset_handle, mesh_handle> mMeshMap;
        nano_free_list<gpu_mesh_offset> mGPUOffsets;
    };
}