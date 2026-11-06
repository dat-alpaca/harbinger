#include "renderer.hpp"
#include "core/assets/asset_hub.hpp"
#include "core/assets/texture/texture_asset.hpp"
#include "core/memory/allocators.hpp"
#include "core/assets/custom_format.hpp"
#include "core/assets/mesh/mesh_asset.hpp"

#include "core/utils/uuid.hpp"
#include "renderer/sort_key.hpp"

#include "core/defines.hpp"
#include "core/assert/assert.hpp"
#include "rhi/common/objects.hpp"
#include "rhi/default_layout.hpp"
#include <cstring>

namespace har
{
    using namespace har::internal;

    void harbinger_renderer::initialize(NON_OWNING rhi_t* rhi, NON_OWNING asset_linker* assetLinker, const harbinger_renderer_desc& desc)
    {
        HAR_ASS(rhi);
        HAR_ASS(assetLinker);

        rRHI = rhi;
        rAssetLinker = assetLinker;

        mCommandBuffer = rRHI->create_command_buffer();
        mTransferBuffer = rRHI->create_command_buffer();
        mMaxDrawInfoPerFrame = desc.maxDrawInfoPerFrame;

        {
            // Attributes:
            mPositionBuffer.resize(desc.maxPositions * sizeof(glm::vec4));
            mNormalBuffer.resize(desc.maxNormals * sizeof(glm::vec4));
            mUV0Buffer.resize(desc.maxUV0s * sizeof(glm::vec2));

            // GPU Offsets:
            byte* backingBuffer = internal::MainStaticAllocator.push
            (
                mGPUOffsets.required_memory(desc.maxMeshes),
                1
            );
            mGPUOffsets.initialize(backingBuffer, desc.maxMeshes);
        }       

        clear_draw_info();

        {
            // Instance:
            buffer_descriptor descriptor;
            descriptor.bufferUsage = buffer_usage::storage | buffer_usage::coherent | buffer_usage::persistent | buffer_usage::write;
            descriptor.sizeBytes = sizeof(instance_data) * desc.maxInstances;

            mInstanceSSBO = rRHI->create_buffer(descriptor);
            mInstanceBuffer.reserve(descriptor.sizeBytes / sizeof(instance_data));

            // User:
            descriptor.sizeBytes = sizeof(byte) * desc.maxInstances * desc.maxDataPerInstance;
            mUserSSBO = rRHI->create_buffer(descriptor);
            mUserBuffer.reserve(descriptor.sizeBytes);

            // Position:
            descriptor.sizeBytes = sizeof(glm::vec4) * desc.maxPositions;
            mPositionSSBO = rRHI->create_buffer(descriptor);

            // Normals:
            descriptor.sizeBytes = sizeof(glm::vec4) * desc.maxNormals;
            mNormalSSBO = rRHI->create_buffer(descriptor);

            // UV0:
            descriptor.sizeBytes = sizeof(glm::vec2) * desc.maxUV0s;
            mUV0SSBO = rRHI->create_buffer(descriptor);

            // Texture entries:
            descriptor.sizeBytes = sizeof(texture_entry) * desc.maxTextureEntries;
            mTextureEntrySSBO = rRHI->create_buffer(descriptor);
        }

        {
            // Set Layout & Main Set:
            descriptor_set_layout layout;
            layout.items.push_back({ descriptor_set_type::storage_buffer, pipeline_stage::vertex, DefaultDescriptor_Position, 1 });
            layout.items.push_back({ descriptor_set_type::storage_buffer, pipeline_stage::vertex, DefaultDescriptor_Normals, 1 });
            layout.items.push_back({ descriptor_set_type::storage_buffer, pipeline_stage::vertex, DefaultDescriptor_UV0, 1 });
            layout.items.push_back({ descriptor_set_type::storage_buffer, pipeline_stage::vertex, DefaultDescriptor_Instance, 1 });
            layout.items.push_back({ descriptor_set_type::storage_buffer, pipeline_stage::both, DefaultDescriptor_User, 1 });
            layout.items.push_back({ descriptor_set_type::storage_buffer, pipeline_stage::both, DefaultDescriptor_TextureEntries, 1 });

            layout.items.push_back({ descriptor_set_type::combined_texture, pipeline_stage::fragment, DefaultDescriptor_Texture0 + 0, DefaultMaxArrayTextures});
            mainLayout = rRHI->create_descriptor_set_layout(layout);
            mainSet = rRHI->create_descriptor_set(mainLayout);
        }

        {
            rRHI->write_storage_buffer_to_set(mainSet, { mPositionSSBO }, DefaultDescriptor_Position);
            rRHI->write_storage_buffer_to_set(mainSet, { mNormalSSBO }, DefaultDescriptor_Normals);
            rRHI->write_storage_buffer_to_set(mainSet, { mUV0SSBO }, DefaultDescriptor_UV0);
            rRHI->write_storage_buffer_to_set(mainSet, { mInstanceSSBO }, DefaultDescriptor_Instance);
            rRHI->write_storage_buffer_to_set(mainSet, { mUserSSBO }, DefaultDescriptor_User);
            rRHI->write_storage_buffer_to_set(mainSet, { mTextureEntrySSBO }, DefaultDescriptor_TextureEntries);
        }

        {
            texture_descriptor descriptor;
            {
                // TODO: fetch api capabilities
                descriptor.arraySize = 2048;
                descriptor.depth = 1;
                descriptor.dimension = texture_dimension::texture_2d_array;
                descriptor.internalFormat = format_t::rgba_ui8;
                descriptor.levels = 1;
                descriptor.isRenderTarget = false;
            }

            sampler_descriptor samplerDescriptor;
            {
                samplerDescriptor.wrapS = sampler_wrap_mode::clamp_to_edge;
                samplerDescriptor.wrapT = sampler_wrap_mode::clamp_to_edge;
                samplerDescriptor.wrapR = sampler_wrap_mode::clamp_to_edge;

                samplerDescriptor.magFilter = sampler_filter::nearest;
                samplerDescriptor.minFilter = sampler_filter::nearest;

                samplerDescriptor.enableMipFiltering = false;
            }
            auto sampler = rRHI->create_sampler(samplerDescriptor);

            // Creates rgba_ui8 textures with sizes: 32x32, 64x64, and 96x96
            for (u8 i = 0; i < DefaultMaxArrayTextures; ++i)
            {
                auto id = i + 1;
                descriptor.width = 32 * id;
                descriptor.height = 32 * id;
                descriptor.layerWidth = 32 * id;
                descriptor.layerHeight = 32 * id;

                auto& arrayTexture = mArrayTextures[i];
                arrayTexture.textureHandle = rRHI->create_texture(descriptor);

                arrayTexture.combinedHandle = rRHI->create_combined_texture({.textureHandle = arrayTexture.textureHandle, .samplerHandle = sampler });
                arrayTexture.arraySize = 2048;
                arrayTexture.layerSize = descriptor.width;
                arrayTexture.format = format_t::rgba_ui8;

                rRHI->write_combined_image_to_set(mainSet, { arrayTexture.combinedHandle }, DefaultDescriptor_Texture0 + i);
            }

            {
                // Default Texture: 
                mTransferBuffer.begin();
                u8 data[32 * 32 * 4];
                std::memset(data, 255, 32 * 32 * 4);

                texture_asset textureAsset;
                {
                    textureAsset.width = 32;
                    textureAsset.height = 32;
                    textureAsset.channels = 4;
                    textureAsset.data = data;
                }

                mTransferBuffer.update_texture_layer
                (
                    mArrayTextures[DefaultTextureBin].textureHandle, 
                    textureAsset, 
                    format_t::rgba_ui8, 
                    data_type::unsigned_byte,
                    DefaultTextureLayer
                );
                mTransferBuffer.end();
                rRHI->execute(mTransferBuffer);

                mTextureEntryBuffer.clear();
                mTextureEntryBuffer.push_back({ .uvScale = { 1.0f, 1.0f }, .bin = DefaultTextureBin, .layer = DefaultTextureLayer });

                mArrayTextures[DefaultTextureBin].currentLayer++;
            }
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

                u32 meshOffset = 0;
                u32 count = 1;
                if (group.sortKey.mesh != invalid_small_handle)
                {
                    auto& offset = mGPUOffsets[group.sortKey.mesh];
                    meshOffset = offset.offset[DefaultDescriptor_Position];
                    count = offset.count;
                }

                mCommandBuffer.draw(count, group.instanceCount, meshOffset, group.offset);
            }

            mCommandBuffer.end();
        }
        rRHI->execute(mCommandBuffer);

