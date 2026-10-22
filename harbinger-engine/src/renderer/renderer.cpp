#include "renderer.hpp"
#include "core/defines.hpp"
#include "renderer/sort_key.hpp"
#include "core/assert/assert.hpp"
#include "rhi/common/objects.hpp"

namespace har
{
    // TODO: config    
    void har_renderer::initialize(NON_OWNING rhi_t* rhi, u64 maxDrawInfoPerFrame, u16 maxMaterialInstances, u16 maxMeshes)
    {
        rRHI = rhi;

        // TODO: asset cache
        // mResources.initialize(&internal::MainStaticAllocator, maxMaterialInstances, maxMeshes);

        mDrawInfo.reserve(maxDrawInfoPerFrame * sizeof(draw_info));
        mDrawGroups.reserve(maxDrawInfoPerFrame * sizeof(draw_group));

        mCommandBuffer = rRHI->create_command_buffer();
        mTransferBuffer = rRHI->create_command_buffer();

        // Buffers:
        {
            buffer_descriptor ssboDescriptor = 
            {
                .bufferUsage = 
                    buffer_usage::storage | 
                    buffer_usage::read | 
                    buffer_usage::write |
                    buffer_usage::persistent |
                    buffer_usage::coherent,
                .sizeBytes = MaxObjectDataSize
            };
            mObjectDataSSBO = rRHI->create_buffer(ssboDescriptor);
        }

        // Descriptor Sets:
        descriptor_set_layout layout;
        {
            layout.items.push_back
            ({
                .type = descriptor_set_type::storage_buffer, 
                .stage = pipeline_stage::vertex,
                .binding = 0,
                .amount = 1,
            });
        }
        mSetLayout = rRHI->create_descriptor_set_layout(layout);
        mMainSet = rRHI->create_descriptor_set(mSetLayout);
    }

    void har_renderer::render()
    {
        prepare_draw_calls();

        issue_render_commands();

        finish_rendering();
    }

    void har_renderer::prepare_draw_calls()
    {
        if (mDrawInfo.empty())
            return;

        // Sorting:
        std::sort(mDrawInfo.begin(), mDrawInfo.end(), [](draw_info& lhs, draw_info& rhs) {
            return lhs.sortKey.pack() < rhs.sortKey.pack();
        });

        // Grouping:
        big_handle_t currentSortKey = invalid_big_handle;
        for (const auto& drawInfo : mDrawInfo)
        {
            big_handle_t sortKey = drawInfo.sortKey.pack();
            if (currentSortKey != sortKey)
            {
                mDrawGroups.push_back(draw_group { .sortKey = drawInfo.sortKey, .instanceCount = 1 });   
                currentSortKey = sortKey;
                continue;
            }

            mDrawGroups.back().instanceCount++;
        }

        // Buffer writing:
        rRHI->write_storage_buffer_to_set(mMainSet, mObjectDataSSBO, 0);
    }

    void har_renderer::issue_render_commands()
    {
        if (mDrawGroups.empty())
            return;

        mCommandBuffer.begin(mRenderTarget);
        mCommandBuffer.bind_descriptor_set(mMainSet);

        u32 firstInstance = 0;
        for (const draw_group& group : mDrawGroups)
        {
            rebind(group.sortKey);
            
            mCommandBuffer.draw_indexed
            (
                mCurrentMesh.indexCount,
                group.instanceCount,
                mCurrentMesh.firstVertex,
                firstInstance
            );

            firstInstance += group.instanceCount;
        }

        mCommandBuffer.end();
        rRHI->execute(mCommandBuffer);
    }

    void har_renderer::finish_rendering()
    {
        mDrawInfo.clear();
        mDrawGroups.clear();
        mCurrentOffset = 0;
    }
    
    void har_renderer::rebind(const sort_key& key)
    {
        HAR_ASS(key.meshHandle != invalid_small_handle);

        if (mCurrentPipeline != key.pipeline)
        {
            mCommandBuffer.bind_graphics_pipeline(key.pipeline);
            mCurrentPipeline = key.pipeline;
        }

        if (mCurrentMeshHandle != key.meshHandle && key.meshHandle != invalid_small_handle)
        {
            // TODO: asset cache
            // mCurrentMesh = mResources.meshes()[key.meshHandle];
            mCurrentMeshHandle = key.meshHandle;
        }
    }
}