#pragma once
#include <glm/glm.hpp>
#include "core/defines.hpp"
#include "core/utils/transform.hpp"
#include "core/memory/memory_tag.hpp"
#include "core/containers/containers.hpp"

#include "rhi/rhi.hpp"
#include "rhi/common/objects.hpp"
#include "renderer/draw_info.hpp"

namespace har
{
    struct instance_data
    {
        glm::mat4 model;
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
            return sizeof(instance_data) * maxInstances;
        }

    public:
        u64 maxDrawInfoPerFrame = DefaultMaxDrawInfoCount;
        u64 maxInstances = DefaultMaxDrawInfoCount;
    
    private:
        constexpr static inline u64 DefaultMaxDrawInfoCount = 16;
        constexpr static inline u64 DefaultMaxInstances = 16;
    };

    class harbinger_renderer
    {
    public:
        void initialize(NON_OWNING rhi_t* rhi, const harbinger_renderer_desc& desc);
        void render();

    public:
        void push_object(const transform_t& transform, graphics_pipeline_handle handle, bool isImmutable);
        void clear_draw_info();

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
        descriptor_set_layout_handle mainLayout;
        descriptor_set_handle mainSet;

        har_vector<memory_tag::renderer, instance_data> mInstanceBuffer;
        buffer_handle mInstanceSSBO;
    };
}