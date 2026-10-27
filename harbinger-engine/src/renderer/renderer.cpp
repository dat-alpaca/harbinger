#include "renderer.hpp"
#include "renderer/sort_key.hpp"

#include "core/defines.hpp"
#include "core/assert/assert.hpp"
#include "rhi/common/objects.hpp"
#include "rhi/default_layout.hpp"

namespace har
{
    using namespace har::internal;

    void harbinger_renderer::initialize(NON_OWNING rhi_t* rhi, const harbinger_renderer_desc& desc)
    {
        HAR_ASS(rhi);

        rRHI = rhi;
        mCommandBuffer = rRHI->create_command_buffer();
        mTransferBuffer = rRHI->create_command_buffer();

        mMaxDrawInfoPerFrame = desc.maxDrawInfoPerFrame;
        
        clear_draw_info();

        {
            // Instance:
            buffer_descriptor descriptor;
            descriptor.bufferUsage = buffer_usage::storage | buffer_usage::coherent | buffer_usage::persistent | buffer_usage::write;
            descriptor.sizeBytes = sizeof(instance_data) * desc.maxInstances;

            mInstanceSSBO = rRHI->create_buffer(descriptor);
            mInstanceBuffer.reserve(descriptor.sizeBytes);

            // User:
            descriptor.sizeBytes = sizeof(byte) * desc.maxInstances * desc.maxDataPerInstance;
            mUserSSBO = rRHI->create_buffer(descriptor);
            mUserBuffer.reserve(descriptor.sizeBytes);
        }

        {
            descriptor_set_layout layout;
            layout.items.push_back({ descriptor_set_type::storage_buffer, pipeline_stage::vertex, DefaultDescriptor_Position, 1,});
            layout.items.push_back({ descriptor_set_type::storage_buffer, pipeline_stage::vertex, DefaultDescriptor_Normals, 1,});
            layout.items.push_back({ descriptor_set_type::storage_buffer, pipeline_stage::vertex, DefaultDescriptor_UV0, 1,});
            layout.items.push_back({ descriptor_set_type::storage_buffer, pipeline_stage::vertex, DefaultDescriptor_Instance, 1,});
            layout.items.push_back({ descriptor_set_type::storage_buffer, pipeline_stage::vertex, DefaultDescriptor_User, 1,});

            mainLayout = rRHI->create_descriptor_set_layout(layout);
            mainSet = rRHI->create_descriptor_set(mainLayout);

            // TODO: remaining ssbos
            rRHI->write_storage_buffer_to_set(mainSet, mInstanceSSBO, DefaultDescriptor_Instance);
            rRHI->write_storage_buffer_to_set(mainSet, mUserSSBO, DefaultDescriptor_User);
        }
    }
    void harbinger_renderer::render()
    {
        if(!rRHI->begin_frame() || mDrawInfo.empty())
            return;

        prepare_draw_info();

        mCommandBuffer.begin();
        {
            for (const auto& group : mDrawGroups)
            {
                rebind(group);
                mCommandBuffer.draw(1, group.instanceCount, 0, group.offset);
            }

            mCommandBuffer.end();
        }
        rRHI->execute(mCommandBuffer);

        rRHI->end_frame();
    }

    void harbinger_renderer::push_object(const transform_t& transform, graphics_pipeline_handle handle, std::span<const byte> userData, bool isImmutable)
    {
        mDrawInfo.push_back(draw_info 
        {
            { .pipeline = handle }, 
            mCurrentOffset++ 
        });

        mInstanceBuffer.push_back( { .model = transform.get_transform_matrix() });
        mUserBuffer.insert(mUserBuffer.end(), userData.begin(), userData.end());
    }

    void harbinger_renderer::prepare_draw_info()
    {
        std::sort(mDrawInfo.begin(), mDrawInfo.end(), [](const draw_info& lhs, const draw_info& rhs)
        {
            return lhs.sortKey.pack() < rhs.sortKey.pack();
        });

        for (const auto& info : mDrawInfo)
        {
            if (mDrawGroups.empty() || mDrawGroups.back().sortKey.pack() != info.sortKey.pack())
                mDrawGroups.push_back(
                { 
                    .instanceCount = 1, 
                    .offset = info.offset, 
                    .sortKey = info.sortKey, 
                });
            else
                mDrawGroups.back().instanceCount++;
        }
    
        mTransferBuffer.begin();
        mTransferBuffer.update_buffer
        (
            mInstanceSSBO, std::span<const byte>
            (
                reinterpret_cast<const byte*>(mInstanceBuffer.data()), 
                mInstanceBuffer.size() * sizeof(instance_data)
            ), 
            0
        );
        mTransferBuffer.update_buffer
        (
            mUserSSBO, std::span<const byte>
            (
                reinterpret_cast<const byte*>(mUserBuffer.data()), 
                mUserBuffer.size() * sizeof(byte)
            ), 
            0
        );
        mTransferBuffer.end();
    }

    void harbinger_renderer::rebind(const draw_group& group)
    {
        if (mBoundPipeline != group.sortKey.pipeline)
        {
            mBoundPipeline = group.sortKey.pipeline; 
            mCommandBuffer.bind_graphics_pipeline(mBoundPipeline);
            mCommandBuffer.bind_descriptor_set(mainSet);
        }
    }

    void harbinger_renderer::clear_draw_info()
    {
        mBoundPipeline = invalid_small_handle;
        mCurrentOffset = 0;

        mDrawInfo.clear();
        mDrawGroups.clear();

        mDrawInfo.reserve(mMaxDrawInfoPerFrame);
        mDrawGroups.reserve(mMaxDrawInfoPerFrame);
        
        mInstanceBuffer.clear();
        mUserBuffer.clear();
    }
}