        rRHI->end_frame();
    }

    void harbinger_renderer::push_object(const transform_t& transform, graphics_pipeline_handle handle, mesh_handle mesh, std::span<const byte> userData, bool isImmutable)
    {
        // TODO: somehow get instance user data

        mDrawInfo.push_back(draw_info 
        {
            .sortKey = 
            { 
                .pipeline = handle, 
                .mesh = mesh,
            }, 
            .offset = mCurrentOffset++,
        });

        u32 userOffset = 0;
        if (userData.size() != 0)
            userOffset = static_cast<u32>(mUserBuffer.size() / sizeof(float));
        mUserBuffer.insert(mUserBuffer.end(), userData.begin(), userData.end());

        mInstanceBuffer.push_back
        ({ 
            .model = transform.get_transform_matrix(),
            .userOffset = userOffset
        });
    }

    mesh_handle harbinger_renderer::push_mesh(asset_handle assetHandle)
    {
        if (mMeshMap.contains(assetHandle))
            return mMeshMap.at(assetHandle);

        const mesh_asset& asset = *static_cast<mesh_asset*>(rAssetLinker->get_asset_hub()->get_asset(assetHandle));

        mesh_handle handle = mGPUOffsets.insert({});
        mMeshMap[assetHandle] = handle;

        auto& gpuOffset = mGPUOffsets[handle];
        gpuOffset.count = asset.vertexCount;
        for (vertex_layout layout = vertex_layout {0}; layout < vertex_layout::count; layout += static_cast<vertex_layout>(1))
        {
            mesh_offsets::data meshOffsetData = asset.offsets.get_data(layout);
            auto* bufferData = static_cast<const byte*>(asset.vertices.data());

            switch (layout)
            {
                case vertex_layout::user:
                case vertex_layout::instance:
                    break;

                case vertex_layout::position:
                {
                    gpuOffset.offset[DefaultDescriptor_Position] = mPositionByteOffset / sizeof(glm::vec4);

                    const float* src = reinterpret_cast<const float*>(bufferData + meshOffsetData.byteOffset);
                    float* dst = reinterpret_cast<float*>(mPositionBuffer.data()) + (mPositionByteOffset / sizeof(float));

                    for (u64 i = 0; i < asset.vertexCount; ++i)
                    {
                        for (u64 j = 0; j < 3; ++j)
                            *dst++ = *src++;
                        *dst++ = 1.0f;
                    }
                    mPositionByteOffset += meshOffsetData.byteCount + (asset.vertexCount * sizeof(float));
                } break;

                case vertex_layout::normals:
                {
                    gpuOffset.offset[DefaultDescriptor_Normals] = mNormalByteOffset / sizeof(glm::vec4);

                    const float* src = reinterpret_cast<const float*>(bufferData + meshOffsetData.byteOffset);
                    float* dst = reinterpret_cast<float*>(mNormalBuffer.data()) + (mNormalByteOffset / sizeof(float));

                    for (u64 i = 0; i < asset.vertexCount; ++i)
                    {
                        for (u64 j = 0; j < 3; ++j)
                            *dst++ = *src++;
                        *dst++ = 0.0f;
                    }
                    mNormalByteOffset += meshOffsetData.byteCount + (asset.vertexCount * sizeof(float));
                } break;

                case vertex_layout::uv0:
                {
                    gpuOffset.offset[DefaultDescriptor_UV0] = mUV0ByteOffset / sizeof(glm::vec2);

                    const float* src = reinterpret_cast<const float*>(bufferData + meshOffsetData.byteOffset);
                    float* dst = reinterpret_cast<float*>(mUV0Buffer.data()) + (mUV0ByteOffset / sizeof(float));

                    for (u64 i = 0; i < asset.vertexCount; ++i)
                    {
                        *dst++ = *src++;
                        *dst++ = *src++;
                    }
                    mUV0ByteOffset += meshOffsetData.byteCount;
                } break;

                default:
                    break;
            }
        }

        return handle;
    }

    void harbinger_renderer::push_texture(asset_handle assetHandle, combined_texture_handle textureHandle)
    {
        auto* assetHub = rAssetLinker->get_asset_hub();
        auto* textureAsset = static_cast<texture_asset*>(assetHub->get_asset(assetHandle));

        auto textureUUID = textureAsset->metadata.assetUUID;

        if (mArrayTextureIdMap.contains(textureUUID))
            return;

        u32 bin = 0;
        auto& arrayTexture = mArrayTextures[bin];
        for (bin = 0; bin < mArrayTextures.size(); ++bin)
        {
            // TODO: accept different texture sizes closer to the nearest bin
            arrayTexture = mArrayTextures[bin];
            if (textureAsset->width != arrayTexture.layerSize)
                continue;
            break;
        }

        mTransferBuffer.begin();
        mTransferBuffer.copy_texture
        (
            textureHandle, 
            arrayTexture.textureHandle, 
            {
                .x = 0,
                .y = 0,
                .z = 0,
                .level = 0
            },
            {
                .x = 0,
                .y = 0,
                .z = 0,
                .level = arrayTexture.currentLayer
            },
            {
                .width = textureAsset->width,
                .height = textureAsset->width,
                .depth = 1
            }
        );
        mTransferBuffer.end();
        rRHI->execute(mTransferBuffer);

        u32 layer = arrayTexture.currentLayer++; 
        mTextureEntryBuffer.push_back({.uvScale = { 1.0f, 1.0f }, .bin = bin, .layer = layer});

        mArrayTextureIdMap[textureUUID] = mTextureEntryBuffer.size() - 1;
    }

    void harbinger_renderer::clear_mesh_data()
    {
        mMeshMap.clear();
        mGPUOffsets.clear();

        mPositionBuffer.clear();
        mNormalBuffer.clear();
        mUV0Buffer.clear();

        mPositionByteOffset = 0;
        mNormalByteOffset = 0;
        mUV0ByteOffset = 0;

        mTransferBuffer.begin();
        mTransferBuffer.update_buffer(mPositionSSBO, {}, 0);
        mTransferBuffer.update_buffer(mNormalSSBO, {}, 0);
        mTransferBuffer.update_buffer(mUV0SSBO, {}, 0);
        mTransferBuffer.end();
        rRHI->execute(mTransferBuffer);
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
    
        // Mesh:
        mTransferBuffer.begin();
        mTransferBuffer.update_buffer
        (
            mPositionSSBO, std::span<const byte>
            (
                reinterpret_cast<const byte*>(mPositionBuffer.data()), 
                mPositionByteOffset
            ), 
            0
        );
        mTransferBuffer.update_buffer
        (
            mNormalSSBO, std::span<const byte>
            (
                reinterpret_cast<const byte*>(mNormalBuffer.data()), 
                mNormalByteOffset
            ), 
            0
        );
        mTransferBuffer.update_buffer
        (
            mUV0SSBO, std::span<const byte>
            (
                reinterpret_cast<const byte*>(mUV0Buffer.data()), 
                mUV0ByteOffset
            ), 
            0
        );
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
        mTransferBuffer.update_buffer
        (
            mTextureEntrySSBO, std::span<const byte>
            (
                reinterpret_cast<const byte*>(mTextureEntryBuffer.data()), 
                mTextureEntryBuffer.size() * sizeof(texture_entry)
            ), 
            0
        );
        mTransferBuffer.end();
        rRHI->execute(mTransferBuffer);
    }

    void harbinger_renderer::rebind(const draw_group& group)
    {
        if (mBoundPipeline == group.sortKey.pipeline)
            return;
        
        mBoundPipeline = group.sortKey.pipeline; 
        mCommandBuffer.bind_graphics_pipeline(mBoundPipeline);
        mCommandBuffer.bind_descriptor_set(mainSet);
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