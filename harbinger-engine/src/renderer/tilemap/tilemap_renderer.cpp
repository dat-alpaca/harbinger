#include <span>

#include "core/defines.hpp"
#include "tilemap_renderer.hpp"
#include "core/utils/memory_utils.hpp"
#include "rhi/common/objects.hpp"

namespace har
{
    void tilemap_renderer::initialize(NON_OWNING rhi_t* rhi, u32 maxTileCount)
    {
        mTiles.resize(maxTileCount != 0 ? maxTileCount : DefaultMaxTile);

        rRHI = rhi;
        mCommandBuffer = rRHI->create_command_buffer();
        mTransferBuffer = rRHI->create_command_buffer();

        auto shaders = initialize_shaders();
        initialize_pipeline(std::move(shaders));
        initialize_buffers();
        initialize_descriptors();
    }

    tilemap_renderer::tilemap_shaders tilemap_renderer::initialize_shaders()
    {
        shader_handle vertexShader   = 0; // rRHI->create_shader(shader_type::vertex, vertexShaderData);
        shader_handle fragmentShader = 0; // rRHI->create_shader(shader_type::fragment, fragmentShaderData);

        return { vertexShader, fragmentShader };
    }

    void tilemap_renderer::initialize_pipeline(tilemap_renderer::tilemap_shaders&& shaders)
    {
        graphics_pipeline_descriptor pipelineDescriptor;
        pipelineDescriptor.vertexInput = 
        {
            .attributes = {},
            .stride = 0
        };
        pipelineDescriptor.inputAssembly.topology = topology_t::triangles;
        pipelineDescriptor.vertexShader = shaders.vertexShader;
        pipelineDescriptor.fragmentShader = shaders.fragmentShader;
        pipelineDescriptor.rasterizer.cullMode = cull_mode::back;
        pipelineDescriptor.rasterizer.frontFace = front_face::ccw;

        blend_attachment blendAttachments[] = { blend_attachment::alpha_blending() };
        pipelineDescriptor.colorBlending.attachments = blendAttachments;

        mPipeline = rRHI->create_graphics_pipeline(std::move(pipelineDescriptor));
    }

    void tilemap_renderer::initialize_buffers()
    {
        // Tiles SSBO:
        buffer_descriptor storageBufferDescriptor;
        {
            storageBufferDescriptor.bufferUsage = buffer_usage::storage | buffer_usage::persistent | buffer_usage::coherent;
            storageBufferDescriptor.sizeBytes = sizeof(tile) * 10000;
        }

        mSSBO = rRHI->create_buffer(storageBufferDescriptor);
        
        // Tilemap global data UBO:
        buffer_descriptor uniformBufferDescriptor;
        {
            uniformBufferDescriptor.bufferUsage = buffer_usage::uniform;
            uniformBufferDescriptor.sizeBytes = sizeof(world);
        }

        mUBO = rRHI->create_buffer(uniformBufferDescriptor);
    }

    void tilemap_renderer::initialize_descriptors()
    {
        // Set:
        descriptor_set_layout setLayout;
        setLayout.items.push_back
        ({
            .type = descriptor_set_type::storage_buffer,
            .stage = pipeline_stage::vertex,
            .binding = 0,
            .amount = 1
        });

        setLayout.items.push_back
        ({
            .type = descriptor_set_type::uniform_buffer,
            .stage = pipeline_stage::vertex,
            .binding = 1,
            .amount = 1
        });

        mSetLayout = rRHI->create_descriptor_set_layout(setLayout);

        // Descriptor:
        mSet = rRHI->create_descriptor_set(mSetLayout);
    }

    void tilemap_renderer::prepare_render()
    {
        // Buffers:
        mTransferBuffer.begin();
        mTransferBuffer.update_buffer(mSSBO, har::as_bytes(std::span<tile> { mTiles }), 0);
        mTransferBuffer.update_buffer(mUBO, har::as_bytes(std::span<world, 1> { &mWorldData, 1 }), 0);
        mTransferBuffer.end();
        rRHI->execute(mTransferBuffer);

        // Set:
        rRHI->write_storage_buffer_to_set(mSet, mSSBO, 0);
        rRHI->write_uniform_buffer_to_set(mSet, mUBO, 1);
    }
    
    void tilemap_renderer::render()
    {
        prepare_render();

        mCommandBuffer.begin(mRenderTarget);

        mCommandBuffer.bind_graphics_pipeline(mPipeline);
        mCommandBuffer.bind_descriptor_set(mSet);

        mCommandBuffer.end();
        rRHI->execute(mCommandBuffer);
    }

    void tilemap_renderer::push_tile(const tile& tile)
    {
        mTiles.push_back(tile);
    }
}